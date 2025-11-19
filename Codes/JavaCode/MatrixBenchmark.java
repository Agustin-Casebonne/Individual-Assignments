import java.util.*;
import java.util.stream.IntStream;

public class MatrixBenchmark {

 
    private static class SparseMatrixCSR {
        double[] values;      
        int[] colIndex;     
        int[] rowPtr;         
        int n;             
    }

    private static SparseMatrixCSR generateSparseMatrix(int N, double sparsity) {
        Random rand = new Random();

        ArrayList<Double> values = new ArrayList<>();
        ArrayList<Integer> colIndex = new ArrayList<>();
        int[] rowPtr = new int[N + 1];
        rowPtr[0] = 0;

        for (int i = 0; i < N; i++) {
            int count = 0;
            for (int j = 0; j < N; j++) {
                if (rand.nextDouble() > sparsity) {
                    values.add(rand.nextDouble());
                    colIndex.add(j);
                    count++;
                }
            }
            rowPtr[i + 1] = rowPtr[i] + count;
        }

        SparseMatrixCSR A = new SparseMatrixCSR();
        A.n = N;
        A.values = values.stream().mapToDouble(x -> x).toArray();
        A.colIndex = colIndex.stream().mapToInt(x -> x).toArray();
        A.rowPtr = rowPtr;

        return A;
    }

    private static void spmvSequential(SparseMatrixCSR A, double[] x, double[] y) {
        Arrays.fill(y, 0.0);

        for (int i = 0; i < A.n; i++) {
            double sum = 0.0;
            for (int idx = A.rowPtr[i]; idx < A.rowPtr[i + 1]; idx++) {
                sum += A.values[idx] * x[A.colIndex[idx]];
            }
            y[i] = sum;
        }
    }

    private static void spmvParallel(SparseMatrixCSR A, double[] x, double[] y) {
        IntStream.range(0, A.n).parallel().forEach(i -> {
            double sum = 0.0;
            for (int idx = A.rowPtr[i]; idx < A.rowPtr[i + 1]; idx++) {
                sum += A.values[idx] * x[A.colIndex[idx]];
            }
            y[i] = sum;
        });
    }

    public static double runBenchmark(int N, int RUNS) {

        final double SPARSITY = 0.95;

        SparseMatrixCSR A = generateSparseMatrix(N, SPARSITY);
        Random rand = new Random();

        double[] x = new double[N];
        double[] y = new double[N];
        double[] yPar = new double[N];

        for (int i = 0; i < N; i++)
            x[i] = rand.nextDouble();

        System.out.printf("Running Java Sparse SpMV for N=%d (%d runs)...%n", N, RUNS);

        long totalSeq = 0;
        long totalPar = 0;

        spmvSequential(A, x, y);

        for (int r = 0; r < RUNS; r++) {

            long start = System.nanoTime();
            spmvSequential(A, x, y);
            long end = System.nanoTime();
            totalSeq += (end - start);

            start = System.nanoTime();
            spmvParallel(A, x, yPar);
            end = System.nanoTime();
            totalPar += (end - start);
        }

        double seqTime = (totalSeq / (double) RUNS) / 1e9;
        double parTime = (totalPar / (double) RUNS) / 1e9;

        System.out.printf("Sequential: %.6f s%n", seqTime);
        System.out.printf("Parallel:   %.6f s%n", parTime);
        System.out.printf("Speedup: %.2fx%n", seqTime / parTime);

        return seqTime;
    }
}
