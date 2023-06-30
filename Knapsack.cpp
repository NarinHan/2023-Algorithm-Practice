/*  REFERENCES
    (1) Lecture slide Chapter 6 : p. 13-28
    (2) Hyperscale AI ChatGPT
    (3) GitHub : https://gist.github.com/YJDave/c9ad61598bbe6d059ef0396b77bbd612
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <queue>
#include <ctime>
#include <cmath>

#define NUM 25

using namespace std ;

int items_bf[3] = { 11, 21, 31 } ;
int items_others[4] = { 10, 100, 1000, 10000 } ;
clock_t start, stop;
double duration = 0 ;

// Generate random values of weight and benefit
// Return type : pair of int in vector
vector<pair<int, int> > generateRandomValues(int n) 
{ 
    int weight ;
    int benefit ;
    vector<pair<int, int> > ret ;
    const int seed = 100 ;
    const int MAX_WEIGHT = 100 ;
    const int MAX_BENEFIT = 500 ;

    srand(seed) ;

    for(int i = 0; i < n; i++) {
        weight = rand() % (MAX_WEIGHT + 1) ;
        benefit = rand() % (MAX_BENEFIT + 1) ;

        ret.push_back(make_pair(weight, benefit)) ;
    }

/*  
    // print randomly generated values
    for(int k = 0; k < n; k++) {
        cout << k + 1 << " " << ret[k].second << " " << ret[k].first << " " << (double)ret[k].second / ret[k].first << "\n" ;
    }
*/

    return ret ;
}

// Compute in a brute force approach
// Return type : pair of processing time and maximum benefit
pair<double, int> bruteForce(vector<pair<int, int> > wb, int W) 
{
    int max_benefit = 0 ;

    start = clock() ; // measure the processing time
    
    for(unsigned long long int i = 0; i < (1ull << wb.size()); i++) {
        int total_weight = 0 ;
        int total_benefit = 0 ;
        for(unsigned long long int j = 0; j < wb.size(); j++) {
            if((i & (1ull << j)) != 0) {
                total_weight += wb[j].first ;
                total_benefit += wb[j].second ;
            }
        }
        if(total_weight <= W) {
            max_benefit = max(max_benefit, total_benefit) ;
        }
    }

    stop = clock() ; // measure the processing time
    duration = (double) (stop - start) / CLOCKS_PER_SEC ;

    return make_pair(duration, max_benefit) ;
}

// Sort benefit/weight ratio in a descending order
bool comp(pair<int, int> a, pair<int, int> b) 
{
    double r1 = (double)a.second / a.first ;
    double r2 = (double)b.second / b.first ;
    return r1 > r2 ;
}

// Compute in a greedy approach
// Return type : pair of processing time and maximum benefit
pair<double, double> greedy(vector<pair<int, int> > wb, int W) 
{
    sort(wb.begin(), wb.end(), comp) ; // sort benefit/weight ratio in a descending order
    
    int total_weight = 0 ;
    double max_benefit = 0.0 ;
    
    start = clock() ; // measure the processing time
    
    for(int i = 0; i < wb.size(); i++) {
        // if weight does not overflow the maximum capacity, add it completely
        if(total_weight + wb[i].first <= W) {
            total_weight += wb[i].first ;
            max_benefit += wb[i].second ;
        }
        // if weight overflows the maximum capacity, add fractional part
        else {
            int remain = W - total_weight ;
            max_benefit += ((double)wb[i].second / wb[i].first) * remain ;
            break ;
        }
    }
    
    stop = clock() ; // measure the processing time
    duration = (double) (stop - start) / CLOCKS_PER_SEC ;

    return make_pair(duration, max_benefit) ;
}

// Compute in a dynamic programming approach
// Return type : pair of processing time and maximum benefit
pair<double, int> dynamicProgramming(vector<pair<int, int> > wb, int W) 
{
    int n = wb.size() ;

    int** arr = new int*[n+1] ; // allocate memory for rows
    for(int i = 0; i <= n; i++) {
        arr[i] = new int[W+1] ; // allocate memory for columns
    }
    
    start = clock() ; // measure the processing time

    for(int i = 0; i <= W; i++) { // fill the first row with 0
        arr[0][i] = 0 ;
    }
    for(int i = 1; i <= n; i++) {
        arr[i][0] = 0; // fill the first column with 0
        for(int j = 1; j <= W; j++) {
            if(wb[i-1].first <= j) {
                if(wb[i-1].second + arr[i-1][j-wb[i-1].first] > arr[i-1][j]) {
                    arr[i][j] = wb[i-1].second + arr[i-1][j-wb[i-1].first] ;
                }
                else {
                    arr[i][j] = arr[i-1][j] ;
                }
            }
            else {
                arr[i][j] = arr[i-1][j] ;
            }
        }
    }
    int max_benefit = arr[n][W] ;

    stop = clock() ; // measure the processing time
    duration = (double) (stop - start) / CLOCKS_PER_SEC ;

    for(int i = 0; i <= n; i++) {
        delete[] arr[i]; // deallocate memory for columns
    }
    delete[] arr; // deallocate memory for rows

    return make_pair(duration, max_benefit) ;
}

// Node for branch and bound approach
struct node 
{
    int level ;
    int weight ;
    int benefit ;
    int bound ;
} ;

// Overloading 'less than' operator to sort the nodes in priority queue
bool operator<(const node &a, const node &b) {
    return a.bound < b.bound;
}

// Compute the benefit bound for branch and bound approach
int bound(node u, vector<pair<int, int> > wb, int W) 
{
    if(u.weight >= W)
        return 0 ;

    int k = u.level + 1 ;
    int total_weight = u.weight ;
    int benefit_bound = u.benefit ;

    while((k < wb.size()) && total_weight + wb[k].first <= W) {
        total_weight += wb[k].first ;
        benefit_bound += wb[k].second ;
        k++ ;
    }

    // fractional 로 자른다고 가정
    if(k < wb.size()) {
        int remain = W - total_weight ;
        benefit_bound += ((double)wb[k].second / wb[k].first) * remain ;
    }

    return benefit_bound ;
}

pair<double, int> branchBound(vector<pair<int, int> > wb, int W)
{
    int max_benefit = 0 ;
    sort(wb.begin(), wb.end(), comp) ; // sort benefit/weight ratio in a descending order

    priority_queue<node> pq ;
    node u, v ;

    start = clock() ; // measure the processing time

    // set start node and push it to the priority queue
    u.level = 0 ;
    u.weight = 0 ;
    u.benefit = 0 ;
    u.bound = bound(u, wb, W) ;
    pq.push(u) ;

    while(!pq.empty()) {
        u = pq.top() ;
        pq.pop() ; // pop the node with highest bound

        // for each child of the popped node, calculate its bound value and add it to the priority queue
        // if its bound value is greater than the current maximum benefit value
        if(u.bound > max_benefit) {
            v.level = u.level + 1 ;
            v.weight = u.weight + wb[v.level - 1].first ;
            v.benefit = u.benefit + wb[v.level - 1].second ;
            if(v.weight <= W) {
                max_benefit = max(max_benefit, v.benefit) ;
            }
            v.bound = bound(v, wb, W) ;
            if(v.bound > max_benefit) {
                pq.push(v) ;
            }

            v.weight = u.weight ;
            v.benefit = u.benefit ;
            v.bound = bound(v, wb, W) ;
            if(v.bound > max_benefit) {
                pq.push(v) ;
            }
        }
    }

    stop = clock() ; // measure the processing time
    duration = (double) (stop - start) / CLOCKS_PER_SEC ;

    return make_pair(duration, max_benefit) ;
}

int main() 
{
    vector<pair<int, int> > wb ; // weight and benefit
    int W ; // maximum capacity
    
    pair<double, int> tempdi ;
    pair<double, double> tempdd ;
    
    vector<pair<double, double> > res_bf ;
    vector<pair<double, double> > res_greedy ;
    vector<pair<double, int> > res_dp ;
    vector<pair<double, int> > res_bb ;

    for(int i = 0; i < 3; i++) {
        wb = generateRandomValues(items_bf[i]) ; // generate random values of weight and benefit
        W = items_bf[i] * NUM ; // determine maximum capacity

        tempdi = bruteForce(wb, W);
        res_bf.push_back(tempdi) ;

        wb.clear() ; // wb 내용 삭제
        vector<pair<int, int> >().swap(wb) ; // wb 메모리 할당 해제 
    }

    for(int i = 0; i < 4; i++) {
        wb = generateRandomValues(items_others[i]) ; // generate random values of weight and benefit
        W = items_others[i] * NUM ; // determine maximum capacity
        
        tempdd = greedy(wb, W) ;
        res_greedy.push_back(tempdd) ;

        tempdi = dynamicProgramming(wb, W) ;
        res_dp.push_back(tempdi) ;
        
        tempdi = branchBound(wb, W) ;
        res_bb.push_back(tempdi) ;
       
        wb.clear() ; // wb 내용 삭제
        vector<pair<int, int> >().swap(wb) ; // wb 메모리 할당 해제 
    }

    // print the results in a table format
    cout << "[1] Brute Force\n" ;
    cout << "--------------------------------------------------------------------------------------------------------------\n" ;
    cout << setw(15) << "Number of items" << " | " ;
    cout << setw(45) << "Processing time in ms / Maximum benefit value" << endl ;
    cout << "--------------------------------------------------------------------------------------------------------------\n" ;

    for(int i = 0; i < 3; i++) {
        cout << setw(15) << right << items_bf[i] << " | " ;
        cout << setw(21) << res_bf[i].first << right << " / " << setw(24) << left << res_bf[i].second << "\n" ;
    }

    cout << "\n[2] Greedy / Dynamic Programming / Branch and Bound" << endl ;
    cout << "--------------------------------------------------------------------------------------------------------------\n" ;
    cout << setw(15) << "Number of items" << " | " ;
    cout << setw(81) << "Processing time in ms / Maximum benefit value" << endl ;
    cout << setw(15) << "" << " | " ;
    cout << setw(27) << "Greedy" << " | " ;
    cout << setw(27) << "D.P." << " | " ;
    cout << setw(27) << "B & B" << endl ;
    cout << "--------------------------------------------------------------------------------------------------------------\n" ;

    for(int i = 0; i < 4; i++) {
        cout << setw(15) << right << items_others[i] << " | " ;
        cout << setw(12) << left << res_greedy[i].first << " / " << setw(12) << left << res_greedy[i].second << " | " ;
        cout << setw(12) << left << res_dp[i].first << " / " << setw(12) << left << res_dp[i].second << " | " ;
        cout << setw(12) << left<< res_bb[i].first << " / " << setw(12) << left << res_bb[i].second << "\n" ;
    }
    
    return 0 ;
}
