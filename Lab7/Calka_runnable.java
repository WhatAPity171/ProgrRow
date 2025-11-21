public class Calka_runnable implements Runnable {
    
    private double dx;
    private double xp;
    private double xk;
    private int N;
    private IntegralAggregator aggregator; // Referencja do obiektu agregującego wyniki

    /**
     * @param xp Dolna granica
     * @param xk Górna granica
     * @param dx dokladnosc calkowania
     * @param aggregator Obiekt IntegralAggregator do którego zostanie dodany częściowy wynik.
     */
    public Calka_runnable(double xp, double xk, double dx, IntegralAggregator aggregator) {
        this.xp = xp;
        this.xk = xk;
        this.aggregator = aggregator; // Przechowujemy referencję do agregatora


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
    }
    
    // Funkcja do całkowania (sin(x))
    private double getFunction(double x) {
        return Math.sin(x);
    }
    
    
    private double compute_integral() {
        double calka = 0;
        if (N == 0) return 0.0;

        for(int i = 0; i < N; i++){
            double x1 = xp + i * dx;
            double x2 = x1 + dx;
            calka += ((getFunction(x1) + getFunction(x2)) / 2.) * dx;
        }
        return calka;
    }
    
    @Override
    public void run() {
        double partialResult = compute_integral(); // Oblicz częściową całkę
        aggregator.addPartialIntegral(partialResult); // Dodaj wynik do wspólnego agregatora
    }
}