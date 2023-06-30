// 알고리듬분석_01분반_22100768_한나린

/*  REFERENCES
    (1) CPP file named 'graph_const_7.cpp' professor provided
    (2) Blog (malloc, calloc, new) : https://seulgit.tistory.com/81
    (3) StackExchange (Runtime of algorithm to transpose a graph) : https://cs.stackexchange.com/questions/64846/runtime-of-algorithm-to-transpose-a-graph
*/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctype.h>
#include <cmath>

#define MAX_NODE 20 // Assume number of nodes <= 20

using namespace std;

int num; // number of nodes

typedef struct g_node
{
    char point;
    int edge;
    struct g_node *link;
} list_node;

typedef struct h_node
{
    int data;
    struct g_node *link;
} head_node;

vector<vector<int> > read_data() { // read data from a file and construct an adjacency array
    ifstream file("hw5_data.txt");
    if (!file) {
        cout << "Failed to open a file!\n";
        exit(1);
    }

    vector<vector<int> > array;
    vector<int> temp;
    char ch;
    while (!file.eof()) {
        file.get(ch);
        if (isdigit(ch)) // store the edge data only when it is a number
            temp.push_back(ch - '0'); // store in int type
    }
    num = sqrt(temp.size()); // find the number of nodes
    if (num > MAX_NODE) {
        cout << "Number of nodes are more than 20! Please check!\n";
        exit(1);
    }

    for (int i = 0; i < num; i++) {
        vector<int> row;
        for (int j = 0; j < num; j++) {
            row.push_back(temp[8 * i + j]);
        }
        array.push_back(row);
    }

    file.close();

    return array;
}

void create_adj_list(head_node **graph) // create adjacency list
{
    *graph = new head_node[num];
    for (int i = 0; i < num; i++) {
        (*graph)[i].data = i;
        (*graph)[i].link = NULL;
    }
}

void insert_adj_list(head_node *graph, vector<vector<int> > array) // insert the data into adjacency list
{
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            if (array[i][j] > 0) {
                list_node *new_node = new list_node;
                new_node->point = (char)(j + 65);
                new_node->edge = array[i][j];
                new_node->link = NULL;

                list_node *temp = graph[i].link;
                if (temp == NULL) {
                    graph[i].link = new_node;
                }
                else {
                    list_node *prev = NULL;
                    while (temp != NULL) {
                        prev = temp;
                        temp = temp->link;
                    }
                    prev->link = new_node;
                }
            }
        }
    }
}

// compute transpose of graph using adjacency array : Theta(V^2)
vector<vector<int> > compute_transpose_by_array(vector<vector<int> > array) 
{
    vector<vector<int> > transpose(num, vector<int>(num, 0));
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            transpose[j][i] = array[i][j];
        }
    }

    return transpose;
}

// compute transpose of graph using adjacency list 
void compute_transpose_by_list(head_node *graph, head_node *tran_graph) 
{
    list_node *temp;

    for (int i = 0; i < num; i++) {
        temp = graph[i].link;
        while (temp != NULL) {
            int vertex = (int)(temp->point) - 65;
            list_node *new_node = new list_node;
            new_node->point = (char)(i + 65);
            new_node->edge = temp->edge;
            new_node->link = NULL;

            list_node *tran_node = tran_graph[vertex].link;
            if (tran_node == NULL) {
                tran_graph[vertex].link = new_node;
            }
            else {
                list_node *prev = NULL;
                while (tran_node != NULL) {
                    prev = tran_node;
                    tran_node = tran_node->link;
                }
                prev->link = new_node;
            }
            temp = temp->link;
        }
    }
}

void print_array(vector<vector<int> > array) 
{
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            cout << array[i][j] << " ";
        }
        cout << "\n";
    }
}

void print_list(head_node *graph) 
{
    for (int i = 0; i < num; i++) {
        list_node *temp = graph[i].link;
        char ch = (char)(i + 65);
        cout << "From " << ch << " : ";
        while (temp != NULL) {
            cout << temp->point << "   ";
            temp = temp->link; 
        }
        cout << endl;
    }
}

int main() {
    head_node *graph = NULL;
    
    vector<vector<int> > array = read_data();
    // cout << "====== Original Array ======\n";
    // print_array(array);

    create_adj_list(&graph);
    insert_adj_list(graph, array);
    cout << "\n====== Array of Original Adjacency List ======\n";
    print_list(graph);
    
    vector<vector<int> > transpose = compute_transpose_by_array(array);
    // cout << "\n====== Transpose Array ======\n";
    // print_array(transpose);

    head_node *tran_graph = NULL;
    create_adj_list(&tran_graph);
    compute_transpose_by_list(graph, tran_graph);
    cout << "\n====== Array of Transpose Adjacency List ======\n";
    print_list(tran_graph);

    return 0;
}