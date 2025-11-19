use std::time::Instant;
use std::thread;
use std::sync::{Arc, Mutex};
use std::mem::size_of;


struct CsrMatrix {
    values: Vec<f64>,
    col_indices: Vec<usize>,
    row_ptr: Vec<usize>,
    num_rows: usize,
    #[allow(dead_code)] 
    num_cols: usize,
}

impl CsrMatrix {
    fn new_random(rows: usize, cols: usize, density: f64) -> Self {
        let mut values = Vec::new();
        let mut col_indices = Vec::new();
        let mut row_ptr = vec![0; rows + 1];
        let mut count = 0;

        for i in 0..rows {
            row_ptr[i] = count;
            for j in 0..cols {
                if (i * j + j) % 100 < (density * 100.0) as usize {
                    values.push(1.0);
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
        let row_start = matrix.row_ptr[i];
        let row_end = matrix.row_ptr[i + 1];
        let mut sum = y[i];
        for k in row_start..row_end {
            sum += matrix.values[k] * x[matrix.col_indices[k]];
        }
        y[i] = sum;
    }
}

fn spmv_csr_parallel(matrix: &Arc<CsrMatrix>, x: &Arc<Vec<f64>>, y: &mut Vec<f64>, num_threads: usize) {
    let rows_per_thread = matrix.num_rows / num_threads;
    let mut handles = vec![];
    let result = Arc::new(Mutex::new(y.clone()));

    for t in 0..num_threads {
        let matrix_ref = Arc::clone(matrix);
        let x_ref = Arc::clone(x);
        let result_ref = Arc::clone(&result);
        
        let start_row = t * rows_per_thread;
        let end_row = if t == num_threads - 1 { matrix.num_rows } else { (t + 1) * rows_per_thread };

        handles.push(thread::spawn(move || {
            let mut local_y_updates = vec![0.0; end_row - start_row];
            for i in start_row..end_row {
                let r_start = matrix_ref.row_ptr[i];
                let r_end = matrix_ref.row_ptr[i+1];
                let mut sum = 0.0;
                for k in r_start..r_end {
                    sum += matrix_ref.values[k] * x_ref[matrix_ref.col_indices[k]];
                }
                local_y_updates[i - start_row] = sum;
            }
            let mut r_lock = result_ref.lock().unwrap();
            for (offset, val) in local_y_updates.iter().enumerate() {
                r_lock[start_row + offset] += val;
            }
        }));
    }
    for handle in handles {
        handle.join().unwrap();
    }
    *y = result.lock().unwrap().clone();
}



fn run_benchmark(n: usize, runs: usize) {
    // 1. Preparar datos
    let matrix = Arc::new(CsrMatrix::new_random(n, n, 0.05)); 
    let x = Arc::new(vec![1.0; n]);
    let mut y = vec![0.0; n];

    // Calcular memoria total (Matriz + Vectores X e Y)
    let vec_size_mb = (n * size_of::<f64>()) as f64 / 1024.0 / 1024.0;
    let total_mem_mb = matrix.memory_usage_mb() + (vec_size_mb * 2.0);

    println!("Running Rust Sparse SpMV for N={} ({} runs)...", n, runs);

    // 2. Ejecutar Secuencial
    let mut total_time_seq = 0.0;
    for _ in 0..runs {
        let start = Instant::now();
        spmv_csr(&matrix, &x, &mut y);
        total_time_seq += start.elapsed().as_secs_f64();
    }
    let avg_seq = total_time_seq / runs as f64;

    // 3. Ejecutar Paralelo
    let mut total_time_par = 0.0;
    let threads = 4;
    for _ in 0..runs {
        let mut y_par = vec![0.0; n]; 
        let start = Instant::now();
        spmv_csr_parallel(&matrix, &x, &mut y_par, threads);
        total_time_par += start.elapsed().as_secs_f64();
    }
    let avg_par = total_time_par / runs as f64;


    let speedup = avg_seq / avg_par;
    
    let s_seq = format!("{:.6}", avg_seq).replace(".", ",");
    let s_par = format!("{:.6}", avg_par).replace(".", ",");
    let s_spd = format!("{:.2}", speedup).replace(".", ",");
    
    println!("Sequential: {} s", s_seq);
    println!("Parallel:   {} s", s_par);
    println!("Speedup: {}x", s_spd);
    

    println!("? CPU usage: N/A | Memory: {:.0} MB", total_mem_mb);
    println!("? Average time: {} s", s_seq);
    println!("---------------------------------------------------------");
}

fn main() {
    println!("--- Rust Sparse Matrix-Vector Benchmark (SpMV - CSR) ---");
    
    let runs = 5;
    let sizes = vec![512, 1024, 1536];

    for n in sizes {
        run_benchmark(n, runs);
    }
}