import java.util.ArrayList;
import java.util.Random;

public class CSRMatrix {
    public int n;
    public double[] values;
    public int[] colIndex;
    public int[] rowPtr;

    // Generar matriz dispersa CSR
    public static CSRMatrix generateSparseMatrix(int n, double density) {
        ArrayList<Double> valuesList = new ArrayList<>();
        ArrayList<Integer> colList = new ArrayList<>();
        int[] rowPtr = new int[n + 1];
        rowPtr[0] = 0;

        Random rand = new Random();
        for (int i = 0; i < n; i++) {
            int count = 0;
            for (int j = 0; j < n; j++) {
                if (rand.nextDouble() > density) {
                    valuesList.add(rand.nextDouble());
                    colList.add(j);
                    count++;
                }
            }
            rowPtr[i + 1] = rowPtr[i] + count;
        }

        double[] values = new double[valuesList.size()];
        int[] colIndex = new int[colList.size()];
        for (int i = 0; i < values.length; i++) values[i] = valuesList.get(i);
        for (int i = 0; i < colIndex.length; i++) colIndex[i] = colList.get(i);

        CSRMatrix mat = new CSRMatrix();
        mat.n = n;
        mat.values = values;
        mat.colIndex = colIndex;
        mat.rowPtr = rowPtr;
        return mat;
    }

    // Estimación de memoria (aprox.)
    public double memoryUsageMB() {
        double mem = values.length * Double.BYTES
                   + colIndex.length * Integer.BYTES
                   + rowPtr.length * Integer.BYTES;
        return mem / (1024.0 * 1024.0);
    }
}
