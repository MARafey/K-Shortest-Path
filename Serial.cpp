#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <queue>
#include <climits>
#include <algorithm>
#include <sstream>
#include <omp.h>
using namespace std;

#define Thread 5

int StringToInt(const char *arg)
{
    return atoi(arg);
}

int Hashing(string str)
{
    int hash = 0;
    for (int i = 0; i < str.size(); i++)
    {
        hash += str[i];
    }
    return hash;
}

vector<string> Split(string str, char delimeter)
{
    vector<string> result;
    string temp = "";
    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == delimeter)
        {
            result.push_back(temp);
            temp = "";
        }
        else
        {
            temp += str[i];
        }
    }
    result.push_back(temp);
    return result;
}

bool Find(vector<vector<pair<int, int>>> shortestPath, vector<pair<int, int>> path)
{
    for (int i = 0; i < shortestPath.size(); ++i)
    {
        if (shortestPath[i] == path)
        {
            return true;
        }
    }
    return false;
}

struct Doctor
{
    string name;
    int id;

    Doctor()
    {
        name = "";
        id = -1;
    }

    bool operator==(const Doctor &d) const
    {
        return this->id == d.id;
    }
    bool operator>(const Doctor &d) const
    {
        return this->id > d.id;
    }
    bool operator<(const Doctor &d) const
    {
        return this->id < d.id;
    }
    bool operator>=(const Doctor &d) const
    {
        return this->id >= d.id;
    }
    bool operator<=(const Doctor &d) const
    {
        return this->id <= d.id;
    }
};

class Graph
{
private:
    vector<vector<pair<int, int>>> graph;
    vector<Doctor> data;
    int Size;

public:
    Graph(int size = 0)
    {
        Size = size;
        graph.resize(size);
        data.resize(size);
    }

    void AddEdge(int u, int v, int w)
    {
        graph[u].push_back({v, w});
    }

    void Resize(int size)
    {
        Size = size;
        graph.resize(size);
        data.resize(size);
    }

    void PrintGraph()
    {
        for (int i = 0; i < Size; i++)
        {
            cout << i << " -> ";
            for (auto j : graph[i])
            {
                cout << "(" << j.first << " " << j.second << ") ";
            }
            cout << endl;
        }
    }

    void PrintGraphCSV()
    {
        for (int i = 0; i < Size; i++)
        {
            if (this->data[i].name == "")
            {
                continue;
            }
            cout << this->data[i].name << " -> ";
            for (auto j : graph[i])
            {
                cout << "(" << this->data[j.first].name << " " << j.second << ") ";
            }
            cout << endl;
        }
    }

    int LargestValue(string filename)
    {
        int largest = 0;
        FILE *file = fopen(filename.c_str(), "r");
        vector<int> values;
        // Reading the file file has 2 columns and is a txt file separated by a space
        while (!feof(file))
        {
            int a, b;
            fscanf(file, "%d %d", &a, &b);
            values.push_back(a);
            values.push_back(b);
        }
        fclose(file);

        for (int i = 0; i < values.size(); i++)
        {
            if (values[i] > largest)
            {
                largest = values[i];
            }
        }

        return largest;
    }

    int LargestValueCSV(string filename)
    {

        FILE *file = fopen(filename.c_str(), "r");

        char buff[200];
        int Size = 0;
        bool flag = false;
        while (!feof(file))
        {
            fgets(buff, 200, file);
            if (flag == false)
            {
                flag = true;
                continue;
            }
            vector<string> values = Split(buff, ',');
            for (int i = 0; i < 2; i++)
            {
                int value = Hashing(values[i]);
                if (value > Size)
                {
                    Size = value;
                }
            }
        }

        return Size;
    }

    void RemoveSelfLoop()
    {
        // Removing the self-loops in the graph
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < graph[i].size(); j++)
            {
                if (graph[i][j].first == i)
                {
                    graph[i].erase(graph[i].begin() + j);
                }
            }
        }
    }

    void RemovingDuplicateEdges()
    {
        // Removing the duplicate edges in the graph
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < graph[i].size(); j++)
            {
                for (int k = j + 1; k < graph[i].size(); k++)
                {
                    if (graph[i][j].first == graph[i][k].first)
                    {
                        graph[i].erase(graph[i].begin() + k);
                    }
                }
            }
        }
    }

    void FR(string filename)
    {
        int size = LargestValue(filename) + 1;
        // cout << size << endl;
        FILE *file = fopen(filename.c_str(), "r");
        Resize(size);

        // Reading the file file has 2 columns and is a txt file separated by a space
        while (!feof(file))
        {
            int a, b;
            fscanf(file, "%d %d", &a, &b);
            AddEdge(a, b, 1);
        }
        fclose(file);
    }

    void FRC(string Filename)
    {
        int size = LargestValueCSV(Filename) + 1;
        Resize(size);
        FILE *file = fopen(Filename.c_str(), "r");

        char buff[200];
        while (!feof(file))
        {
            fgets(buff, 200, file);
            vector<string> values = Split(buff, ',');
            int u = Hashing(values[0]) % size;
            int v = Hashing(values[1]) % size;
            int cost = StringToInt(values[2].c_str());

            AddEdge(u, v, cost);
            this->data[u].name = values[0];
            this->data[u].id = u;
            this->data[v].name = values[1];
            this->data[v].id = v;
        }
        fclose(file);
    }

    vector<vector<pair<int, int>>> findKShortestPaths(int start, int end, int k)
    {
        vector<vector<pair<int, int>>> shortestPaths;

        // Find the shortest path from start to end using Dijkstra's algorithm
        vector<pair<int, int>> shortestPath = Dijkstra(start, end);
        shortestPaths.push_back(shortestPath);

        srand(time(0));

        k--;
        while (k != 0)
        {
            // Making a copy of the Graph
            Graph g = *this;

            // Excluding the selected edge from the graph
            vector<pair<int, int>> excludedEdges(shortestPaths.size());

            // selecting random edge from each of the already found shortest path and excluding it
#pragma omp parallel for num_threads(Thread) shared(excludedEdges) firstprivate(shortestPaths)
            for (int i = 0; i < shortestPaths.size(); i++)
            {
                int id = omp_get_thread_num();
                srand(time(0) + id);
                int randomEdge = rand() % shortestPaths[i].size() - 1;
#pragma omp critical
                excludedEdges[i] = make_pair(shortestPaths[i][randomEdge].first, shortestPaths[i][randomEdge + 1].first);
            }

            // Finding the shortest path from start to end excluding the selected edge
            vector<pair<int, int>> path = g.Dijkstra(start, end, excludedEdges);

            // Adding the shortest path to the list of k shortest paths if not already present
            if (Find(shortestPaths, path) == false)
            {
                if (path.size() == 1)
                {
                    continue;
                }
                shortestPaths.push_back(path);
                k--;
            }
        }

        return shortestPaths;
    }

    vector<pair<int, int>> Dijkstra(int start, int end, vector<pair<int, int>> excludedEdges = {})
    {
        // Initialize a priority queue to store vertices
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        // Initialize a vector to store the shortest distance from the start vertex
        vector<int> distance(Size, INT_MAX);

        // Initialize a vector to store the parent of each vertex in the shortest path
        vector<int> parent(Size, -1);

        // Initialize the distance of the start vertex to 0
        distance[start] = 0;

        // Add the start vertex to the priority queue
        pq.push(make_pair(0, start));

        // Loop until the priority queue is empty
        while (!pq.empty())
        {
            // Get the vertex with the smallest distance from the priority queue
            int u = pq.top().second;
            pq.pop();

// Loop through the neighbors of the current vertex
#pragma omp parallel for num_threads(Thread) shared(distance, parent, pq, graph, excludedEdges) // proc_bind(policy)
            for (int i = 0; i < graph[u].size(); i++)
            {
                int v = graph[u][i].first;
                int weight = graph[u][i].second;

                // Check if the edge (u, v) is not excluded
                if (find(excludedEdges.begin(), excludedEdges.end(), make_pair(u, v)) == excludedEdges.end())
                {
// Relax the edge (u, v)
#pragma omp critical
                    if (distance[u] + weight < distance[v])
                    {
                        distance[v] = distance[u] + weight;
                        parent[v] = u;
                        pq.push(make_pair(distance[v], v));
                    }
                }
            }
        }

        // Reconstruct the shortest path from the start vertex to the end vertex
        vector<pair<int, int>> path;
        for (int v = end; v != -1; v = parent[v])
        {
            path.push_back(make_pair(v, distance[v]));
        }

        // Reverse the path
        reverse(path.begin(), path.end());

        return path;
    }
};

int main(int argc, char *argv[])
{
    string filename = argv[1];
    string fileType = argv[2];
    int k = StringToInt(argv[3]);
    Graph g;

    string FilePath = "Dataset/" + filename;
    if (fileType == "txt")
    {
        FilePath += ".txt";
        g.FR(FilePath);
        g.RemoveSelfLoop();
        g.RemovingDuplicateEdges();
        // g.PrintGraph();
        vector<vector<pair<int, int>>> shortestPaths = g.findKShortestPaths(0, 3, k);

        for (int i = 0; i < shortestPaths.size(); i++)
        {
            cout << "Path " << i + 1 << ": ";
            for (int j = 0; j < shortestPaths[i].size(); j++)
            {
                cout << shortestPaths[i][j].first << " ";
            }
            cout << endl;
        }
    }
    else if (fileType == "csv")
    {
        FilePath += ".csv";
        g.FRC(FilePath);
        g.RemoveSelfLoop();
        g.RemovingDuplicateEdges();
        vector<vector<pair<int, int>>> shortestPaths = g.findKShortestPaths(0, 3, k);
    }
    else
    {
        cout << "Invalid File Type" << endl;
        return 1;
    }

    return 0;
}
