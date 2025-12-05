from matrix import run_benchmark

if __name__ == "__main__":
    runs = 5
    sizes = [512, 1024, 1536, 2048]

    print("--- Python Sparse Matrix-Vector Benchmark (SpMV CSR) ---")
    for n in sizes:
        run_benchmark(n, runs=runs)
