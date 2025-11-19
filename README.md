# Matrix Multiplication Benchmarking Project 🚀

**Author:** Agustín Casebonne
**Course:** Individual Assignment - High Performance Computing
**Date:** November 2025

## 📋 Overview

This repository contains a rigorous comparative analysis of matrix multiplication algorithms implemented in **C++, Java, Python, and Rust**. The study focuses on performance profiling, memory management, and the impact of parallelization across different programming paradigms.

The project is divided into two key stages:
1.  **Stage 1: Dense Matrix Multiplication:** Standard $O(N^3)$ algorithm.
2.  **Stage 2: Sparse Matrix Multiplication (SpMV):** Memory-bound $O(N_{nz})$ algorithm using **CSR (Compressed Sparse Row)** format.

## 📂 Project Structure

```text
.
├── Codes/
│   ├── CppCcode/       # C++ Implementation (Raw pointers, std::thread)
│   ├── JavaCode/       # Java Implementation (Native threads, JIT)
│   ├── Python code/    # Python Implementation (Pure Python)
│   └── Rust code/      # Rust Implementation (Arc, Mutex, Safety checks)
├── Data/               # Benchmarking logs, screenshots, and plots
├── Documentation/      # PDF Reports and Summaries
└── README.md