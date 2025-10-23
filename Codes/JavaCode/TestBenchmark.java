import java.lang.management.ManagementFactory;
import com.sun.management.OperatingSystemMXBean;

public class TestBenchmark {

    public static void main(String[] args) {
        final int RUNS = 5;
        final int[] MATRIX_SIZES = {512, 1024, 1536};

        System.out.println("--- Java Matrix Multiplication Benchmark (Standard IJK) ---");

        OperatingSystemMXBean osBean =
                (OperatingSystemMXBean) ManagementFactory.getOperatingSystemMXBean();

        for (int N : MATRIX_SIZES) {
            double avgTime = MatrixBenchmark.runBenchmark(N, RUNS);

            // CPU load (en porcentaje)
            double cpuLoad = osBean.getProcessCpuLoad() * 100.0;

            // Memoria usada por el proceso (en MB)
            long memUsed = (Runtime.getRuntime().totalMemory()
                    - Runtime.getRuntime().freeMemory()) / (1024 * 1024);

            System.out.printf("→ CPU usage: %.1f%% | Memory: %d MB%n", cpuLoad, memUsed);
            System.out.printf("→ Average time: %.6f s%n", avgTime);
            System.out.println("---------------------------------------------------------");
        }
    }
}
