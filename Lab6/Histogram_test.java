import java.util.Scanner;

class Histogram_test {
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
        System.out.println("\n=== WARIANT 3 — CYKLICZNY PODZIAŁ WIERSZOWY ===");

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
		System.out.println("\n=== WARIANT 4 — BLOKOWY PODZIAŁ KOLUMNOWY ===");

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
		//Wariant 5
		System.out.println("\n=== WARIANT 5 — 2D PODZIAŁ BLOKOWY ===");

		obraz.clear_hist_parallel();

		int rows = obraz.getSizeN();
		int cols = obraz.getSizeM();

		int blocksR = 2;
		int blocksC = 2;

		Thread[] threads = new Thread[blocksR * blocksC];
		int id = 0;

		int rowBlock = rows / blocksR;
		int colBlock = cols / blocksC;

		for (int br = 0; br < blocksR; br++) {
    		for (int bc = 0; bc < blocksC; bc++) {

        		int r1 = br * rowBlock;
        		int r2 = (br == blocksR - 1 ? rows - 1 : r1 + rowBlock - 1);

        		int c1 = bc * colBlock;
        		int c2 = (bc == blocksC - 1 ? cols - 1 : c1 + colBlock - 1);

        		threads[id] = new Thread(new Watek2DBlokowy(id, r1, r2, c1, c2, obraz));
        		threads[id].start();
        		id++;
    		}
		}

		for (int i = 0; i < threads.length; i++){
			try { threads[i].join(); }
            catch (InterruptedException e) {}
        }

		obraz.compareHistograms();


    }
}
