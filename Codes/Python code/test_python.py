from matrix import *
RUNS = 5
MATRIX_SIZES = [512, 1024,1536] 

print("--- Python Matrix Multiplication Benchmark  ---")

for N in MATRIX_SIZES:
    run_benchmark(N, RUNS)
    print("---------------------------------------------------------")