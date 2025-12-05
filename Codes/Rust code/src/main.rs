use rand::Rng;
use rayon::prelude::*;
use std::time::Instant;
use std::sync::{Arc, Mutex};
use std::mem::size_of;

struct CsrMatrix {
    values: Vec<f64>,
    col_indices: Vec<usize>,
    row_ptr: Vec<usize>,
    num_rows: usize,
    num_cols: usize,
}

impl CsrMatrix {
    fn new_random(rows: usize, cols: usize, density: f64) -> Self {
        let mut values = Vec::new();
        let mut col_indices = Vec::new();
        let mut row_ptr = vec![0; rows + 1];
        let mut count = 0;

        let mut rng = rand::thread_rng();
        for i in 0..rows {
            row_ptr[i] = count;
            for j in 0..cols {
                if rng.r#gen::<f64>() > density {
                    values.push(rng.r#gen::<f64>());
                    col_indices.push(j);
                    count += 1;
                }
            }
        }
        row_ptr[rows] = count;

        CsrMatrix {
            values,
            col_indices,
            row_ptr,
            num_rows: rows,
            num_cols: cols,
        }
    }

    fn memory_usage_mb(&self) -> f64 {
        let size_f64 = size_of::<f64>();
        let size_usize = size_of::<usize>();
        let total_bytes = (self.values.len() * size_f64)
            + (self.col_indices.len() * size_usize)
            + (self.row_ptr.len() * size_usize);
        total_bytes as f64 / 1024.0 / 1024.0
    }
}

fn spmv_csr(matrix: &CsrMatrix, x: &Vec<f64>, y: &mut Vec<f64>) {
    for i in 0..matrix.num_rows {
        let mut sum = 0.0;
        for k in matrix.row_ptr[i]..matrix.row_ptr[i + 1] {
            sum += matrix.values[k] * x[matrix.col_indices[k]];
        }
        y[i] = sum;
    }
}

fn spmv_csr_parallel(matrix: &Arc<CsrMatrix>, x: &Arc<Vec<f64>>, y: &mut Vec<f64>) {
    let result = Arc::new(Mutex::new(y.clone()));
    (0..matrix.num_rows).into_par_iter().for_each(|i| {
        let mut sum = 0.0;
        for k in matrix.row_ptr[i]..matrix.row_ptr[i + 1] {
            sum += matrix.values[k] * x[matrix.col_indices[k]];
        }
        let mut y_lock = result.lock().unwrap();
        y_lock[i] = sum;
    });
    *y = result.lock().unwrap().clone();
}

fn run_benchmark(n: usize, runs: usize) {
    let matrix = Arc::new(CsrMatrix::new_random(n, n, 0.95));
    let x = Arc::new(vec![1.0; n]);
    let mut y_seq = vec![0.0; n];
    let mut y_par = vec![0.0; n];

    // Corregido el cálculo de memoria
    let total_mem_mb = matrix.memory_usage_mb() + (2.0 * n as f64 * size_of::<f64>() as f64 / 1024.0 / 1024.0);

    let mut total_seq = 0.0;
    let mut total_par = 0.0;

    for _ in 0..runs {
        let start = Instant::now();
        spmv_csr(&matrix, &x, &mut y_seq);
        total_seq += start.elapsed().as_secs_f64();

        let start = Instant::now();
        spmv_csr_parallel(&matrix, &x, &mut y_par);
        total_par += start.elapsed().as_secs_f64();
    }

    let avg_seq = total_seq / runs as f64;
    let avg_par = total_par / runs as f64;
    let speedup = avg_seq / avg_par;
    let num_threads = rayon::current_num_threads() as f64;
    let efficiency = speedup / num_threads;

    println!(
        "N={}: Sequential {:.6}s | Parallel {:.6}s | Speedup {:.2}x | Efficiency {:.2}",
        n, avg_seq, avg_par, speedup, efficiency
    );
    println!("Memory usage: {:.2} MB", total_mem_mb);
    println!("---------------------------------------------------------");
}

fn main() {
    println!("--- Rust Sparse Matrix-Vector Benchmark (SpMV CSR) ---");

    let runs = 5;
    let sizes = vec![512, 1024, 1536, 2048];

    for n in sizes {
        run_benchmark(n, runs);
    }
}
