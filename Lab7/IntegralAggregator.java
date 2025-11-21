
public class IntegralAggregator {
    private double totalIntegral = 0.0; // Całkowity wynik całki

    /**
      @param partialResult Częściowy wynik całki obliczony przez wątek.
     */
    public synchronized void addPartialIntegral(double partialResult) {
        totalIntegral += partialResult;
        // System.out.println("Dodano: " + partialResult + ", Aktualna suma: " + totalIntegral);
    }

    /**
     * @return Całkowita suma częściowych całek.
     */
    public double getTotalIntegral() {
        return totalIntegral;
    }
}