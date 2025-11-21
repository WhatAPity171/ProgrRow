
public class IntegralAggregator {
    private double totalIntegral = 0.0; // Całkowity wynik całki

    
    public synchronized void addPartialIntegral(double partialResult) {
        totalIntegral += partialResult;
        // System.out.println("Dodano: " + partialResult + ", Aktualna suma: " + totalIntegral);
    }

    
    public double getTotalIntegral() {
        return totalIntegral;
    }
}