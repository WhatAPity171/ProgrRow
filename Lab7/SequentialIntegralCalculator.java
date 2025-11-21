// SequentialIntegralCalculator.java
public class SequentialIntegralCalculator {
    public static void main(String[] args) {
        double start = 0;               // Dolna granica całkowania
        double end = Math.PI;           // Górna granica całkowania
        double dx = 0.000001;           // Zadana dokładność (szerokość pojedynczego trapezu)

        System.out.println("--- Sekwencyjne obliczanie całki ---");
        System.out.println("Funkcja: sin(x)");
        System.out.println("Przedział: [" + start + ", " + end + "]");
        System.out.println("Parametr dokładności (dx): " + dx);
        System.out.println("-------------------------------------");

        long startTime = System.nanoTime(); // Pomiar czasu rozpoczęcia
        
        // Utworzenie obiektu Calka_callable dla całego przedziału
        Calka_callable integralTask = new Calka_callable(start, end, dx);
        
        // Wywołanie metody compute_integral() do obliczenia całki
        double result = integralTask.compute_integral();
        
        long endTime = System.nanoTime();   // Pomiar czasu zakończenia
        long duration = (endTime - startTime) / 1_000_000; // Czas w milisekundach

        System.out.println("Obliczona całka: " + result);
        System.out.println("Oczekiwany wynik (dla sin(x) w [0, PI]): " + 2.0);
        System.out.println("Różnica: " + Math.abs(result - 2.0));
        System.out.println("Czas wykonania sekwencyjnego: " + duration + " ms");
        System.out.println("-------------------------------------");
    }
}