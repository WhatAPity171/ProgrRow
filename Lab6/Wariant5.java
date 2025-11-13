import java.util.Scanner;

class Wariant5{
	public static void main(String[] args) {
	System.out.println("\n=== WARIANT 5 — 2D PODZIAŁ BLOKOWY ===");
		Scanner scanner = new Scanner(System.in);

        System.out.println("Set image size: n (#rows), m(#columns)");
        int n = scanner.nextInt();
        int m = scanner.nextInt();
        Obraz obraz = new Obraz(n, m);


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
