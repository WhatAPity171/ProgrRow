import java.util.ArrayList;
import java.util.List;

public class RunnableIntegralCalculator {

    public static void main(String[] args) {
        double totalStart = 0;               // Całkowita dolna granica całkowania
        double totalEnd = Math.PI;           // Całkowita górna granica całkowania
        double dxForAccuracy = 0.000001;     // Parametr dx determinujący dokładność całego obliczenia

        int N_TASKS_AND_THREADS = Runtime.getRuntime().availableProcessors() * 4; 
        if (N_TASKS_AND_THREADS < 1) N_TASKS_AND_THREADS = 1; // Zapewnij minimum jeden wątek/zadanie.


        System.out.println("--- Równoległe obliczanie całki (z Runnable i jawnym tworzeniem wątków) ---");
        System.out.println("Funkcja: sin(x)");
        System.out.println("Całkowity przedział: [" + totalStart + ", " + totalEnd + "]");
        System.out.println("Parametr dokładności (dx): " + dxForAccuracy);
        System.out.println("Liczba utworzonych wątków: " + N_TASKS_AND_THREADS);
        System.out.println("------------------------------------------------------------------");


        IntegralAggregator aggregator = new IntegralAggregator(); // Utworzenie obiektu do zbierania wyników
        List<Thread> threads = new ArrayList<>(N_TASKS_AND_THREADS); // Lista do przechowywania referencji do utworzonych wątków

        double subIntervalWidth = (totalEnd - totalStart) / N_TASKS_AND_THREADS;

        long startTime = System.nanoTime();

        // 1. Jawne utworzenie obiektów Calka_runnable i wątków
        for (int i = 0; i < N_TASKS_AND_THREADS; i++) {
            double subIntervalStart = totalStart + i * subIntervalWidth;
            double subIntervalEnd = subIntervalStart + subIntervalWidth;

            // Korekta dla ostatniego podprzedziału, aby dokładnie zamknąć całkowity przedział
            if (i == N_TASKS_AND_THREADS - 1) {
                subIntervalEnd = totalEnd;
            }

            // Utworzenie obiektu Calka_runnable, przekazując mu referencję do agregatora
            Calka_runnable task = new Calka_runnable(subIntervalStart, subIntervalEnd, dxForAccuracy, aggregator);
            
            // Jawne utworzenie wątku, przekazując mu obiekt Calka_runnable
            Thread thread = new Thread(task);
            threads.add(thread); // Dodanie wątku do listy
            thread.start();      // Rozpoczęcie wykonania wątku (wywołanie metody run() Calka_runnable)
        }

        // 2. Oczekiwanie na zakończenie wszystkich wątków
        for (Thread thread : threads) {
            try {
                thread.join(); // Metoda join() powoduje, że główny wątek czeka na zakończenie danego wątku
            } catch (InterruptedException e) {
                System.err.println("Wątek główny został przerwany podczas oczekiwania na wątek roboczy: " + e.getMessage());
                Thread.currentThread().interrupt(); // Przywracamy status przerwania
            }
        }
        
        long endTime = System.nanoTime();   // Pomiar czasu zakończenia wykonania
        long duration = (endTime - startTime) / 1_000_000; // Czas w milisekundach

        double totalIntegralResult = aggregator.getTotalIntegral(); // Pobranie końcowego wyniku z agregatora

        System.out.println("------------------------------------------------------------------");
        System.out.println("Całkowita obliczona całka (równolegle - Runnable): " + totalIntegralResult);
        System.out.println("Oczekiwany wynik (dla sin(x) w [0, PI]): " + 2.0);
        System.out.println("Różnica: " + Math.abs(totalIntegralResult - 2.0));
        System.out.println("Czas wykonania równoległego (Runnable): " + duration + " ms");
        System.out.println("------------------------------------------------------------------");

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