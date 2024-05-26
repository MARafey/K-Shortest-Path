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
#include <mpi.h>
using namespace std;

#define Thread 3

int StringToInt(const char *arg)
{
    /*
    Summary :
        This function converts a string to an integer.
    */
    return atoi(arg); // Convert string to integer
}

int Hashing(string str)
{
    /*
    Summary:
        This function hashes a string to an integer. As string can not be used to access the array, we hash the string to an integer.
        Ref : https://domino.ai/data-science-dictionary/hash-table
    */
    int hash = 0; // Hashing the string
    for (int i = 0; i < str.size(); i++)
    {
        hash += str[i]; // Summing up the ASCII values of the characters
    }
    return hash; // Returning the hash value
}

vector<string> Split(string str, char delimeter)
{
    /*
    Summary:
        This function splits a string based on the delimeter. It returns a vector of strings.
    */
    vector<string> result;               // Splitting the string based on the delimeter
    string temp = "";                    // Temporary string to store the split values
    for (int i = 0; i < str.size(); i++) // Looping through the string
    {
        if (str[i] == delimeter) // If the character is the delimeter
        {
            result.push_back(temp); // Push the temporary string to the result vector
            temp = "";
        }
        else
        {
            temp += str[i]; // Append the character to the temporary string
        }
    }
    result.push_back(temp); // Push the last value to the result vector
    return result;          // Return the result vector
}

bool Find(vector<vector<pair<int, int>>> &shortestPath, vector<pair<int, int>> &path)
{
    /*
    Summary:
        This function checks if the path is already present in the list of shortest paths.
    */
    for (int i = 0; i < shortestPath.size(); i++) // Check if the path is already present in the list of shortest paths
    {
        if (shortestPath[i] == path) // If the path is already present
        {
            return true;
        }
    }
    return false;
}

struct Doctor
{
    string name; // Name of the doctor
    int id;      // Hash value of the name

    Doctor()
    {
        name = "";
        id = 0;
    }

    /*
    Overloading operations to use for comparison that is required for built-in functions like sort min-max and so on
    */
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
    void operator=(const Doctor &d)
    {
        this->name = d.name;
        this->id = d.id;
    }
};

class Graph
{
public:
    vector<vector<pair<int, int>>> graph; // Adjacency list of the graph to store the edges and weights
    vector<Doctor> data;
    int Size;

    Graph(int size = 0) // Constructor
    {
        Size = size;
        graph.resize(size);
        data.resize(size);
    }

    void AddEdge(int u, int v, int w) // Function to add an edge to the graph from u to v with weight w
    {
        graph[u].push_back({v, w});
    }

    void Resize(int size) // Function to resize the graph
    {
        Size = size;
        graph.resize(size);
        data.resize(size);
    }

    void PrintGraph() // Function to print the graph
    {
        for (int i = 0; i < Size; i++)
        {
            cout << i << " -> "; // Print the vertex
            for (int j = 0; j < graph[i].size(); j++)
            {
                cout << "(" << graph[i][j].first << " " << graph[i][j].second << ") "; // Print the edges and weights
            }
            cout << endl;
        }
    }

    void PrintGraphCSV() // Function to print the graph in CSV format
    {
        for (int i = 0; i < Size; i++)
        {
            if (this->data[i].name == "") // Skip the empty nodes
            {
                continue;
            }
            cout << this->data[i].name << " -> ";
            for (int j = 0; j < graph[i].size(); j++)
            {
                cout << "(" << this->data[graph[i][j].first].name << " " << graph[i][j].second << ") "; // Print the edges and weights
            }
            cout << endl;
        }
    }

    int LargestValue(string filename) // Function to find the largest value in the file
    {
        int largest = 0;                           // Initialize the largest value to 0
        FILE *file = fopen(filename.c_str(), "r"); // Open the file in read mode
        vector<int> values;                        // Vector to store the values
        // Reading the file file has 2 columns and is a txt file separated by a space
        while (!feof(file)) // Loop until the end of the file
        {
            int a, b;                      // Variables to store the values
            fscanf(file, "%d %d", &a, &b); // Read the values from the file
            values.push_back(a);           // Push the values to the vector
            values.push_back(b);
        }
        fclose(file); // Close the file

        for (int i = 0; i < values.size(); i++) // Loop through the values
        {
            if (values[i] > largest) // If the value is greater than the largest value
            {
                largest = values[i]; // Update the largest value
            }
        }

        return largest; // Return the largest value
    }

    int LargestValueCSV(string filename) // Function to find the largest value in the CSV file
    {
        int largest = 0;                           // Initialize the largest value to 0
        FILE *file = fopen(filename.c_str(), "r"); // Open the file in read mode
        char buff[200];                            // Buffer to store the values
        bool flag = false;                         // Flag to skip the first line

        while (!feof(file)) // Loop until the end of the file
        {
            fgets(buff, 200, file); // Read the line from the file

            if (flag == false) // Skip the first line
            {
                flag = true;
                continue;
            }
            vector<string> values = Split(buff, ','); // Split the line based on the delimeter
            for (int i = 0; i < 2; i++)               // Loop through the values
            {
                int value = Hashing(values[i]); // Hash the value
                if (value > largest)            // If the value is greater than the largest value
                {
                    largest = value; // Update the largest value
                }
            }
        }

        return largest; // Return the largest value
    }

    string FindName(int id) // Function to find the name of the doctor
    {
        return this->data[id].name; // Return the name of the doctor
    }

    void RemoveSelfLoop()
    {
        /*
        Summary :
            The data set may contain self loop within it which is not required in the graph. This function removes the self loops from the graph.
        */
        // Removing the self-loops in the graph
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < graph[i].size(); j++)
            {
                if (graph[i][j].first == i) // If the edge is a self-loop
                {
                    graph[i].erase(graph[i].begin() + j); // Removing the self-loop
                }
            }
        }
    }

    void RemovingDuplicateEdges()
    {
        /*
        Summary:
            The data set may contain duplicate edges within it which is not required in the graph. This function removes the duplicate edges from the graph.
            For Example:
                1 2
                1 2
                1 3
                1 4
                2 3
                2 4
                2 4
                3 4
                3 4
                4 5
                In this example, the edge 1 2 is repeated twice. This function removes the duplicate edges from the graph.
        */
        // Removing the duplicate edges in the graph
        for (int i = 0; i < Size; i++)
        {
            for (int j = 0; j < graph[i].size(); j++)
            {
                for (int k = j + 1; k < graph[i].size(); k++)
                {
                    if (graph[i][j].first == graph[i][k].first) // If the edge is a duplicate edge
                    {
                        graph[i].erase(graph[i].begin() + k); // Removing the duplicate edge
                    }
                }
            }
        }
    }

    void FR(string filename) // Function to read the file and create the graph
    {
        int size = LargestValue(filename) + 1;
        // cout << size << endl;
        FILE *file = fopen(filename.c_str(), "r");
        Resize(size);

        // Reading the file file has 2 columns and is a txt file separated by a space
        while (!feof(file))
        {
            int a, b;
            fscanf(file, "%d %d", &a, &b); // Read the values from the file
            AddEdge(a, b, 1);              // Add the edge to the graph with weight 1
        }
        fclose(file);
    }

    void FRC(string Filename)
    {
        int size = LargestValueCSV(Filename) + 1;  // Finding the largest value in the CSV file
        Resize(size);                              // Resizing the graph
        FILE *file = fopen(Filename.c_str(), "r"); // Open the file in read mode

        char buff[200]; // Buffer to store the values
        while (!feof(file))
        {
            fgets(buff, 200, file);                    // Read the line from the file using fgets function to read the white spaces as well
            vector<string> values = Split(buff, ',');  // Split the line based on the delimeter
            int u = Hashing(values[0]) % size;         // Hash the values to get the index  of the graph taking modulo of the size of the graph
            int v = Hashing(values[1]) % size;         // Hash the values to get the index  of the graph taking modulo of the size of the graph
            int cost = StringToInt(values[2].c_str()); // Convert the string to integer

            AddEdge(u, v, cost);            // Add the edge to the graph
            this->data[u].name = values[0]; // Add the name of the doctor
            this->data[u].id = u;           // Add the id of the doctor
            this->data[v].name = values[1]; // Add the name of the doctor
            this->data[v].id = v;           // Add the id of the doctor
        }
        fclose(file); // Close the file
    }

    // vector<vector<pair<int, int> > > findKShortestPaths(int start, int end, int k) // Function to find the k shortest paths from start to end
    // {
    //     /*
    //         Summary:
    //             This function finds the k shortest paths from the start vertex to the end vertex.
    //             It uses Dijkstra's algorithm to find the shortest path from the start vertex to the end vertex.
    //             It then excludes a random edge from the shortest path and finds the shortest path from the start vertex to the end vertex excluding the selected edge.
    //             It repeats this process until k shortest paths are found.
    //     */
    //     vector<vector<pair<int, int> > > shortestPaths; // Vector to store the shortest paths
    //     // Find the shortest path from start to end using Dijkstra's algorithm
    //     vector<pair<int, int> > shortestPath = Dijkstra(start, end);
    //     shortestPaths.push_back(shortestPath); // Add the shortest path to the list of shortest paths
    //     int removedEdges = 0; // Variable to store the number of removed edges
    //     k--; // Decrement the value of k as we have already found the shortest path
    //     while (k != 0)
    //     {
    //         // Making a copy of the Graph
    //         Graph g = *this;
    //         // Excluding the selected edge from the graph
    //         vector<pair<int, int> > excludedEdges(shortestPaths.size());
    //     }
    //     return shortestPaths; // Return the list of k shortest paths
    // }

    vector<vector<pair<int, int>>> findKShortestPaths(int start, int end, int k)
    {
        vector<vector<pair<int, int>>> shortestPaths;
        vector<pair<int, int>> path = Dijkstra(start, end);

        if (path.empty())
            return shortestPaths; // Return empty if no path is found

        vector<vector<pair<int, int>>> pathsFound;
        pathsFound.push_back(path);
        shortestPaths.push_back(path);

        vector<vector<pair<int, int>>> bannedEdges(k); // Each vector corresponds to a list of banned edges for that path iteration

        while (shortestPaths.size() <= k)
        {

            cout << "Shortest Path Size: " << shortestPaths.size() << endl;
            bool new_path_found = false;

            for (int i = 0; i < shortestPaths.size(); i++)
            {
                vector<pair<int, int>> &sp = shortestPaths[i];

                for (int j = 0; j < sp.size(); j++)
                {
                    int u = sp[j].first;
                    int v = sp[j + 1].first;

                    // Temporarily ban edge u->v and try to find another path
                    bannedEdges[i].push_back({u, v});
                    vector<pair<int, int>> new_path = Dijkstra(start, end, bannedEdges[i]);

                    // Check if new path is different and not already found
                    if (!new_path.empty() && find(pathsFound.begin(), pathsFound.end(), new_path) == pathsFound.end())
                    {
                        pathsFound.push_back(new_path);
                        shortestPaths.push_back(new_path);
                        new_path_found = true;
                        break; // Break out to start with this new path in the next iteration
                    }

                    // Remove the last temporarily banned edge for future iterations
                    bannedEdges[i].pop_back();
                }
                if (new_path_found) // Break if new path is found
                    break;
            }

            if (!new_path_found)
                break; // Break if no new path can be found
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
#pragma omp parallel for num_threads(Thread) shared(distance, parent, pq, excludedEdges) // proc_bind(policy)
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
    int rank = 0;
    int nproc = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    string filename = argv[1];
    string fileType = argv[2];
    int k = StringToInt(argv[3]);
    Graph g;

    srand(time(NULL) + rank);

    string FilePath = "Dataset/" + filename;
    if (fileType == "txt")
    {
        FilePath += ".txt";

        if (rank == 0)
        {
            g.FR(FilePath);
            g.RemoveSelfLoop();
            g.RemovingDuplicateEdges();
            // g.PrintGraph();

            // Sending the graph to all the processes
            for (int i = 1; i < nproc; i++)
            {
                MPI_Send(&g.Size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                for (int j = 0; j < g.Size; j++)
                {
                    int size = g.graph[j].size();
                    MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    for (int k = 0; k < size; k++)
                    {
                        MPI_Send(&g.graph[j][k].first, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        MPI_Send(&g.graph[j][k].second, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    }
                }
            }
        }
        else
        {

            int Size = 0;
            MPI_Recv(&Size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            g.Resize(Size);
            for (int i = 0; i < Size; i++)
            {
                int size;
                MPI_Recv(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int j = 0; j < size; j++)
                {
                    int v, w;
                    MPI_Recv(&v, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&w, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    g.AddEdge(i, v, w);
                }
            }
        }
        // waiting for all the processes to finish
        MPI_Barrier(MPI_COMM_WORLD);

        int start = rand() % g.Size;
        int end = rand() % g.Size;
        // Finding the k shortest paths from the start vertex to the end vertex
        // cout << start << " " << end << endl;
        vector<vector<pair<int, int>>> shortestPaths = g.findKShortestPaths(start, end, k);

        if (rank != 0)
        {
            // Sending start and end vertex to the root process
            MPI_Send(&start, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);

            // Sending the size of the shortest paths to the root process
            int completeSIze = shortestPaths.size();
            MPI_Send(&completeSIze, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            // Sending the shortest paths to the root process
            for (int i = 0; i < shortestPaths.size(); i++)
            {
                int size = shortestPaths[i].size();
                MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                for (int j = 0; j < size; j++)
                {
                    // Sending the nodes of the shortest path to the root process one by one
                    MPI_Send(&shortestPaths[i][j].first, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
        else
        {
            // Receiving the shortest paths from all the processes
            for (int i = 1; i < nproc; i++)
            {
                cout << "============================="
                     << " Process " << i
                     << "=============================" << endl;
                int start = 0;
                int end = 0;

                MPI_Recv(&start, 1, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&end, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                cout << "Start: " << start << " End: " << end << endl;

                vector<vector<pair<int, int>>> Paths;
                int size = 0;
                MPI_Recv(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                Paths.resize(size);
                for (int j = 0; j < size; j++)
                {
                    int pathSize = 0;
                    MPI_Recv(&pathSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    Paths[j].resize(pathSize);
                    for (int k = 0; k < pathSize; k++)
                    {
                        int node = 0;
                        MPI_Recv(&node, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        Paths[j][k].first = node;
                    }
                }
                for (int j = 0; j < Paths.size(); j++)
                {
                    cout << "Path " << j + 1 << ": ";
                    for (int k = 0; k < Paths[j].size() - 1; k++)
                    {
                        cout << Paths[j][k].first << "->";
                    }
                    cout << Paths[j][Paths[j].size() - 1].first << endl;
                    cout << endl;
                }
            }
        }
    }
    else if (fileType == "csv")
    {
        FilePath += ".csv";
        if (rank == 0)
        {
            g.FRC(FilePath);
            g.RemoveSelfLoop();
            g.RemovingDuplicateEdges();
            // g.PrintGraphCSV();
            //  Sending the graph to all the processes
            for (int i = 1; i < nproc; i++)
            {
                MPI_Send(&g.Size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                for (int j = 0; j < g.Size; j++)
                {
                    int size = g.graph[j].size();
                    MPI_Send(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    for (int k = 0; k < size; k++)
                    {
                        MPI_Send(&g.graph[j][k].first, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                        MPI_Send(&g.graph[j][k].second, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    }
                }
            }

            // Sending the data to all the processes
            for (int i = 1; i < nproc; i++)
            {
                for (int j = 0; j < g.Size; j++)
                {
                    MPI_Send(&g.data[j].id, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    char name[50];
                    strcpy(name, g.data[j].name.c_str());
                    MPI_Send(name, 50, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
            }
        }
        else
        {
            int NSize = 0;
            MPI_Recv(&NSize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            g.Resize(NSize);
            for (int i = 0; i < NSize; i++)
            {
                int nsize;
                MPI_Recv(&nsize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for (int j = 0; j < nsize; j++)
                {
                    int vv, ww;
                    MPI_Recv(&vv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(&ww, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    g.AddEdge(i, vv, ww);
                }
            }
            for (int i = 0; i < NSize; i++)
            {
                int id = 0;
                MPI_Recv(&id, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                char name[50];
                MPI_Recv(name, 50, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                g.data[id % g.Size].name = name;
                g.data[id % g.Size].id = id;
            }
        }
        // waiting for all the processes to finish
        MPI_Barrier(MPI_COMM_WORLD);

        int start = 0;
        int end = 0;
        do
        {
            start = rand() % g.Size;
            end = rand() % g.Size;
            if (start != end && g.FindName(start) != "" && g.FindName(end) != "")
            {
                break;
            }
        } while (true);

        // Finding the k shortest paths from the start vertex to the end vertex
        vector<vector<pair<int, int>>> shortestPaths = g.findKShortestPaths(start, end, k);

        // salaves sending the data to the root process
        if (rank != 0)
        {
            // Sending start and end vertex to the root process
            MPI_Send(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            // Sending the size of the shortest paths to the root process
            int completeSIze = shortestPaths.size();
            MPI_Send(&completeSIze, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

            // Sending the shortest paths to the root process
            for (int i = 0; i < shortestPaths.size(); i++)
            {
                int size = shortestPaths[i].size();
                MPI_Send(&size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                for (int j = 0; j < size; j++)
                {
                    // Sending the nodes of the shortest path to the root process one by one
                    MPI_Send(&shortestPaths[i][j].first, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                }
            }
        }
        else
        {
            // Receiving the shortest paths from all the processes
            for (int i = 1; i < nproc; i++)
            {
                cout << "============================="
                     << " Process " << i
                     << "=============================" << endl;
                int start = 0;
                int end = 0;

                MPI_Recv(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&end, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                cout << "Start: " << g.FindName(start) << " End: " << g.FindName(end) << endl;

                vector<vector<pair<int, int>>> Paths;
                int size = 0;
                MPI_Recv(&size, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                Paths.resize(size);
                for (int j = 0; j < size; j++)
                {
                    int pathSize = 0;
                    MPI_Recv(&pathSize, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    Paths[j].resize(pathSize);
                    for (int k = 0; k < pathSize; k++)
                    {
                        int node = 0;
                        MPI_Recv(&node, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        Paths[j][k].first = node;
                    }
                }
                for (int j = 0; j < Paths.size(); j++)
                {
                    cout << "Path " << j + 1 << ": ";
                    for (int k = 0; k < Paths[j].size() - 1; k++)
                    {
                        cout << g.FindName(Paths[j][k].first) << "->";
                    }
                    cout << g.FindName(Paths[j][Paths[j].size() - 1].first) << endl;
                    cout << endl;
                }
            }
        }
    }
    else
    {
        cout << "Invalid File Type" << endl;
        return 1;
    }
    MPI_Finalize();
    return 0;
}
