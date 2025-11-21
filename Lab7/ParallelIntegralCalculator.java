// ParallelIntegralCalculator.java
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.*; // Import wszystkich klas z pakietu concurrent
import java.util.Scanner;
import java.util.Locale;

public class ParallelIntegralCalculator {

    public static void main(String[] args) {
        double totalStart = 0;               // Całkowita dolna granica
        double totalEnd = Math.PI;           // Całkowita górna granica
        //double dxForAccuracy = 0.000001;     // Parametr dx determinujący dokładność całego obliczenia
        Scanner sc = new Scanner(System.in).useLocale(Locale.US);//Locale żeby przyjmował kropkę a nie przecinek
        double dxForAccuracy;
        System.out.println("Podaj dokladnosc dx");
        dxForAccuracy = sc.nextDouble();

        // Integer N_THREADS = 4;
        int N_THREADS = Runtime.getRuntime().availableProcessors(); 

        int N_TASKS = N_THREADS * 8;

        System.out.println("--- Równoległe obliczanie całki ---");
        System.out.println("Funkcja: sin(x)");
        System.out.println("Całkowity przedział: [" + totalStart + ", " + totalEnd + "]");
        System.out.println("Parametr dokładności (dx): " + dxForAccuracy);
        System.out.println("Liczba wątków w puli (N_THREADS): " + N_THREADS);
        System.out.println("Liczba zadań (N_TASKS): " + N_TASKS);
        System.out.println("-------------------------------------");

        // Tworzenie puli wątków o stałej liczbie wątków
        ExecutorService executor = Executors.newFixedThreadPool(N_THREADS);
        List<Future<Double>> futures = new ArrayList<>();

        // Obliczenie szerokości pojedynczego podprzedziału dla każdego zadania
        double subIntervalWidth = (totalEnd - totalStart) / N_TASKS;

        long startTime = System.nanoTime(); // Pomiar czasu rozpoczęcia

        // d) Zadania tworzone i przekazywane do wykonania w jednej pętli
        for (int i = 0; i < N_TASKS; i++) {
            double subIntervalStart = totalStart + i * subIntervalWidth;
            double subIntervalEnd = subIntervalStart + subIntervalWidth;

            // Zapewnienie, że ostatni podprzedział dokładnie kończy się na totalEnd,
            // aby uniknąć błędów zmiennoprzecinkowych
            if (i == N_TASKS - 1) {
                subIntervalEnd = totalEnd;
            }

            // dxForAccuracy jest niezależne od szerokości podprzedziału zadań
            Calka_callable task = new Calka_callable(subIntervalStart, subIntervalEnd, dxForAccuracy);
            
            futures.add(executor.submit(task));
        }

        // Zamknięcie executora. Nie przyjmuje nowych zadań, ale czeka na zakończenie bieżących.
        executor.shutdown();

        double totalIntegralResult = 0;

        // e) Wyniki odbierane w kolejnej pętli, aby umożliwić działanie równoległe
        for (Future<Double> future : futures) {
            try {
                totalIntegralResult += future.get(); // Blokuje do momentu, aż wynik będzie dostępny
            } catch (InterruptedException | ExecutionException e) {
                System.err.println("Błąd podczas pobierania wyniku częściowej całki: " + e.getMessage());
                e.printStackTrace();
            }
        }
        
        try {//w teorii future.get gwarantuje czekanie na zakonczenie wszystkich watkow ale to jest dodatkowe zabezpieczenie
            executor.awaitTermination(Long.MAX_VALUE, TimeUnit.NANOSECONDS);
        } catch (InterruptedException e) {
            System.err.println("Przerwanie oczekiwania na zakończenie executora: " + e.getMessage());
        }

        long endTime = System.nanoTime();   // Pomiar czasu zakończenia
        long duration = (endTime - startTime) / 1_000_000; // Czas w milisekundach

        System.out.println("-------------------------------------");
        System.out.println("Całkowita obliczona całka (równolegle): " + totalIntegralResult);
        System.out.println("Oczekiwany wynik (dla sin(x) w [0, PI]): " + 2.0);
        System.out.println("Różnica: " + Math.abs(totalIntegralResult - 2.0));
        System.out.println("Czas wykonania równoległego: " + duration + " ms");
        System.out.println("-------------------------------------");

        // Dla porównania, uruchomienie wersji sekwencyjnej i pomiar czasu
        System.out.println("\n--- Sekwencyjne obliczanie całki (do porównania) ---");
        long seqStartTime = System.nanoTime();
        Calka_callable sequentialTask = new Calka_callable(totalStart, totalEnd, dxForAccuracy);
        double sequentialResult = sequentialTask.compute_integral();
        long seqEndTime = System.nanoTime();
        long seqDuration = (seqEndTime - seqStartTime) / 1_000_000; // milliseconds
        System.out.println("Sekwencyjnie obliczona całka: " + sequentialResult);
        System.out.println("Różnica: " + Math.abs(sequentialResult - 2.0));
        System.out.println("Czas wykonania sekwencyjnego: " + seqDuration + " ms");
        System.out.println("------------------------------------------------------");
    }
}