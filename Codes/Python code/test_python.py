import psutil
import os
from matrix import run_benchmark

def monitor_and_run(N, RUNS):
    """
    Executes the benchmark while showing CPU and memory usage.
    """
    pid = os.getpid()
    process = psutil.Process(pid)

    print(f"\n--- Testing Sparse SpMV N={N} ---")
    process.cpu_percent(interval=None)  

    avg_time = run_benchmark(N, RUNS)

    cpu_percent = process.cpu_percent(interval=None)
    mem_usage = process.memory_info().rss / (1024 ** 2)  # MB

    print(f"→ CPU usage: {cpu_percent:.1f}% | Memory: {mem_usage:.1f} MB")
    print(f"→ Average time: {avg_time:.6f} s")
    print("---------------------------------------------------------\n")


if __name__ == "__main__":
    RUNS = 5
    MATRIX_SIZES = [512, 1024, 1536]

    print("=== Python Sparse Matrix-Vector Multiplication (SpMV CSR) ===")
    for N in MATRIX_SIZES:
        monitor_and_run(N, RUNS)
