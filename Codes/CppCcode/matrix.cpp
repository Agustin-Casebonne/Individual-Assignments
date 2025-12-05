#include "matrix.h"
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>
#include <iomanip>
using namespace std;
using namespace std::chrono;

// Generación de matriz dispersa en formato CSR
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

// Producto matriz-vector secuencial
void spmv_sequential(const CSRMatrix& A, const vector<double>& x, vector<double>& y) {
    int N = A.n;
    y.assign(N, 0.0);

    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++)
            sum += A.values[k] * x[A.colIndex[k]];
        y[i] = sum;
    }
}

// Producto matriz-vector paralelo
void spmv_parallel(const CSRMatrix& A, const vector<double>& x, vector<double>& y) {
    int N = A.n;
    y.assign(N, 0.0);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++)
            sum += A.values[k] * x[A.colIndex[k]];
        y[i] = sum;
    }
}

// Producto matriz-vector vectorizado (simulado para MSVC, sin #pragma omp simd)
void spmv_vectorized(const CSRMatrix& A, const vector<double>& x, vector<double>& y) {
    int N = A.n;
    y.assign(N, 0.0);

#pragma omp parallel for schedule(static)
    for (int i = 0; i < N; i++) {
        double sum = 0.0;
        // #pragma omp simd  // Comentado para MSVC
        for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++)
            sum += A.values[k] * x[A.colIndex[k]];
        y[i] = sum;
    }
}

// Benchmark completo
void run_benchmark(int N, int RUNS) {
    CSRMatrix A = generate_sparse_matrix(N);
    vector<double> x(N), y_seq(N), y_par(N), y_vec(N);

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(0.0, 1.0);

    for (int i = 0; i < N; i++) x[i] = distrib(gen);

    cout << "Running C++ Sparse SpMV for N=" << N << " (" << RUNS << " runs)..." << endl;

    double total_seq = 0.0, total_par = 0.0, total_vec = 0.0;

    // Warm-up
    spmv_sequential(A, x, y_seq);
    spmv_parallel(A, x, y_par);
    spmv_vectorized(A, x, y_vec);

    // Benchmark
    for (int r = 0; r < RUNS; r++) {
        auto start = high_resolution_clock::now();
        spmv_sequential(A, x, y_seq);
        auto end = high_resolution_clock::now();
        total_seq += duration<double>(end - start).count();

        start = high_resolution_clock::now();
        spmv_parallel(A, x, y_par);
        end = high_resolution_clock::now();
        total_par += duration<double>(end - start).count();

        start = high_resolution_clock::now();
        spmv_vectorized(A, x, y_vec);
        end = high_resolution_clock::now();
        total_vec += duration<double>(end - start).count();
    }

    double avg_seq = total_seq / RUNS;
    double avg_par = total_par / RUNS;
    double avg_vec = total_vec / RUNS;

    double speedup_par = avg_seq / avg_par;
    double speedup_vec = avg_seq / avg_vec;
    int num_threads = omp_get_max_threads();
    double efficiency_par = speedup_par / num_threads;
    double efficiency_vec = speedup_vec / num_threads;

    cout << fixed << setprecision(6);
    cout << "Sequential: " << avg_seq << " s\n";
    cout << "Parallel:   " << avg_par << " s | Speedup: " << speedup_par << "x | Efficiency: " << efficiency_par << "\n";
    cout << "Vectorized: " << avg_vec << " s | Speedup: " << speedup_vec << "x | Efficiency: " << efficiency_vec << "\n";
    cout << "---------------------------------------------------------\n";
}
