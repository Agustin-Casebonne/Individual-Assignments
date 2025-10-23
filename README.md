Big Data Course: Individual Assignment (Matrix Multiplication Benchmark)

Project Overview (Assignment 1)

This repository contains the code and documentation for the "Basic Matrix Multiplication in Different Languages" assignment for the Big Data course at the Universidad de Las Palmas de Gran Canaria (ULPGC).

The main objective of this assignment is to conduct a comparative performance analysis (benchmarking) of the standard $O(N^3)$ matrix multiplication algorithm implemented in three different languages: C++, Java, and Python (with Rust included as an optional high-performance language).

The study focuses on measuring and analyzing execution time, scalability, and computational overhead as the matrix size ($N$) increases, adhering to professional benchmarking and profiling methodologies.

Repository Structure

The project artifacts are organized to meet the submission requirements, ensuring separation between implementation, testing data, and final documentation.

Individual_Assignment_Matrix_Multiplication/

├── Codes/

│   ├── C++ code/          
│   ├── Java code/           
│   ├── Python code/         
│   └── Rust code/           
├── Datos/                  
└── Documentacion/        


How to Run the Benchmarks

All runner files (test-c++.cpp, TestBenchmark.java, test_python.py, matrix_rust.rs) are configured to run 5 trials for matrix sizes $N \in \{512, 1024, 1536\}$.
