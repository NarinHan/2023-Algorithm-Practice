/*  REFERENCES
    (1) CPP file named 'graph_const_7.cpp' professor provided
    (2) Dijkstra : https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-using-priority_queue-stl/
    (3) Priority Queue : https://jungeu1509.github.io/algorithm/use-priorityqueue/
    (4) Floyd : https://www.geeksforgeeks.org/floyd-warshall-algorithm-dp-16/
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <vector>
#include <string>
#include <queue>

#define MAX_VER 30 // assume number of vertices in graph is <= 30

using namespace std ;

int ncity ; // number of cities
vector<pair<int, string> > city_table; 

typedef struct g_node {
    int city ;
    int distance ;
    struct g_node *link ;
    bool operator> (const g_node& other) const { // overload comparison operator for priority queue
        return distance > other.distance ; 
    }
} list_node ;

typedef struct h_node {
    int data ;
    struct g_node *link ;
} head_node ;

vector<vector<string> > read_data()
{
    ifstream file("hw6.data"); // input data : "hw6.data"
    if (!file) {
        cout << "Failed to open a file!\n";
        exit(1);
    }

    string line ;
    int row = 0 ;
    vector<string> temp ;
    vector<vector<string> > array ;

    while (getline(file, line)) {
        row++ ; // count the number of lines to figure out the number of vertices
    }

    file.clear() ; // clear any error flags
    file.seekg(0, ios::beg) ; // move back to the beginning of the file

    string word ;
    while (file >> word) {
        temp.push_back(word) ;
    }
    int col = temp.size() / row ;

    for (int i = 0; i < row; i++) {
        vector<string> vrow ;
        for (int j = 0; j < col; j++) {
            vrow.push_back(temp[col * i + j]) ;
        }
        array.push_back(vrow) ; // store the values in 'array'
    }

    file.close() ;

    return array ;
}

void convert_to_int_array(vector<vector<int> > &dist, vector<vector<string> > array) 
{
    for (int i = 0; i < ncity; i++) {
        for (int j = 1; j <= ncity; j++) {
            if (array[i][j] == "INF") { // no direct path
                dist[i][j-1] = INT_MIN ;
            }
            else {
                dist[i][j-1] = stoi(array[i][j]) ;
            }
        }
    }
}

void create_adj_list(head_node **graph) // create adjacency list
{
    *graph = new head_node[ncity] ;
    for (int i = 0; i < ncity; i++) {
        (*graph)[i].data = i ;
        (*graph)[i].link = NULL ;
    }
}

void insert_adj_list(head_node *graph, vector<vector<int> > dist) // insert the data into adjacency list
{
    for (int i = 0; i < ncity; i++) {
        for (int j = 0; j < ncity; j++) {
            if (dist[i][j] == 0)
                continue ;
            if (dist[i][j] != INT_MIN) {
                    list_node *new_node = new list_node ;
                    new_node->city = j ;
                    new_node->distance = dist[i][j] ; 
                    new_node->link = NULL ;

                    list_node *temp = graph[i].link ;
                    if (temp == NULL) {
                        graph[i].link = new_node ;
                    }
                    else {
                        list_node *prev = NULL ;
                        while (temp != NULL) {
                            prev = temp ;
                            temp = temp->link ;
                        }
                        prev->link = new_node ;
                    }
            }
        }
    }
}

void print_list(head_node *graph) 
{
    for (int i = 0; i < ncity; i++) {
        list_node *temp = graph[i].link ;
        cout << "From " << left << setw(8) << city_table[i].second << " : " ;
        while (temp != NULL) {
            cout << "[" << left << setw(8) << city_table[temp->city].second << "| " << left << setw(3) << temp->distance << "] " ;
            temp = temp->link ;
        }
        cout << endl ;
    }
}

void print_array(vector<vector<int> > array)
{
    // row header
    cout << setw(8) << " " << " | " ;
    for (int i = 0; i < ncity; i++) {
        cout << setw(8) << city_table[i].second << " | " ;
    }
    
    // separator
    int repetitions = 120 ;
    string separator ;
    for (int r = 0; r < repetitions; r++) {
        separator += "-" ;
    }
    cout << "\n" << separator << endl ;
    
    for (int i = 0; i < ncity; i++) {
        // column header
        cout << setw(8) << city_table[i].second << " | " ;
        // content
        for (int j = 0; j < ncity; j++) {
            if (array[i][j] == INT_MIN) {
                cout << setw(8) << "INF" << " | " ;
            }
            else {
                cout << setw(8) << array[i][j] << " | " ;
            }
        }
        cout << endl ;
    }
} 

vector<vector<int> > dijkstra(head_node *graph) 
{
    vector<vector<int> > shortest(ncity, vector<int>(ncity)) ;
    vector<bool> visited(ncity) ;
    priority_queue<list_node*, vector<list_node*>, greater<list_node*> > pq ;

    for (int source = 0; source < ncity; source++) { // 모든 city 가 한 번씩 source
        // initialization
        fill(shortest[source].begin(), shortest[source].end(), INT_MAX) ;
        fill(visited.begin(), visited.end(), false) ;
        shortest[source][source] = 0 ; 
        list_node *temp = new list_node() ;
        temp->city = source ;
        temp->distance = 0 ;
        temp->link = NULL ;
        pq.push(temp) ;

        while (!pq.empty()) {
            list_node *curr_node = pq.top() ;
            pq.pop() ;
            int u = curr_node->city ;
            visited[u] = true ;

            for (list_node* neighbor = graph[u].link; neighbor != NULL; neighbor = neighbor->link) {
                int vertex = neighbor->city ;
                int weight = neighbor->distance ;

                // 방문한 적 없음 + shortest 가 업데이트 된 적이 있음 + relaxation 적용 가능
                if (!visited[vertex] && shortest[source][u] != INT_MAX && shortest[source][u] + weight < shortest[source][vertex]) {
                    shortest[source][vertex] = shortest[source][u] + weight ;
                    list_node *new_node = new list_node() ;
                    new_node->city = vertex ;
                    new_node->distance = shortest[source][vertex] ;
                    new_node->link = NULL ;
                    pq.push(new_node) ;
                }
            }
        }

        // delete curr_node ; // memory deallocation
    }    

    return shortest ;
}

vector<vector<int> > floyd(vector<vector<int> > dist)
{
    vector<vector<int> > ret(ncity, vector<int>(ncity)) ;
    
    // initialization
    for (int i = 0; i < ncity; i++) {
        for (int j = 0; j < ncity; j++) {
            if (dist[i][j] == INT_MIN) {
                ret[i][j] = INT_MAX ;
            }
            else {
                ret[i][j] = dist[i][j] ;
            }
        }
    }

    for (int k = 0; k < ncity; k++) {
        for (int i = 0; i < ncity; i++) {
            for (int j = 0; j < ncity; j++) {
                if ((ret[i][k] != INT_MAX && ret[k][j] != INT_MAX) && ret[i][k] + ret[k][j] < ret[i][j]) {
                    ret[i][j] = ret[i][k] + ret[k][j] ;
                }
            }
        }
    }

    return ret ;
}

int main()
{
    vector<vector<string> > array ;
    head_node *graph = NULL ;

    array = read_data() ;
    ncity = array.size() ;
    for (int i = 0; i < ncity; i++) { // build a city table
        city_table.push_back(make_pair(i, array[i][0])) ; // first column of the array is the name of the city
    }
    vector<vector<int> > dist(ncity, vector<int>(ncity)) ;
    convert_to_int_array(dist, array) ;
   
    create_adj_list(&graph) ;
    insert_adj_list(graph, dist);

    vector<vector<int> > darray(ncity, vector<int>(ncity)) ;
    darray = dijkstra(graph) ;
    cout << "\n[1] Dijkstra's Algorithm\n" ;
    print_array(darray) ;

    vector<vector<int> > farray(ncity, vector<int>(ncity)) ;
    farray = floyd(dist) ;
    cout << "\n[2] Floyd's Algorithm\n" ;
    print_array(farray) ;

    return 0 ;
}
