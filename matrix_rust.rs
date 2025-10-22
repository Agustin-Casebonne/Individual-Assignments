use std::time::Instant;

fn multiply_matrices(n: usize, a: &Vec<f64>, b: &Vec<f64>, c: &mut Vec<f64>) {
    c.fill(0.0);
    for i in 0..n {
        for j in 0..n {
            let mut sum = 0.0;
            for k in 0..n {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}

// Inicialización sin rand
fn initialize_matrices(n: usize) -> (Vec<f64>, Vec<f64>, Vec<f64>) {
    let size = n * n;

    let a: Vec<f64> = (0..size).map(|i| ((i * 37 % 100) as f64) / 100.0).collect();
    let b: Vec<f64> = (0..size).map(|i| ((i * 53 % 100) as f64) / 100.0).collect();
    let c: Vec<f64> = vec![0.0; size];

    (a, b, c)
}

fn run_benchmark(n: usize, runs: usize) -> f64 {
    let (a, b, mut c) = initialize_matrices(n);
    let mut total_duration = 0.0;

    println!("Running Rust benchmark for N={} ({} runs)...", n, runs);

    for _ in 0..runs {
        let start = Instant::now();
        multiply_matrices(n, &a, &b, &mut c);
        total_duration += start.elapsed().as_secs_f64();
    }

    let average_duration = total_duration / runs as f64;
    println!("Rust N={} Average Time: {:.6} s", n, average_duration);
    average_duration
}

fn main() {
    println!("--- Rust Matrix Multiplication Benchmark ---");
    
    let runs = 3; // menos para que no tarde demasiado
    let matrix_sizes = vec![64, 128, 256]; // tamaños menores para entorno online

    for n in matrix_sizes {
        run_benchmark(n, runs);
        println!("---------------------------------------------------------");
    }
}
