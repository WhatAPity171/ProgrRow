import java.util.Scanner;

class Wariant3{
	public static void main(String[] args) {
	System.out.println("\n=== WARIANT 3 — CYKLICZNY PODZIAŁ WIERSZOWY ===");
		Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#columns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();

        Obraz obraz = new Obraz(n, m);
		obraz.clear_hist_parallel();

		int numThreads = 4;
		Thread[] t = new Thread[numThreads];

		for (int i = 0; i < numThreads; i++) {
    		t[i] = new Thread(new WatekCykliczny(i, numThreads, obraz));
    		t[i].start();
		}
		for (int i = 0; i < numThreads; i++) {
    		try { t[i].join(); }
            catch (InterruptedException e) {}
		}

		obraz.compareHistograms();

	}
}
