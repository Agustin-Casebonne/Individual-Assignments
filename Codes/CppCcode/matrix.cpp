#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include "matrix.h"

using namespace std;
using namespace std::chrono;

// Multiplicación estándar IJK
void multiply_matrices(int N, const vector<vector<double>>& A, const vector<vector<double>>& B, vector<vector<double>>& C) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            double sum = 0.0;
            for (int k = 0; k < N; ++k) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = sum;
        }
    }
}

// Inicialización de matrices con números aleatorios
void initialize_matrices(int N, vector<vector<double>>& A, vector<vector<double>>& B, vector<vector<double>>& C) {
    A.assign(N, vector<double>(N));
    B.assign(N, vector<double>(N));
    C.assign(N, vector<double>(N, 0.0));

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> distrib(0.0, 1.0);

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = distrib(gen);
            B[i][j] = distrib(gen);
        }
    }
}

// Ejecuta el benchmark y devuelve tiempo promedio
double run_benchmark(int N, int RUNS) {
    vector<vector<double>> A, B, C;
    initialize_matrices(N, A, B, C); 

    long long total_duration_us = 0;

    cout << "Running C++ benchmark for N=" << N << " (" << RUNS << " runs)..." << endl;

    for (int r = 0; r < RUNS; ++r) {
        C.assign(N, vector<double>(N, 0.0)); 
        
        auto start = high_resolution_clock::now();
        multiply_matrices(N, A, B, C);
        auto end = high_resolution_clock::now();
        
        auto duration = duration_cast<microseconds>(end - start);
        total_duration_us += duration.count();
    }

    double average_duration_s = (double)total_duration_us / RUNS / 1'000'000.0;
    cout << "C++ N=" << N << " Average Time: " << fixed << setprecision(6) << average_duration_s << " s" << endl;

    // Aproximación simple de memoria usada por matrices (en MB)
    size_t mem_bytes = (sizeof(double) * 3 * N * N);
    cout << "→ Approx. Memory used by matrices: " << mem_bytes / (1024.0 * 1024.0) << " MB" << endl;

    return average_duration_s;
}
