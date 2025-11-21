// ForkJoinMergeSort.java
import java.util.Arrays;
import java.util.Random;
import java.util.concurrent.ForkJoinPool;

public class ForkJoinMergeSort {

    public static void main(String[] args) {
        int ARRAY_SIZE = 20_000_000; // Dwadzieścia milionów elementów
        int[] data = generateRandomArray(ARRAY_SIZE);

        System.out.println("--- Sortowanie przez scalanie z ForkJoinPool (równoległe) ---");
        System.out.println("Rozmiar tablicy: " + ARRAY_SIZE);

        ForkJoinPool pool = ForkJoinPool.commonPool();

        long startTime = System.currentTimeMillis();
        DivideTask mainTask = new DivideTask(Arrays.copyOf(data, data.length));

        int[] sortedArray = pool.invoke(mainTask);

        long endTime = System.currentTimeMillis();
        long duration = endTime - startTime;

        System.out.println("Zakończono sortowanie ForkJoin.");
        System.out.println("Czas wykonania ForkJoin: " + duration + " ms");
        
        long sequentialStartTime = System.currentTimeMillis();
        int[] sequentialSortedArray = Arrays.copyOf(data, data.length); // Kopia oryginalnej tablicy
        Arrays.sort(sequentialSortedArray); // Standardowe sortowanie Javie (Dual-Pivot QuickSort)
        long sequentialEndTime = System.currentTimeMillis();
        long sequentialDuration = sequentialEndTime - sequentialStartTime;

        System.out.println("\n--- Porównanie z sortowaniem sekwencyjnym ---");
        System.out.println("Czas wykonania Arrays.sort (sekwencyjnie): " + sequentialDuration + " ms");
    }
    
    
    private static int[] generateRandomArray(int size) {
        int[] arr = new int[size];
        Random rand = new Random();
        for (int i = 0; i < size; i++) {
            arr[i] = rand.nextInt(size * 10); // Wartości od 0 do (size * 10 - 1)
        }
        return arr;
    }

    
    private static boolean isSorted(int[] arr) {
        for (int i = 0; i < arr.length - 1; i++) {
            if (arr[i] > arr[i+1]) {
                return false;
            }
        }
        return true;
    }
}