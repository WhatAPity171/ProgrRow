import java.util.Random;

class Obraz {

    private int size_n;
    private int size_m;
    private char[][] tab;

    private char[] tab_symb;      // 94 znaki ASCII: od 33 do 126
    private int[] histogram;      // wersja sekwencyjna
    private int[] hist_parallel;  // wersja równoległa

    public Obraz(int n, int m) {

        this.size_n = n;
        this.size_m = m;

        tab = new char[n][m];
        tab_symb = new char[94];

        final Random random = new Random();

        for (int k = 0; k < 94; k++) 
            tab_symb[k] = (char) (k + 33);

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                tab[i][j] = tab_symb[random.nextInt(94)];
                System.out.print(tab[i][j] + " ");
            }
            System.out.print("\n");
        }
        System.out.println();

        histogram = new int[94];
        hist_parallel = new int[94]; // równoległy

        clear_histogram();
        clear_hist_parallel();
    }

    public void clear_histogram() {
        for (int i = 0; i < 94; i++) histogram[i] = 0;
    }

    public void clear_hist_parallel() {
        for (int i = 0; i < 94; i++) hist_parallel[i] = 0;
    }

    /* -------- SEKWENCYJNE OBLICZANIE HISTOGRAMU – nie ruszamy -------- */
    public void calculate_histogram() {
        for (int i = 0; i < size_n; i++) {
            for (int j = 0; j < size_m; j++) {
                for (int k = 0; k < 94; k++) {
                    if (tab[i][j] == tab_symb[k])
                        histogram[k]++;
                }
            }
        }
    }

    public void print_histogram() {
        for (int i = 0; i < 94; i++)
            System.out.println(tab_symb[i] + " " + histogram[i]);
    }

    /* -------------------------------------------------------------------- */
    /* ----------- METODY RÓWNOLEGŁE – WARIANT 1 (wątek = jeden znak) ----- */
    /* -------------------------------------------------------------------- */

    // oblicza wystąpienia znaku o indeksie k (ASCII – 33)
    public synchronized void addToParallelHistogram(int k, int count) {
        hist_parallel[k] += count;
    }

    // bezpieczny druk (blokada)
    public synchronized void print_parallel_line(int k, int threadId, int count) {
        System.out.print("Wątek " + threadId + ":   " + tab_symb[k] + " ");
        for (int i = 0; i < count; i++) System.out.print("=");
        System.out.println();
    }

    public int getSizeN() { return size_n; }
    public int getSizeM() { return size_m; }
    public char getAt(int i, int j) { return tab[i][j]; }
    public char getSymbol(int k) { return tab_symb[k]; }

    public int getSymbolCount(int k) {
        int count = 0;
        for (int i = 0; i < size_n; i++)
            for (int j = 0; j < size_m; j++)
                if (tab[i][j] == tab_symb[k]) count++;
        return count;
    }

    public void compareHistograms() {
        System.out.println("\n--- PORÓWNANIE HISTOGRAMÓW ---");
        boolean ok = true;
        for (int i = 0; i < 94; i++) {
            if (histogram[i] != hist_parallel[i]) {
                System.out.println("BŁĄD dla znaku " + tab_symb[i] +
                        ": seq=" + histogram[i] + ", par=" + hist_parallel[i]);
                ok = false;
            }
        }
        if (ok) System.out.println("Histogram równoległy = sekwencyjny ✔");
    }

    public int getParallelValue(int k) {
        return hist_parallel[k];
    }

    //Do wariantu 3
    public int[] countRowsCyclic(int startRow, int step) {
    	int[] localHist = new int[94];

    	for (int i = startRow; i < size_n; i += step) {
        	for (int j = 0; j < size_m; j++) {
            	int idx = tab[i][j] - 33;
            	localHist[idx]++;
        	}
    	}
    	return localHist;
	}
	public synchronized void mergeLocalHistogram(int[] local) {
    	for (int i = 0; i < 94; i++)
        	hist_parallel[i] += local[i];
	}
	//Wariant 4
	public int[] countColumnsBlock(int startCol, int endCol) {
    	int[] localHist = new int[94];

    	for (int i = 0; i < size_n; i++) {
        	for (int j = startCol; j <= endCol; j++) {
            	int idx = tab[i][j] - 33;
            	localHist[idx]++;
        	}
    	}
    	return localHist;
	}
	//Wariant 5 - 2D
	public int[] countBlock2D(int startRow, int endRow, int startCol, int endCol) {
    	int[] localHist = new int[94];

    	for (int i = startRow; i <= endRow; i++) {
        	for (int j = startCol; j <= endCol; j++) {
           		int idx = tab[i][j] - 33;
        	    localHist[idx]++;
    	    }
    	}
    	return localHist;
	}


}
