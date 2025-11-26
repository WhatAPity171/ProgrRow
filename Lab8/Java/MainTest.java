class Czytelnik implements Runnable {
    private final Czytelnia czytelnia;

    public Czytelnik(Czytelnia c) {
        this.czytelnia = c;
    }

    @Override
    public void run() {
        try {
            while (true) {
                czytelnia.chceCzytac();

                czytelnia.czytam();

                czytelnia.koniecCzytania();
                
                Thread.sleep((long) (Math.random() * 200));
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}

class Pisarz implements Runnable {
    private final Czytelnia czytelnia;

    public Pisarz(Czytelnia c) {
        this.czytelnia = c;
    }

    @Override
    public void run() {
        try {
            while (true) {
                czytelnia.chcePisac();
                
                czytelnia.pisze();
                
                czytelnia.koniecPisania();

                Thread.sleep((long) (Math.random() * 500));
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }
}

public class MainTest {
    public static void main(String[] args) {
        Czytelnia c = new Czytelnia();
        
        final int LICZBA_CZYTELNIKOW = 10;
        final int LICZBA_PISARZY = 3;

        Thread[] watki = new Thread[LICZBA_CZYTELNIKOW + LICZBA_PISARZY];

        for (int i = 0; i < LICZBA_PISARZY; i++) {
            watki[i] = new Thread(new Pisarz(c), "Pisarz-" + i);
            watki[i].start();
        }

        for (int i = 0; i < LICZBA_CZYTELNIKOW; i++) {
            watki[LICZBA_PISARZY + i] = new Thread(new Czytelnik(c), "Czytelnik-" + i);
            watki[LICZBA_PISARZY + i].start();
        }
        
        System.out.println("Test Monitora Czytelnia/Pisarze został uruchomiony. Aby zakończyć, użyj Ctrl+C.");
    }
}