#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <chrono>
#include <queue>
#include <thread>

class Graph;
using namespace std;

class Graph {
    map<int, vector<int>> graphRep; // adjacency list
    vector<int> targets;

public:

    void add_point(const int vertex, const vector<int>& linkedEdges) {
        graphRep[vertex] = linkedEdges;
    }

    void load_graph(const string& graphFile) {
        fstream fs;
        fs.open(graphFile, fstream::in);

        if (!fs.is_open()) {
            cout << "Error - cannot open filee" << endl;
        }

        string line;
        int vertex = 1;
        while (getline(fs, line)) {
            vector<int> linked;
            stringstream ss(line);
            string token;

            while (ss >> token) {
                linked.push_back(stoi(token));
            }
            add_point(vertex, linked);
            linked.clear();
            vertex++;
        }
    }

    vector<int> neighbors(const int vertex) {
        return graphRep[vertex];
    }

    int size() {
        return graphRep.size();
    }

    void load_targets(int targetCount = 3) {
        for (int i = 0; i < targetCount; i++) {
            targets.push_back((rand() % size()) + 1);
        }
    }

    void show_targets() {
        for (int t : targets) {
            cout << t << " ";
        }
    }

    vector<int> get_targets() {
        return targets;
    }

    void remove_target(int target) {
        targets.erase(std::remove(targets.begin(), targets.end(), target), targets.end());
    }

    vector<int> shortest_path(int start, int goal) {
        if (start == goal) return {start};
        map<int,int> parent;
        queue<int> q;
        q.push(start);
        map<int,bool> visited;
        visited[start] = true;

        while(!q.empty()) {
            int v = q.front(); q.pop();
            for (int nb : neighbors(v)) {
                if (visited[nb]) continue;
                visited[nb] = true;
                parent[nb] = v;
                if (nb == goal) {
                    vector<int> path;
                    int cur = goal;
                    while(cur != start) { path.push_back(cur); cur = parent[cur]; }
                    path.push_back(start);
                    reverse(path.begin(), path.end());
                    return path;
                }
                q.push(nb);
            }
        }
        return {};
    }
};


class Agent {
    public:
        int id;
        int position;
        Agent(int id, int starting_position): id(id), position(starting_position) {}
};

class Runner : public Agent {
    vector<int> visited;
    vector<int> stack;
    bool is_active = true;
    Graph& graph;

    public:
        Runner(int id, int starting_position, Graph& graph): Agent(id, starting_position), graph(graph) {
            stack.push_back(starting_position);
        }

    void kill() {
            is_active = false;
            position = 0;
        }

    void check_alive() {
            if (is_active) {
                cout << "Alive" << endl;
            } else {
                cout << "Dead" << endl;
            }
        }

    bool is_alive() {
            return is_active;
        }

    bool is_visited(int vertex) {
            for (int v : visited) {
                if (v == vertex) return true;
            }
            return false;
        }

    void dfs_search() {
            if (!is_visited(position)) visited.push_back(position);
            bool move_back = true;

            vector<int> neighbors = graph.neighbors(position);

            for (int v : neighbors) {
                if (!is_visited(v)) {
                    stack.push_back(position);
                    position = v;
                    move_back = false;
                    break;
                }
            }

            if (move_back && !stack.empty()) {
                step_back();
            }
        }

    void show_visited() {
            for (int v : visited) {
                cout << v << " ";
            }
        }

    vector<int> get_visited() {
            return visited;
        }

    void rndz(vector<int> received_visited) {
            for (int v : received_visited) {
                if (!is_visited(v)) {
                    visited.push_back(v);
                }
            }
        }

    void step_back() {
            position = stack.back();
            stack.pop_back();
        }

};

class Seeker : public Agent {
    Graph& graph;
    int vision_range = 3;

    public:
        Seeker(int id, int starting_position, Graph& graph, int vision): Agent(id, starting_position), vision_range(vision), graph(graph) {}

        void rnd_move() {
            vector<int> neighbors = graph.neighbors(position);

            if(!neighbors.empty()) {
                int rnd = rand() % neighbors.size();
                position = neighbors[rnd];
            }
        }

        bool can_see_runner(Runner& r, Graph& g) { return g.shortest_path(position,r.position).size() <= vision_range; }

        void move_to_target(Graph& g, int target = -1) {
            if(target!=-1 && target!=position) {
                vector<int> path = g.shortest_path(position,target);
                if(path.size()>=2) position=path[1];
            } else {
                vector<int> nb = g.neighbors(position);
                if(!nb.empty()) position=nb[rand()%nb.size()];
            }
        }
};

class Simulation {
    int sim_id;
    vector<Runner> runners;
    vector<Seeker> seekers;
    Graph graph;
    int ms = 100;
    int targets;

    public:
        Simulation(int id, int num_of_runners, int num_of_seekers, string graphFile, int seeker_vision_range, int _ms, int _targets) {
            sim_id = id;
            graph.load_graph(graphFile);
            ms = _ms;
            targets = _targets;

            for (int i=0; i < num_of_runners; i++) {
                int rnd = rand() % graph.size();
                runners.emplace_back(i, rnd, graph);
            }
            for (int i=0; i < num_of_seekers; i++) {
                int rnd = rand() % graph.size();
                seekers.emplace_back(i, rnd, graph, seeker_vision_range);
            }
        }

        bool is_target(int vertex) {
            for (int v : graph.get_targets()) {
                if (v == vertex) return true;
            }
            return false;
        }

        void check_rendezvous() {
            for (int i = 0; i < runners.size(); ++i) {
                for (int j = i + 1; j < runners.size(); ++j) {
                    Runner& r = runners[i];
                    Runner& r2 = runners[j];
                    if (r.position == r2.position) {
                        r.rndz(r2.get_visited());
                        r2.rndz(r.get_visited());

                        r2.step_back();
                    }
                }
            }
        }

        void check_collision() {
            for (Runner& r : runners) {
                for (Seeker& s : seekers) {
                    if (r.position == s.position) {
                        r.kill();
                        cout << "Runner " << r.id << " has been caught!" << endl;
                    }
                }
            }
        }

        bool runners_active() {
            bool is_active = false;
            for (Runner& r : runners) {
                if (r.is_alive()) is_active = true;
            }
            return is_active;
        }

        void start() {
            cout << "Simulation has started." << endl;
            graph.load_targets(targets);

            cout << "Targets positions: " << endl;
            graph.show_targets();
            cout << endl;
            cout << "Number of runners: " << runners.size() << endl;
            cout << "Number of seekers: " << seekers.size() << endl;
            cout << endl;

            do {
                this_thread::sleep_for(chrono::milliseconds(ms));
                for (Runner& r: runners) {
                    if (r.is_alive()) {
                        r.dfs_search();
                        // cout << "Runner position: " << r.position << endl;
                        if (is_target(r.position)) {
                            graph.remove_target(r.position);
                            cout << "Runner " << r.id << " found target at : " << r.position << endl;
                        }
                    }
                }

                vector<int> alert_positions;
                for (Runner& r: runners) {
                    if (!r.is_alive()) continue;
                    for (Seeker& s: seekers) {
                        if (s.can_see_runner(r, graph)) {
                            cout << "Runner " << r.id << " has been spotted at: " << r.position << endl;
                            alert_positions.push_back(r.position);
                            break;
                        }
                    }
                }

                for (Seeker& s: seekers) {
                    int target = -1;
                    for (Runner& r: runners) {
                        if (s.can_see_runner(r, graph)) {
                            target = r.position;
                            break;
                        }
                    }

                    if (target != -1) {
                        vector<int> path = graph.shortest_path(s.position, target);
                        if (path.size() >= 2)
                            s.position = path[1];
                    } else {
                        vector<int> nb = graph.neighbors(s.position);
                        if (!nb.empty())
                            s.position = nb[rand() % nb.size()];
                    }
                }


                check_rendezvous();
                check_collision();


            } while (!graph.get_targets().empty() && runners_active());

            if (graph.get_targets().empty()) {
                cout << "Runners win!." << endl;
            } else {
                cout << "Seekers win!." << endl;
            }
        }
};

int main() {
    string GraphPath = "graph.txt";
    srand(time(nullptr));

    int num_of_runners = 10;
    int num_of_seekers = 10;
    int seeker_vision_range = 1;
    int ms = 10;
    int targets = 10;

    Simulation sim(1, num_of_runners, num_of_seekers, GraphPath, seeker_vision_range, ms, targets);

    sim.start();

    return 0;
}