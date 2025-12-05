import java.util.Random;
import java.lang.management.ManagementFactory;
import java.lang.management.MemoryMXBean;

public class Main {

    // Multiplicación secuencial SpMV
    public static double[] spmvSequential(CSRMatrix A, double[] x) {
        int N = A.n;
        double[] y = new double[N];
        for (int i = 0; i < N; i++) {
            double sum = 0.0;
            for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++) {
                sum += A.values[k] * x[A.colIndex[k]];
            }
            y[i] = sum;
        }
        return y;
    }

    // Multiplicación paralela SpMV
    public static double[] spmvParallel(CSRMatrix A, double[] x) {
        int N = A.n;
        double[] y = new double[N];
        java.util.stream.IntStream.range(0, N).parallel().forEach(i -> {
            double sum = 0.0;
            for (int k = A.rowPtr[i]; k < A.rowPtr[i + 1]; k++) {
                sum += A.values[k] * x[A.colIndex[k]];
            }
            y[i] = sum;
        });
        return y;
    }

    // Benchmark
    public static void runBenchmark(int N, int runs, double density) {
        CSRMatrix A = CSRMatrix.generateSparseMatrix(N, density);
        double[] x = new double[N];
        Random rand = new Random();
        for (int i = 0; i < N; i++) x[i] = rand.nextDouble();

        MemoryMXBean memoryBean = ManagementFactory.getMemoryMXBean();
        long memBefore = memoryBean.getHeapMemoryUsage().getUsed();

        // Secuencial
        long t0 = System.nanoTime();
        double[] ySeq = null;
        for (int i = 0; i < runs; i++) ySeq = spmvSequential(A, x);
        long t1 = System.nanoTime();
        double avgSeq = (t1 - t0) / 1e9 / runs;

        // Paralelo
        t0 = System.nanoTime();
        double[] yPar = null;
        for (int i = 0; i < runs; i++) yPar = spmvParallel(A, x);
        t1 = System.nanoTime();
        double avgPar = (t1 - t0) / 1e9 / runs;

        double speedup = avgSeq / avgPar;
        double efficiency = speedup / Runtime.getRuntime().availableProcessors();
        long memAfter = memoryBean.getHeapMemoryUsage().getUsed();
        double memUsage = (memAfter - memBefore) / (1024.0 * 1024.0) + A.memoryUsageMB();

        System.out.printf("N=%d: Sequential %.6fs | Parallel %.6fs | Speedup %.2fx | Efficiency %.2f%n",
                          N, avgSeq, avgPar, speedup, efficiency);
        System.out.printf("Memory usage: %.2f MB%n", memUsage);
        System.out.println("---------------------------------------------------------");
    }

    public static void main(String[] args) {
        int[] sizes = {512, 1024, 1536, 2048};
        int runs = 5;
        double density = 0.1;

        System.out.println("--- Java Sparse Matrix-Vector Benchmark (SpMV CSR) ---");
        for (int n : sizes) runBenchmark(n, runs, density);
    }
}
