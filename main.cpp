#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;

mutex mtx;

long long combinations_len(const int n, const int r) {
    int k = n - r;
    double result = 1;
    for (int i = 2; i < n + 1; i++) {
        if (i < r + 1){
            if (i < k + 1) {
                result *= (double) 1 / i;
            }
        } 
        else if (i < k + 1) {
            if (i < r + 1) {
                result *= (double) 1 / i;
            }
        }
        else {
            result *= (double) i;
        } 
    }
    return result;
}

void combos(const int r, const long long tail_len, int* const tail_of_combs,
const int body_rows, const long long body_len, int** const body_of_combs) {
    int combination = (1 << r) - 1;
    // filling body of combs
    for (int i = 0; i < body_rows; i++)
    {
        for (long long j = 0; j < body_len; j++)
        {
            // save combination
            body_of_combs[i][j] = combination;

            int x = combination & -combination;
            int y = combination + x;
            int z = (combination & ~y);
            combination = z / x;
            combination >>= 1;
            combination |= y;
        }
    }
    

    // filling tail of combs
    for (long long i = 0; i < tail_len; i++)
    {
        // save combination
        tail_of_combs[i] = combination;

        int x = combination & -combination;
        int y = combination + x;
        int z = (combination & ~y);
        combination = z / x;
        combination >>= 1;
        combination |= y;
    }
    
}

void calculate_array_of_combs(const vector<double> arr, vector<double>* res_arr, int* const combs_arr, const long long combs_arr_len){
    int inp_floats_len = arr.size();

    double lower_bound = 0.33999999;
    double upper_bound = 0.34000001;

    for (long long i = 0; i < combs_arr_len; i++){

        double sum = 0;
        for (int j = 0; j < inp_floats_len; j++)
        {
            if ((combs_arr[i] >> j) & 1)
            {
                sum += arr[j];
            }
        }
        sum /= 10;
        if (lower_bound < sum)
        {
            if (sum < upper_bound)
            {
                mtx.lock();
                // saving the sum
                res_arr -> push_back(sum);
                cout << ".";
                mtx.unlock();
            }
        }
    }
}

template <class T>
void print_array(const T &arr, const long long arr_len) {
    for (int i = 0; i < arr_len; i++)
    {
        cout << arr[i] << endl;
    }
    
}

int main()
{
    vector<double> inputFloats {
        0.36951738595962524,
        0.06341691315174103,
        0.22787682712078094,
        0.33528494834899900,
        0.35664117336273193,
        0.34278947114944460,
        0.34722217917442320,
        0.35534474253654480,
        0.33563575148582460,
        0.35207062959671020,
        0.33868536353111267,
        0.33861255645751953,
        0.34092786908149720,
        0.34524348378181460,
        0.33949270844459534,
        0.34907341003417970,
        0.33099761605262756,
        0.33712810277938840,
        0.33012279868125916,
        0.35278642177581787,
        0.24193483591079712,
        0.29794731736183167,
        0.30697146058082580,
        0.22207792103290558,
        0.19377246499061584,
        0.19113427400588990,
        0.21730311214923860,
        0.26347228884696960,
        0.21534232795238495,
        0.33051928877830505,
        0.28944417834281920,
        0.30140167474746704,
        0.36951738595962524,
        0.06341691315174103,
        0.22787682712078094,
        0.33528494834899900,
        0.35664117336273193,
        0.34278947114944460,
    };
    vector<double> resultCombs;
    resultCombs.reserve(100);

    int r = 10;
    int n = inputFloats.size();
    int n_threads = _Thrd_hardware_concurrency();

    long long combs_len = combinations_len(n, r);
    cout << "Input combs = " << n << endl;
    cout << "Combs length =\t" << combs_len << endl;

    int body_rows = n_threads - 1;
    long long body_cols_len = combs_len / n_threads;
    long long tail_len = combs_len - body_cols_len * (n_threads - 1);
    cout << "Tail length =\t" << tail_len << endl;

    // arrays of combs declaration
    int *tail_of_combs = new int[tail_len];
    int **body_of_combs = new int*[body_rows];
    for (int i = 0; i < body_rows; i++)
    {
        body_of_combs[i] = new int[body_cols_len];
    }

    // initialization of body and tail with combos
    combos(r, tail_len, tail_of_combs, body_rows, body_cols_len, body_of_combs);

    // parallel calculating of floats combinations
    thread *body_threads = new thread[body_rows];

    for (int i = 0; i < body_rows; i++)
    {
        body_threads[i] = thread(calculate_array_of_combs, inputFloats, &resultCombs, body_of_combs[i], body_cols_len);
    }
    
    calculate_array_of_combs(inputFloats, &resultCombs, tail_of_combs, tail_len);  // tail in main thread
    
    for (int i = 0; i < body_rows; i++)
    {
        body_threads[i].join();
    }
    

    cout << endl << "RESULT ARRAY:" << endl;
    print_array(resultCombs, resultCombs.size());


    
    // cleaning memory
    delete[] body_threads;  

    for (int i = 0; i < body_rows; i++)
    {
        delete[] body_of_combs[i];
    }
    delete[] body_of_combs;
    body_of_combs = nullptr;

    delete[] tail_of_combs;
    tail_of_combs = nullptr;
}
