class WatekCykliczny implements Runnable {

    private int id;
    private int numThreads;
    private Obraz obraz;

    public WatekCykliczny(int id, int numThreads, Obraz obraz) {
        this.id = id;
        this.numThreads = numThreads;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        int[] local = obraz.countRowsCyclic(id, numThreads);
        obraz.mergeLocalHistogram(local);

        // druk bez przerywania
        synchronized (obraz) {
            System.out.println("Wątek " + id + " (cykliczne wiersze):");
            for (int i = 0; i < 94; i++) {
                char c = (char)(i + 33);
                System.out.print(c + " ");
                for (int k = 0; k < local[i]; k++) System.out.print("=");
                System.out.println();
            }
        }
    }
}
