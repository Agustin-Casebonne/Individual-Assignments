import random
import time
import sys

sys.setrecursionlimit(2000)

def multiply_matrices(N, A, B, C):
    """
    Performs standard matrix multiplication C = A * B.
    Uses the classic I-J-K loop order (pure Python).
    """
    for i in range(N):
        for j in range(N):
            sum_val = 0.0
            for k in range(N):
                sum_val += A[i][k] * B[k][j]
            C[i][j] = sum_val


def initialize_matrices(N):
    """
    Initializes two N x N matrices with random float values and a result matrix C with zeros.
    Returns A, B, C.
    """
    def generate_matrix(size):
        return [[random.random() for _ in range(size)] for _ in range(size)]

    A = generate_matrix(N)
    B = generate_matrix(N)
    C = [[0.0 for _ in range(N)] for _ in range(N)]
    return A, B, C

def run_benchmark(N, RUNS):
    """
    Runs the matrix multiplication multiple times and returns the average duration.
    """
    A, B, C_template = initialize_matrices(N)

    total_duration_s = 0.0

    print(f"Running Python benchmark for N={N} ({RUNS} runs)...")

    for r in range(RUNS):
        C = [[0.0 for _ in range(N)] for _ in range(N)]
        
        start = time.perf_counter()
        multiply_matrices(N, A, B, C)
        end = time.perf_counter()
        
        duration = end - start
        total_duration_s += duration

    average_duration_s = total_duration_s / RUNS
    print(f"Python N={N} Average Time: {average_duration_s:.6f} s")
    
    return average_duration_s

def main():
    
    RUNS = 5
    MATRIX_SIZES = [512, 1024] 

    print("--- Python Matrix Multiplication Benchmark  ---")

    for N in MATRIX_SIZES:
        run_benchmark(N, RUNS)
        print("---------------------------------------------------------")

if __name__ == "__main__":
    main()
