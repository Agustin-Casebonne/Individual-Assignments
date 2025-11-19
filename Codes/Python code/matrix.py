import random
import time
import multiprocessing as mp



def generate_sparse_matrix_csr(N, sparsity=0.95):
    """
    Generates a sparse matrix in CSR format.
    sparsity = fraction of zeros.
    """

    values = []
    col_index = []
    row_ptr = [0]

    for i in range(N):
        count = 0
        for j in range(N):
            if random.random() > sparsity:       
                values.append(random.random())
                col_index.append(j)
                count += 1
        row_ptr.append(row_ptr[-1] + count)

    return values, col_index, row_ptr, N



def spmv_sequential(values, col_index, row_ptr, N, x, y):
    for i in range(N):
        start = row_ptr[i]
        end = row_ptr[i + 1]
        s = 0.0
        for idx in range(start, end):
            s += values[idx] * x[col_index[idx]]
        y[i] = s


# Paralelo

def _spmv_row_task(args):
    """Trabajo para cada fila en modo paralelo"""
    i, values, col_index, row_ptr, x = args
    start = row_ptr[i]
    end = row_ptr[i + 1]
    total = 0.0
    for idx in range(start, end):
        total += values[idx] * x[col_index[idx]]
    return total


def spmv_parallel(values, col_index, row_ptr, N, x, y):
    with mp.Pool(mp.cpu_count()) as pool:
        results = pool.map(
            _spmv_row_task,
            [(i, values, col_index, row_ptr, x) for i in range(N)]
        )
    for i in range(N):
        y[i] = results[i]



def run_benchmark(N, RUNS):
    SPARSITY = 0.95

    values, col_index, row_ptr, _ = generate_sparse_matrix_csr(N, SPARSITY)
    x = [random.random() for _ in range(N)]
    y = [0.0] * N
    y_par = [0.0] * N

    print(f"Running Python Sparse SpMV for N={N} ({RUNS} runs)...")

    total_seq = 0.0
    total_par = 0.0

    # Warm-up
    spmv_sequential(values, col_index, row_ptr, N, x, y)

    # Benchmark
    for r in range(RUNS):
        start = time.perf_counter()
        spmv_sequential(values, col_index, row_ptr, N, x, y)
        end = time.perf_counter()
        total_seq += end - start

        start = time.perf_counter()
        spmv_parallel(values, col_index, row_ptr, N, x, y_par)
        end = time.perf_counter()
        total_par += end - start

    avg_seq = total_seq / RUNS
    avg_par = total_par / RUNS

    print(f"Sequential: {avg_seq:.6f} s")
    print(f"Parallel:   {avg_par:.6f} s")
    print(f"Speedup: {avg_seq/avg_par:.2f}x")

    return avg_seq
