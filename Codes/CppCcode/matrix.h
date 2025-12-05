#ifndef MATRIX_H
#define MATRIX_H

#include <vector>

struct CSRMatrix {
    std::vector<double> values;
    std::vector<int> colIndex;
    std::vector<int> rowPtr;
    int n;
};

// Generar matriz dispersa
CSRMatrix generate_sparse_matrix(int N, double sparsity = 0.95);

// SpMV secuencial
void spmv_sequential(const CSRMatrix& A, const std::vector<double>& x, std::vector<double>& y);

// SpMV paralelo con OpenMP
void spmv_parallel(const CSRMatrix& A, const std::vector<double>& x, std::vector<double>& y);

// SpMV vectorizado (simd) opcional
void spmv_vectorized(const CSRMatrix& A, const std::vector<double>& x, std::vector<double>& y);

// Benchmark completo
void run_benchmark(int N, int RUNS);

#endif
