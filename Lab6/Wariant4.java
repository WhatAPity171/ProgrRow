import java.util.Scanner;

class Wariant4{
	public static void main(String[] args) {
	System.out.println("\n=== WARIANT 4 — BLOKOWY PODZIAŁ KOLUMNOWY ===");
		Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#columns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        Obraz obraz = new Obraz(n, m);
		obraz.clear_hist_parallel();

		numThreads = 4;
		t = new Thread[numThreads];

		int block = m / numThreads;
		int start = 0;

		for (int i = 0; i < numThreads; i++) {
    		int end = (i == numThreads - 1 ? m - 1 : start + block - 1);

    		t[i] = new Thread(new WatekKolumnowy(i, start, end, obraz));
    		t[i].start();

    		start = end + 1;
		}

		for (int i = 0; i < numThreads; i++){
			try { t[i].join(); }
            catch (InterruptedException e) {}
		}

		obraz.compareHistograms();

	}
}
