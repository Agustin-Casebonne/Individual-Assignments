#include <iostream>
#include <vector>
#include "matrix.h"

using namespace std;

int main() {
    const int RUNS = 5;
    const vector<int> MATRIX_SIZES = {512, 1024, 1536};

    cout << "--- C++ Matrix Multiplication Benchmark (Standard IJK) ---" << endl;

    for (int N : MATRIX_SIZES) {
        run_benchmark(N, RUNS);
        cout << "---------------------------------------------------------" << endl;
    }

    return 0;
}
 