import java.util.concurrent.Callable;

public class Calka_callable implements Callable<Double> {
    
    private double dx;
    private double xp;
    private double xk;
    private int N;
    
    public Calka_callable(double xp, double xk, double dx) {
        this.xp = xp;
        this.xk = xk;

        if (xp == xk) {
            this.N = 0;
            this.dx = 0;
        } else {
            this.N = (int) Math.ceil((xk - xp) / dx);
            if (this.N == 0) {
                this.N = 1;
            }
            this.dx = (xk - xp) / this.N; 
        }

        // System.out.println("Tworzę Calka_callable: xp=" + xp + ", xk=" + xk + ", N=" + N + ", final_dx=" + this.dx);
    }
    
    private double getFunction(double x) {
        return Math.sin(x);
    }
    
    public double compute_integral() {
        double calka = 0;
        if (N == 0) return 0.0;

        for(int i = 0; i < N; i++){
            double x1 = xp + i * dx;
            double x2 = x1 + dx;
            calka += ((getFunction(x1) + getFunction(x2)) / 2.) * dx;
        }
        // System.out.println("Częściowa całka [" + String.format("%.5f", xp) + ", " + String.format("%.5f", xk) + "]: " + calka);
        return calka;
    }
    
    @Override
    public Double call() throws Exception {
        return compute_integral();
    }
}