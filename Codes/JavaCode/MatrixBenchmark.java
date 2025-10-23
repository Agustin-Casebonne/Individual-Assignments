import java.util.Random;
import java.util.Arrays;

public class MatrixBenchmark {

    // Clase interna que realiza la multiplicación
    private static class MatrixMultiplier {
        public static void multiply(int N, double[][] A, double[][] B, double[][] C) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < N; j++) {
                    double sum = 0.0;
                    for (int k = 0; k < N; k++) {
                        sum += A[i][k] * B[k][j];
                    }
                    C[i][j] = sum;
                }
            }
        }
    }

    // Inicializa las matrices con números aleatorios
    public static double[][][] initializeMatrices(int N) {
        Random random = new Random();
        double[][] A = new double[N][N];
        double[][] B = new double[N][N];
        double[][] C = new double[N][N];

        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A[i][j] = random.nextDouble();
                B[i][j] = random.nextDouble();
            }
        }
        return new double[][][]{A, B, C};
    }

    // Ejecuta el benchmark
    public static double runBenchmark(int N, int RUNS) {
        double[][][] matrices = initializeMatrices(N);
        double[][] A = matrices[0];
        double[][] B = matrices[1];
        double[][] C = matrices[2]; 

        long totalDurationNs = 0;

        System.out.printf("Running Java benchmark for N=%d (%d runs)...%n", N, RUNS);

        // Calentamiento (para estabilizar JIT)
        for (int r = 0; r < Math.min(RUNS, 3); r++) {
            MatrixMultiplier.multiply(N, A, B, C);
        }

        // Benchmark real
        for (int r = 0; r < RUNS; r++) {
            for (int i = 0; i < N; i++) {
                Arrays.fill(C[i], 0.0);
            }

            long start = System.nanoTime();
            MatrixMultiplier.multiply(N, A, B, C);
            long end = System.nanoTime();

            totalDurationNs += (end - start);
            System.out.printf("Run %d: %.3f s%n", r + 1, (end - start) / 1_000_000_000.0);
        }

        double averageDurationS = (double) totalDurationNs / RUNS / 1_000_000_000.0;
        System.out.printf("Java N=%d Average Time: %.6f s%n", N, averageDurationS);
        return averageDurationS;
    }
}
