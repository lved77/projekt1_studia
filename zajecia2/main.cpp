#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class Graph;
using namespace std;
static void clearConsole();

// Klasa reprezentująca graf jako listę sąsiedztwa
class Graph {
    map<int, list<int>> graphRep; // mapa: wierzchołek -> lista sąsiadów

public:
    // Dodaje wierzchołek i jego połączenia do grafu
    void add_point(const int vertex, const list<int>& linkedEdges) {
        graphRep[vertex] = linkedEdges;
    }

    // Zwraca sąsiadów wierzchołka jako vector
    vector<int> getNeighbors(int vertex) const {
        auto it = graphRep.find(vertex);
        if (it == graphRep.end()) return {};
        vector<int> result;
        result.reserve(it->second.size());
        for (int v : it->second) result.push_back(v);
        return result;
    }

    // Sprawdza czy wierzchołek istnieje w grafie
    bool hasVertex(int vertex) const {
        return graphRep.find(vertex) != graphRep.end();
    }

    // Zwraca liczbę wierzchołków w grafie
    int vertexCount() const {
        return static_cast<int>(graphRep.size());
    }

    // Ładuje graf z pliku tekstowego - każda linia to lista sąsiadów wierzchołka
    static Graph loadFromFile(const string& path) {
        Graph g;
        ifstream fs(path);
        if (!fs.is_open()) {
            cerr << "Blad: nie moge otworzyc pliku z grafem: " << path << "\n";
            return g;
        }

        string line;
        int vertex = 1;
        while (getline(fs, line)) {
            list<int> linked;
            stringstream ss(line);
            string token;
            while (ss >> token) {
                try {
                    linked.push_back(stoi(token));
                } catch (...) {
                    // W razie dziwnego znaku po prostu ignorujemy
                }
            }
            g.add_point(vertex, linked);
            ++vertex;
        }

        return g;
    }

    // Znajduje najkrótszą ścieżkę między dwoma wierzchołkami używając BFS
    vector<int> shortestPath(int start, int goal) const {
        if (start == goal) return {start};
        unordered_map<int, int> parent;
        unordered_set<int> visited;
        queue<int> q;

        q.push(start);
        visited.insert(start);

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (int nb : getNeighbors(v)) {
                if (!visited.insert(nb).second) continue;
                parent[nb] = v;
                if (nb == goal) {
                    vector<int> path;
                    int cur = goal;
                    while (cur != start) {
                        path.push_back(cur);
                        cur = parent[cur];
                    }
                    path.push_back(start);
                    reverse(path.begin(), path.end());
                    return path;
                }
                q.push(nb);
            }
        }

        return {};
    }

    // Oblicza odległość między dwoma wierzchołkami (liczba krawędzi) używając BFS
    int distance(int start, int goal) const {
        if (start == goal) return 0;
        unordered_map<int, int> dist;
        queue<int> q;
        q.push(start);
        dist[start] = 0;

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            int dv = dist[v];

            for (int nb : getNeighbors(v)) {
                if (dist.find(nb) != dist.end()) continue;
                dist[nb] = dv + 1;
                if (nb == goal) return dist[nb];
                q.push(nb);
            }
        }

        return -1; // wierzchołki nie są połączone
    }
};

// Klasa bazowa dla wszystkich agentów w grze
class Agent {
protected:
    int id;

public:
    int position;
    Agent(int id, int starting_position): id(id), position(starting_position) {}
    virtual ~Agent() = default;

    // Wirtualna metoda - każdy agent decyduje o swoim następnym ruchu
    virtual int decideNextMove(const Graph& graph) = 0;
};

// Klasa reprezentująca ducha (Runner) - goni Seeker lub eksploruje graf
class Runner : public Agent {
    Graph& graph;

public:
    const int* seekerPosRef = nullptr; // referencja do pozycji Seeker

    Runner(int id, int starting_position, Graph& graph): Agent(id, starting_position), graph(graph) {}

    // Decyduje o następnym ruchu - goni Seeker jeśli jest blisko, w przeciwnym razie eksploruje graf
    int decideNextMove(const Graph& graph) override {
        // Jeśli Seeker jest w odległości 4 lub mniej, goni go najkrótszą ścieżką
        if (seekerPosRef) {
            int d = graph.distance(position, *seekerPosRef);
            if (d >= 0 && d <= 4) {
                auto path = graph.shortestPath(position, *seekerPosRef);
                if (path.size() >= 2) return path[1];
            }
        }

        // W przeciwnym razie eksploruje graf używając DFS
        static unordered_map<int, vector<int>> stackMap;
        static unordered_map<int, unordered_set<int>> visitedMap;

        auto& stackRef = stackMap[id];
        auto& visitedRef = visitedMap[id];

        if (stackRef.empty() || stackRef.back() != position) {
            stackRef.clear();
            visitedRef.clear();
            stackRef.push_back(position);
            visitedRef.insert(position);
        }

        for (int nb : graph.getNeighbors(position)) {
            if (!visitedRef.count(nb)) {
                visitedRef.insert(nb);
                stackRef.push_back(nb);
                return nb;
            }
        }

        if (!stackRef.empty()) {
            stackRef.pop_back();
            if (!stackRef.empty()) return stackRef.back();
        }

        return position;
    }
};

// Klasa reprezentująca Seeker (Pac-Man) - zbiera punkty i unika duchów
class Seeker : public Agent {
public:
    const unordered_set<int>* pointsRef = nullptr;          // referencja do zbioru punktów do zebrania
    const unordered_set<int>* runnerPositionsRef = nullptr; // referencja do pozycji duchów

    Seeker(int id, int starting_position): Agent(id, starting_position) {}

    // Decyduje o następnym ruchu - dąży do najbliższego punktu unikając duchów
    int decideNextMove(const Graph& graph) override {
        if (!pointsRef || pointsRef->empty()) return position;

        // Znajduje najbliższy punkt
        int closestPoint = -1;
        int closestDist = numeric_limits<int>::max();
        for (int p : *pointsRef) {
            int d = graph.distance(position, p);
            if (d >= 0 && d < closestDist) {
                closestDist = d;
                closestPoint = p;
            }
        }

        if (closestPoint == -1) return position;

        // Oznacza wierzchołki w promieniu 2 od duchów jako niebezpieczne
        unordered_set<int> danger;
        if (runnerPositionsRef) {
            for (int runner : *runnerPositionsRef) {
                queue<pair<int, int>> q;
                unordered_set<int> seen;
                q.push({runner, 0});
                seen.insert(runner);

                while (!q.empty()) {
                    auto [v, depth] = q.front();
                    q.pop();

                    if (depth > 2) continue;
                    danger.insert(v);

                    if (depth == 2) continue;
                    for (int nb : graph.getNeighbors(v)) {
                        if (seen.insert(nb).second) {
                            q.push({nb, depth + 1});
                        }
                    }
                }
            }
        }

        // Używa algorytmu A* do znalezienia ścieżki do najbliższego punktu
        // Kary są dodawane za wchodzenie na niebezpieczne wierzchołki
        struct OpenNode { int v; int g; int f; };
        auto cmp = [](const OpenNode& a, const OpenNode& b) { return a.f > b.f; };
        priority_queue<OpenNode, vector<OpenNode>, decltype(cmp)> open(cmp);
        unordered_map<int, int> parent;
        unordered_map<int, int> gCost;

        open.push({position, 0, 0});
        gCost[position] = 0;

        while (!open.empty()) {
            OpenNode cur = open.top();
            open.pop();

            if (cur.v == closestPoint) {
                // Odtwarza ścieżkę od celu do startu
                vector<int> path;
                int curV = cur.v;
                while (curV != position) {
                    path.push_back(curV);
                    curV = parent[curV];
                }
                path.push_back(position);
                reverse(path.begin(), path.end());
                if (path.size() >= 2) return path[1];
                return position;
            }

            for (int nb : graph.getNeighbors(cur.v)) {
                int stepPenalty = danger.count(nb) ? 5 : 0;
                int tentative = cur.g + 1 + stepPenalty;

                if (!gCost.count(nb) || tentative < gCost[nb]) {
                    gCost[nb] = tentative;
                    parent[nb] = cur.v;
                    open.push({nb, tentative, tentative});
                }
            }
        }

        return position;
    }
};

// Klasa zarządzająca całą symulacją gry
class Simulation {
    int sim_id;
    vector<Runner> runners;    // lista duchów
    vector<Seeker> seekers;    // lista Seeker (w praktyce jeden)
    Graph graph;               // graf reprezentujący planszę
    unordered_set<int> points; // punkty do zebrania
    bool win = false;
    bool lose = false;
    mutable mt19937 rng;       // generator losowy

    public:
        Simulation(int id, int num_of_runners, int num_of_seekers, string graphFile) 
            : sim_id(id), rng(random_device{}()) {
            graph = Graph::loadFromFile(graphFile);
            enableAnsiOnWindows();

            // Tworzy agentów z losowymi pozycjami startowymi
            for (int i=0; i < num_of_runners; i++) {
                int rnd = rand() % 1000;
                runners.emplace_back(i, rnd, graph);
            }

            for (int i=0; i < num_of_seekers; i++) {
                int rnd = rand() % 1000;
                seekers.emplace_back(i, rnd);
            }

            // Inicjalizuje pozycje i punkty
            initializePoints(6);
        }

        // Wykonuje jedną turę gry
        bool step() {
            if (win || lose) return false;
            if (seekers.empty() || runners.empty()) return false;

            // Używamy pierwszego Seeker jako Pac-Man
            Seeker& seeker = seekers[0];

            // 1) Zbieramy aktualne pozycje Runner, żeby Seeker wiedział kogo unikać
            unordered_set<int> runnerPositions;
            for (const auto& r : runners) runnerPositions.insert(r.position);

            seeker.pointsRef = &points;
            seeker.runnerPositionsRef = &runnerPositions;

            // 2) Runner przekazujemy adres pozycji Seeker (na bieżąco)
            for (auto& r : runners) r.seekerPosRef = &seeker.position;

            // 3) Seeker wykonuje swój ruch
            int nextSeeker = seeker.decideNextMove(graph);
            if (runnerPositions.count(nextSeeker)) {
                lose = true;
                seeker.position = nextSeeker;
                return false;
            }
            seeker.position = nextSeeker;
            points.erase(seeker.position);
            if (points.empty()) {
                win = true;
                return false;
            }

            // 4) Runner poruszają się kolejno – unikamy nakładania się na siebie
            unordered_set<int> newRunnerPositions;
            for (auto& runner : runners) {
                int proposed = runner.decideNextMove(graph);
                if (newRunnerPositions.count(proposed)) {
                    proposed = runner.position;
                }
                runner.position = proposed;
                newRunnerPositions.insert(runner.position);
                if (runner.position == seeker.position) lose = true;
            }

            return !(win || lose);
        }

        // Rysuje aktualny stan planszy w konsoli
        void render() const {
            const int width = 28; // szerokość planszy w znakach

            unordered_set<int> runnerPositions;
            for (const auto& r : runners) runnerPositions.insert(r.position);

            if (!seekers.empty()) {
                cout << "Stan gry (Seeker = " << seekers[0].position
                     << ", punkty: " << points.size() << ")\n";
            }

            for (int v = 1; v <= graph.vertexCount(); ++v) {
                string cell = "  ";

                if (!seekers.empty() && v == seekers[0].position) cell = color("33", "■ "); // żółty - Seeker
                else if (runnerPositions.count(v)) cell = color("35", "■ "); // fioletowy - duch
                else if (points.count(v)) cell = color("34", "* "); // niebieski - punkt
                else if (graph.getNeighbors(v).empty()) cell = color("37", "██"); // szary - ściana

                cout << cell;
                if (v % width == 0) cout << '\n';
            }

            if (graph.vertexCount() % width != 0) cout << '\n';
            cout.flush();
        }

        bool isWin() const { return win; }
        bool isLose() const { return lose; }

        // Główna pętla gry - czyści ekran, renderuje planszę i wykonuje ruchy
        void startGame() {
            while (true) {
                clearConsole();
                render();
                if (!step()) break;
                this_thread::sleep_for(chrono::milliseconds(900));
            }

            if (isWin()) {
                cout << "\nSeeker zebrał wszystkie punkty – wygrana!\n";
            } else if (isLose()) {
                cout << "\nRunner dorwały Seeker – przegrana.\n";
            } else {
                cout << "\nGra została przerwana.\n";
            }
        }

    private:
        static string color(const char* code, const string& what) {
            return string("\x1b[") + code + "m" + what + "\x1b[0m";
        }

        // Inicjalizuje pozycje agentów i punkty na planszy
        void initializePoints(int pointsCount) {
            unordered_set<int> taken;

            if (!seekers.empty()) {
                seekers[0].position = randomSpawn(true, taken);
                taken.insert(seekers[0].position);
            }

            for (auto& r : runners) {
                int position = randomSpawn(true, taken);
                taken.insert(position);
                r.position = position;
            }

            points.clear();
            for (int i = 0; i < pointsCount; ++i) {
                int target = randomSpawn(true, taken);
                points.insert(target);
            }

            win = false;
            lose = false;
        }

        // Losuje wierzchołek spełniający warunki
        int randomSpawn(bool requireNeighbors, const unordered_set<int>& forbidden) {
            vector<int> candidates;
            candidates.reserve(graph.vertexCount());

            for (int v = 1; v <= graph.vertexCount(); ++v) {
                if (!graph.hasVertex(v)) continue;
                if (forbidden.count(v)) continue;
                if (requireNeighbors && graph.getNeighbors(v).empty()) continue;
                candidates.push_back(v);
            }

            if (candidates.empty()) return 1;

            uniform_int_distribution<size_t> dist(0, candidates.size() - 1);
            return candidates[dist(rng)];
        }

        // Włącza obsługę kolorów ANSI w konsoli Windows
        static void enableAnsiOnWindows() {
#ifdef _WIN32
            SetConsoleOutputCP(65001); // UTF-8
            SetConsoleCP(65001); // UTF-8
            
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) return;
            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)) return;
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
#endif
        }
};

// Czyści ekran konsoli
static void clearConsole() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) return;

    DWORD cellCount = static_cast<DWORD>(csbi.dwSize.X) * static_cast<DWORD>(csbi.dwSize.Y);
    DWORD count;
    COORD homeCoords = {0, 0};

    FillConsoleOutputCharacter(hOut, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hOut, homeCoords);
#else
    cout << "\x1b[2J\x1b[H";
    cout.flush();
#endif
}

// Funkcja główna programu
int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string GraphPath = "graph.txt";
    if (argc > 1) GraphPath = argv[1];

    // Sprawdza czy plik z grafem istnieje
    Graph testGraph = Graph::loadFromFile(GraphPath);
    if (testGraph.vertexCount() == 0) {
        cerr << "Błąd: nie można załadować grafu z pliku " << GraphPath << "\n";
        return 1;
    }

    // Tworzy symulację z 4 duchami i 1 Seeker
    Simulation sim(1, 4, 1, GraphPath);
    sim.startGame();

    cout << "\nNaciśnij Enter, aby zakończyć...";
    cout.flush();
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    return 0;
}

