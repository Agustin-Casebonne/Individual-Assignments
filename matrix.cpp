#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace std::chrono;


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

    double average_duration_s = (double)total_duration_us / RUNS / 1000000.0;
    cout << "C++ N=" << N << " Average Time: " << fixed << setprecision(6) << average_duration_s << " s" << endl;
    
    return average_duration_s;
}


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
