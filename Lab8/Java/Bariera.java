public class Bariera {
    private final int totalThreads;
    private int currentWaitCount;
    private int generation;

    public Bariera(int totalThreads) {
        this.totalThreads = totalThreads;
        this.currentWaitCount = 0;
        this.generation = 0;
        System.out.println("Bariera zainicjalizowana dla " + totalThreads + " wątków.");
    }

    public synchronized void czekajNaBarierze() throws InterruptedException {
        int mojaGeneracja = generation;

        currentWaitCount++;

        if (currentWaitCount == totalThreads) {

            System.out.println(Thread.currentThread().getName() + ": OSTATNI WĄTEK, otwieram Barierę Generacji " + mojaGeneracja);

            generation++;
            currentWaitCount = 0;

            notifyAll(); 
        } else {

            while (mojaGeneracja == generation) {
                wait();
            }
        }
    }
}