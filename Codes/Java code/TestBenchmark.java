public class TestBenchmark {

    public static void main(String[] args) {
        final int RUNS = 5;
        final int[] MATRIX_SIZES = {512, 1024, 1536};

        System.out.println("--- Java Matrix Multiplication Benchmark (Standard IJK) ---");

        for (int N : MATRIX_SIZES) {
            MatrixBenchmark.runBenchmark(N, RUNS);
            System.out.println("---------------------------------------------------------");
        }
    }
}
