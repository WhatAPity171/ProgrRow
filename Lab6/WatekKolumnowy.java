class WatekKolumnowy implements Runnable {

    private int id;
    private int startCol, endCol;
    private Obraz obraz;

    public WatekKolumnowy(int id, int startCol, int endCol, Obraz obraz) {
        this.id = id;
        this.startCol = startCol;
        this.endCol = endCol;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        int[] local = obraz.countColumnsBlock(startCol, endCol);
        obraz.mergeLocalHistogram(local);

        synchronized (obraz) {
            System.out.println("Wątek " + id + " (kolumny " + startCol + "-" + endCol + "):");
            for (int i = 0; i < 94; i++) {
                char c = (char)(i + 33);
                System.out.print(c + " ");
                for (int k = 0; k < local[i]; k++) System.out.print("=");
                System.out.println();
            }
        }
    }
}
