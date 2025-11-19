#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include "matrix.h"

using namespace std;
using namespace std::chrono;


struct CSRMatrix {
    vector<double> values;  
    vector<int> colIndex;  
    vector<int> rowPtr;      
    int n;
};


CSRMatrix generate_sparse_matrix(int N, double sparsity = 0.95) {
    CSRMatrix M;
    M.n = N;
    M.rowPtr.resize(N + 1);
    M.rowPtr[0] = 0;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(0.0, 1.0);

    for (int i = 0; i < N; i++) {
        int count = 0;
        for (int j = 0; j < N; j++) {
            if (distrib(gen) > sparsity) {  
                M.values.push_back(distrib(gen));
                M.colIndex.push_back(j);
                count++;
            }
        }
        M.rowPtr[i + 1] = M.rowPtr[i] + count;
    }

    return M;
}


void spmv_sequential(const CSRMatrix& A, const vector<double>& x, vector<double>& y) {
    int N = A.n;
    y.assign(N, 0.0);

    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++) {
            sum += A.values[k] * x[A.colIndex[k]];
        }
        y[i] = sum;
    }
}

void spmv_parallel(const CSRMatrix& A, const vector<double>& x, vector<double>& y) {
    int N = A.n;
    y.assign(N, 0.0);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++) {
            sum += A.values[k] * x[A.colIndex[k]];
        }
        y[i] = sum;
    }
}


double run_benchmark(int N, int RUNS) {
    const double SPARSITY = 0.95;

    CSRMatrix A = generate_sparse_matrix(N, SPARSITY);
    vector<double> x(N), y(N), y_par(N);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(0.0, 1.0);

    for (int i = 0; i < N; i++) x[i] = distrib(gen);

    cout << "Running C++ Sparse SpMV for N=" << N << " (" << RUNS << " runs)..." << endl;

    long long total_seq = 0;
    long long total_par = 0;

    // Warm-up
    spmv_sequential(A, x, y);

    for (int r = 0; r < RUNS; r++) {
        auto start = high_resolution_clock::now();
        spmv_sequential(A, x, y);
        auto end = high_resolution_clock::now();
        total_seq += duration_cast<microseconds>(end - start).count();

        start = high_resolution_clock::now();
        spmv_parallel(A, x, y_par);
        end = high_resolution_clock::now();
        total_par += duration_cast<microseconds>(end - start).count();
    }

    double avg_seq = (double)total_seq / RUNS / 1'000'000.0;
    double avg_par = (double)total_par / RUNS / 1'000'000.0;

    cout << "Sequential: " << fixed << setprecision(6) << avg_seq << " s\n";
    cout << "Parallel:   " << fixed << setprecision(6) << avg_par << " s\n";
    cout << "Speedup: " << (avg_seq / avg_par) << "x\n";

    return avg_seq;
}
