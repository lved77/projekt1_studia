#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <sstream>
#include <vector>
#include <algorithm>

class Graph;
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
            for (int i : visited) {
                cout << i << " " << endl;
            }
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
            add_vertex(vertex);
            list<int> neighbors = graph.get_neighbors(vertex);

            for (int i : neighbors) {
                if (!find_vertex(i)) {
                    cout << "searching: " << i << endl;
                    dfs(i);
                } else {
                    show_visited();
                    cout << "visited vertex: " << i << endl;
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
    fs.open(R"(C:\Users\kryst\Downloads\Projekt1-15102025\Projekt1-20251015T125322Z-1-001\Projekt1\graph.txt)", fstream::in);

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

    graph.get_point(1);

    Runner r(1, 1, graph);

    r.dfs(r.position);

    fs.close();

    return 0;
}

