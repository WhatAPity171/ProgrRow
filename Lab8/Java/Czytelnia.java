import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;


public class Czytelnia {

    private int l_c = 0; 
    private int l_p = 0; 

    private final Lock lock = new ReentrantLock();
    private final ReentrantLock reentrantLock = new ReentrantLock();

    private final Condition czytelnicyCzekaja = reentrantLock.newCondition();
    private final Condition pisarzeCzekaja = reentrantLock.newCondition();

    private int pisarzeOczekujacy = 0;

    public void chceCzytac() throws InterruptedException {
        reentrantLock.lock();
        try {
            while (l_p > 0 || pisarzeOczekujacy > 0) {
                System.out.println(Thread.currentThread().getName() + " -> Czekam na czytanie");
                czytelnicyCzekaja.await();
            }
            l_c++;
        } finally {
            reentrantLock.unlock();
        }
    }

    public void czytam() {
        if (l_p > 0) {
            System.err.println("BŁĄD: Czytelnik czyta, gdy aktywny jest pisarz! (l_p: " + l_p + ")");
            System.exit(1);
        }
        System.out.println(Thread.currentThread().getName() + " czyta. Aktywnych: " + l_c + "C, " + l_p + "P");
        
        try {
            Thread.sleep((long) (Math.random() * 50));
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void koniecCzytania() {
        reentrantLock.lock();
        try {
            l_c--;
            if (l_c == 0) {
                if (reentrantLock.hasWaiters(pisarzeCzekaja)) {
                    pisarzeCzekaja.signal();
                }
            }
        } finally {
            reentrantLock.unlock();
        }
    }


    public void chcePisac() throws InterruptedException {
        reentrantLock.lock();
        pisarzeOczekujacy++; 
        try {
            while (l_c > 0 || l_p > 0) {
                System.out.println(Thread.currentThread().getName() + " -> Czekam na pisanie");
                pisarzeCzekaja.await();
            }
            pisarzeOczekujacy--; 
            l_p = 1; 
        } finally {
            reentrantLock.unlock();
        }
    }

    public void pisze() {
        if (l_c > 0 || l_p != 1) {
            System.err.println("BŁĄD: Pisarz pisze z naruszeniem! (l_c: " + l_c + ", l_p: " + l_p + ")");
            System.exit(1);
        }
        System.out.println(Thread.currentThread().getName() + " PISZE. Aktywnych: " + l_c + "C, " + l_p + "P");

        try {
            Thread.sleep((long) (Math.random() * 100));
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void koniecPisania() {
        reentrantLock.lock();
        try {
            l_p = 0; 
            
            if (reentrantLock.hasWaiters(pisarzeCzekaja)) {
                pisarzeCzekaja.signal(); 
            } else {
                czytelnicyCzekaja.signalAll(); 
            }
        } finally {
            reentrantLock.unlock();
        }
    }
}