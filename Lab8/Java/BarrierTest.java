import java.util.concurrent.ThreadLocalRandom;

class WatekRoboczy implements Runnable {
    private final Bariera barieraA;
    private final Bariera barieraB;
    private final String name;

    public WatekRoboczy(String name, Bariera a, Bariera b) {
        this.name = name;
        this.barieraA = a;
        this.barieraB = b;
        Thread.currentThread().setName(name);
    }

    @Override
    public void run() {
        try {
            System.out.println(name + " -> Przed Barierą A");
            Thread.sleep(ThreadLocalRandom.current().nextInt(100, 500));
            barieraA.czekajNaBarierze();

            System.out.println(name + " -> Pomiędzy Barierą A i B");
            Thread.sleep(ThreadLocalRandom.current().nextInt(100, 500));
            barieraB.czekajNaBarierze(); 

            System.out.println(name + " -> Po Barierze B");

        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}

public class BarrierTest {
    public static void main(String[] args) {
        final int NUM_THREADS = 5;

        // Dwie niezależne instancje bariery
        Bariera barrierA = new Bariera(NUM_THREADS);
        Bariera barrierB = new Bariera(NUM_THREADS);

        Thread[] threads = new Thread[NUM_THREADS];
        
        for (int i = 0; i < NUM_THREADS; i++) {
            threads[i] = new Thread(new WatekRoboczy("Watek-" + (i + 1), barrierA, barrierB));
            threads[i].start();
        }
        
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
        System.out.println("Wszystkie wątki zakończyły pracę.");
    }
}