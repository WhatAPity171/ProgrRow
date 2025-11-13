import java.util.Scanner;

class Wariant1 {
    public static void main(String[] args) {

        Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#columns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        Obraz obraz = new Obraz(n, m);

        System.out.println("=== SEKWENCYJNE LICZENIE HISTOGRAMU ===");
        obraz.calculate_histogram();
        obraz.print_histogram();

        System.out.println("\n=== RÓWNOLEGŁE LICZENIE (wariant 1) ===");

        Watek[] watki = new Watek[94];

        for (int k = 0; k < 94; k++) {
            watki[k] = new Watek(k + 1, k, obraz);
            watki[k].start();
        }

        for (int k = 0; k < 94; k++) {
            try { watki[k].join(); }
            catch (InterruptedException e) {}
        }

        System.out.println("\n=== PORÓWNANIE ===");
        obraz.compareHistograms();
    }
}