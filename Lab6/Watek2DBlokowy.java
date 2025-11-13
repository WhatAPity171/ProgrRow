class Watek2DBlokowy implements Runnable {

    private int id;
    private int r1, r2, c1, c2;
    private Obraz obraz;

    public Watek2DBlokowy(int id, int r1, int r2, int c1, int c2, Obraz obraz) {
        this.id = id;
        this.r1 = r1;
        this.r2 = r2;
        this.c1 = c1;
        this.c2 = c2;
        this.obraz = obraz;
    }

    @Override
    public void run() {
        int[] local = obraz.countBlock2D(r1, r2, c1, c2);
        obraz.mergeLocalHistogram(local);

        synchronized (obraz) {
            System.out.println("Wątek " + id + " blok: (" + r1 + "-" + r2 + ", " + c1 + "-" + c2 + "):");

            for (int i = 0; i < 94; i++) {
                char c = (char)(i + 33);
                System.out.print(c + " ");
                for (int k = 0; k < local[i]; k++) System.out.print("=");
                System.out.println();
            }
        }
    }
}
