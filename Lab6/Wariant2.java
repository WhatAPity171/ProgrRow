import java.util.Scanner;

class Wariant2{
	public static void main(String[] args) {
	System.out.println("\n=== WARIANT 2 - Runnable===");
		Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#columns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        Obraz obraz = new Obraz(n, m);

        obraz.calculate_histogram();

        int numThreads = 3;     // np. trzy bloki znaków
        int block = 94 / numThreads;

        Thread[] threads = new Thread[numThreads];

        int start = 0;

        for (int i = 0; i < numThreads; i++) {

            int end = (i == numThreads - 1) ? 93 : (start + block - 1);

            threads[i] = new Thread(new WatekRunnable(i + 1, start, end, obraz));
            threads[i].start();

            start = end + 1;
        }

        for (int i = 0; i < numThreads; i++) {
            try { threads[i].join(); }
            catch (InterruptedException e) {}
        }

        obraz.compareHistograms();
    }
}

