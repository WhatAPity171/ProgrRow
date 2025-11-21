import java.util.Scanner;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

class Wariant2_ExecutorPool {
    public static void main(String[] args) {
        System.out.println("\n=== WARIANT 2 - ExecutorService Pool ===");
        Scanner scanner = new Scanner(System.in);

        System.out.println("Podaj rozmiar obrazu: n (wiersze), m (kolumny)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        
        System.out.println("Podaj liczbę zadań (ile bloków symboli do przetworzenia):");
        int N_TASKS = scanner.nextInt(); // Liczba zadań

        // Liczba wątków w puli. Dobrym punktem wyjścia jest liczba dostępnych rdzeni procesora.
        int N_THREADS = Runtime.getRuntime().availableProcessors();
        if (N_THREADS == 0) N_THREADS = 1; // Zapobiegnięcie 0 wątków w niektórych środowiskach.

        System.out.println("Pula wątków będzie miała " + N_THREADS + " wątków.");

        Obraz obraz = new Obraz(n, m);

        // Obliczanie histogramu sekwencyjnie dla porównania
        obraz.calculate_histogram();
        
        // Czyszczenie histogramu równoległego przed nowym obliczeniem
        obraz.clear_hist_parallel();

        // Tworzenie puli wątków
        ExecutorService executor = Executors.newFixedThreadPool(N_THREADS);

        // Dzielenie zakresu 94 symboli na N_TASKS bloków
        int totalSymbols = 94;
        int symbolsPerTask = totalSymbols / N_TASKS;
        int startSymbol = 0;

        System.out.println("\nRozsyłam zadania do wykonania...");
        long startTime = System.currentTimeMillis();

        for (int i = 0; i < N_TASKS; i++) {
            int endSymbol;
            // Ostatnie zadanie bierze wszystkie pozostałe symbole
            if (i == N_TASKS - 1) {
                endSymbol = totalSymbols - 1;
            } else {
                endSymbol = startSymbol + symbolsPerTask - 1;
            }

            // Tworzymy obiekt WatekRunnable i przekazujemy go do puli
            Runnable worker = new WatekRunnable(i + 1, startSymbol, endSymbol, obraz);
            executor.execute(worker); // execute() przyjmuje Runnable
            
            startSymbol = endSymbol + 1;
        }

        
        executor.shutdown();

        // Czekamy na zakończenie wszystkich zadań w puli
        try {
            executor.awaitTermination(Long.MAX_VALUE, TimeUnit.NANOSECONDS);
        } catch (InterruptedException e) {
            System.err.println("Oczekiwanie na zakończenie wątków zostało przerwane: " + e.getMessage());
            Thread.currentThread().interrupt(); // Przywróć status przerwania
        }

        long endTime = System.currentTimeMillis();
        long duration = endTime - startTime;

        System.out.println("\nZakończono wszystkie zadania w puli ExecutorService.");
        System.out.println("Czas wykonania równoległego z pulą wątków: " + duration + " ms");

        obraz.compareHistograms();
        scanner.close();
    }
}