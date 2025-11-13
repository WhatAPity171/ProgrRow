class Watek extends Thread {

    private int symbolIndex; // k
    private Obraz obraz;
    private int threadId;

    public Watek(int id, int symbolIndex, Obraz obraz) {
        this.symbolIndex = symbolIndex;
        this.obraz = obraz;
        this.threadId = id;
    }

    @Override
    public void run() {
        int count = obraz.getSymbolCount(symbolIndex);

        obraz.addToParallelHistogram(symbolIndex, count);

        obraz.print_parallel_line(symbolIndex, threadId, count);
    }
}
