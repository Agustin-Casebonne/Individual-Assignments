import numpy as np
import time
import psutil
from numba import njit, prange
import os

# --- Definición de la matriz CSR ---
class CSRMatrix:
    def __init__(self, n, values, col_index, row_ptr):
        self.n = n
        self.values = values
        self.col_index = col_index
        self.row_ptr = row_ptr

    @staticmethod
    def generate_sparse_matrix(n, density=0.1):
        values = []
        col_index = []
        row_ptr = [0]

        for i in range(n):
            count = 0
            for j in range(n):
                if np.random.random() > density:
                    values.append(np.random.random())
                    col_index.append(j)
                    count += 1
            row_ptr.append(row_ptr[-1] + count)
        return CSRMatrix(n, np.array(values), np.array(col_index), np.array(row_ptr))

    def memory_usage_mb(self):
        total_bytes = (
            self.values.nbytes +
            self.col_index.nbytes +
            self.row_ptr.nbytes
        )
        return total_bytes / (1024*1024)

# --- Operación SpMV ---
@njit
def spmv_seq(A_values, A_col, A_row, x):
    n = len(A_row) - 1
    y = np.zeros(n)
    for i in range(n):
        s = 0.0
        for k in range(A_row[i], A_row[i+1]):
            s += A_values[k] * x[A_col[k]]
        y[i] = s
    return y

@njit(parallel=True)
def spmv_par(A_values, A_col, A_row, x):
    n = len(A_row) - 1
    y = np.zeros(n)
    for i in prange(n):
        s = 0.0
        for k in range(A_row[i], A_row[i+1]):
            s += A_values[k] * x[A_col[k]]
        y[i] = s
    return y

# --- Benchmark ---
def run_benchmark(n, runs=5, density=0.1):
    matrix = CSRMatrix.generate_sparse_matrix(n, density)
    x = np.random.random(n)

    # Medir memoria
    process = psutil.Process(os.getpid())
    mem_before = process.memory_info().rss / (1024*1024)

    # Secuencial
    t0 = time.perf_counter()
    for _ in range(runs):
        y_seq = spmv_seq(matrix.values, matrix.col_index, matrix.row_ptr, x)
    t1 = time.perf_counter()
    avg_seq = (t1 - t0) / runs

    # Paralelo
    t0 = time.perf_counter()
    for _ in range(runs):
        y_par = spmv_par(matrix.values, matrix.col_index, matrix.row_ptr, x)
    t1 = time.perf_counter()
    avg_par = (t1 - t0) / runs

    speedup = avg_seq / avg_par if avg_par > 0 else float('inf')
    efficiency = speedup / os.cpu_count() if os.cpu_count() else 0.0
    mem_after = process.memory_info().rss / (1024*1024)
    mem_usage = mem_after - mem_before + matrix.memory_usage_mb()

    print(f"N={n}: Sequential {avg_seq:.6f}s | Parallel {avg_par:.6f}s | "
          f"Speedup {speedup:.2f}x | Efficiency {efficiency:.2f}")
    print(f"Memory usage: {mem_usage:.2f} MB")
    print("---------------------------------------------------------")
