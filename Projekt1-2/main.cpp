#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <sstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <thread>

using namespace std;

class Graph {
    map<int, list<int>> graphRep; // adjacency list

public:
    // przyjmuje wierzchołek (vertex) i jego krawędzie (linkedEdges)
    void add_point(const int vertex, const list<int>& linkedEdges) {
        graphRep[vertex] = linkedEdges;
    }

    // przyjmuje wierzchołek i zwraca krawędzie z którymi jest połączony
    // później pewnie do zmiany
    void get_point(int const vertex) {
        cout << "Edges linked to point " << vertex << endl;
        for (auto const v : graphRep[vertex]) {
            cout << v << " ";
        }
        cout << endl;
    }

    list<int> get_neighbors(int vertex) {
        return graphRep[vertex];
    }

};

// Klasa podstawowa agenta
class Agent {
    protected:
        int id;

    public:
        int position; // aktualna pozycja, na starcie losowo przypisywana
        Agent(int id, int starting_position): id(id), position(starting_position) {}

        virtual void move(int new_position) {}

        void check_vertex() {}
};

class Runner : public Agent {
    vector<int> visited;
    bool is_alive = true;
    Graph& graph;

    public:
        Runner(int id, int starting_position, Graph& g): Agent(id, starting_position), graph(g) {}

        void show_visited() {
            cout << "Already visited: ";
            for (int i : visited) {
                cout << i << " ";
            }
            cout << endl;
        }

        void add_vertex(int vertex) {
            visited.push_back(vertex);
        }

        bool find_vertex(int vertex) {
            for (int i : visited) {
                if (i == vertex) {
                    return true;
                }
            }
            return false;
        }

        bool dfs(int vertex) {
            cout << "Visiting: " << vertex << endl;
            add_vertex(vertex);
            list<int> neighbors = graph.get_neighbors(vertex);

            for (int i : neighbors) {
                this_thread::sleep_for(chrono::nanoseconds(250ms));
                if (!find_vertex(i)) {
                    cout << "go to: " << i << endl;
                    dfs(i);
                } else {
                    show_visited();
                    // cout << "visited: " << i << endl;
                }
            }
            cout << "done" << endl;
        }
};

class Seeker : public Agent {
    public:
        Seeker(int id, int starting_position): Agent(id, starting_position) {}

        // move
        // catch_runner - złapanie runner i ustawienie is_alive = false
};

int main() {
    Graph graph;

    // Otwieranie pliku
    // Później może przerobić żeby można było ścieżke w parametrach startowych podać
    // lub jeśli się nie poda żeby automatycznie generowało ale to już jak szybko zrobimy główną funkcjonalność
    fstream fs;
    fs.open(R"(C:\Users\admin\Downloads\Projekt1\Projekt1\graph.txt)", fstream::in);

    if (!fs.is_open()) {
        cout << "Error" << endl;
    }

    // Wpisywanie z pliku do obiektu
    // może lepiej by było to całe zrobić jako osobna funkcja obiektu Graph
    // która będzie przyjmować plik i sobie wrzucać to do listy
    string line;
    int vertex = 1;
    while (getline(fs, line)) {
        list<int> linked;
        stringstream ss(line);
        string snum;

        while (getline(ss, snum, ' ')) {
            linked.push_back(stoi(snum));
        }
        graph.add_point(vertex, linked);
        linked.clear();
        vertex++;
    }

    Runner r(1, 1, graph);

    r.dfs(r.position);

    fs.close();

    return 0;
}
