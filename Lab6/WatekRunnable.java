class WatekRunnable implements Runnable {

    private int startK, endK;
    private int id;
    private Obraz obraz;

    public WatekRunnable(int id, int startK, int endK, Obraz obraz) {
        this.startK = startK;
        this.endK = endK;
        this.obraz = obraz;
        this.id = id;
    }

    @Override
    public void run() {

        for (int k = startK; k <= endK; k++) {

            int count = obraz.getSymbolCount(k);

            obraz.addToParallelHistogram(k, count);

            obraz.print_parallel_line(k, id, count);
        }
    }
}
