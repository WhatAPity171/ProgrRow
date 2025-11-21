import java.util.Arrays;
import java.util.concurrent.RecursiveTask;

class DivideTask extends RecursiveTask<int[]> {
    
    private int[] arrayToDivide;
    
    private static final int SEQUENTIAL_THRESHOLD = 1024; // Przykładowo 1024 elementy

    public DivideTask(int[] arrayToDivide) {
        this.arrayToDivide = arrayToDivide;
    }
    
    @Override
    protected int[] compute() {
        // Jeśli fragment tablicy jest wystarczająco mały, sortuj go sekwencyjnie.
        if (arrayToDivide.length <= SEQUENTIAL_THRESHOLD) {
            Arrays.sort(arrayToDivide);
            return arrayToDivide;
        }
        
        int mid = arrayToDivide.length / 2;
        
        // Tworzenie nowych tablic dla lewej i prawej połówki.
        int[] leftHalf = Arrays.copyOfRange(arrayToDivide, 0, mid);
        int[] rightHalf = Arrays.copyOfRange(arrayToDivide, mid, arrayToDivide.length);
        
        // Utwórz nowe zadania dla każdej połówki.
        DivideTask leftTask = new DivideTask(leftHalf);
        DivideTask rightTask = new DivideTask(rightHalf);
        
        leftTask.fork();
        rightTask.fork();
        int[] sortedRight = rightTask.join();
        //int[] sortedRight = rightTask.compute();
        int[] sortedLeft = leftTask.join();
        
        // Po otrzymaniu posortowanych połówek, scal je w jedną posortowaną tablicę.
        int[] mergedArray = new int[arrayToDivide.length]; // Utwórz nową tablicę na wynik scalania.
        scal_tab(sortedLeft, sortedRight, mergedArray);
        
        return mergedArray; // Zwróć posortowaną i scaloną tablicę.
    }
    
    /**
     * @param tab1 Pierwsza posortowana tablica.
     * @param tab2 Druga posortowana tablica.
     * @param scal_tab Tablica docelowa, do której zostaną scalone elementy. Musi mieć wystarczający rozmiar.
     */
    private void scal_tab(
            int[] tab1,
            int[] tab2,
            int[] scal_tab) {
        
        int i = 0, j = 0, k = 0; // i dla tab1, j dla tab2, k dla scal_tab
        
        // Porównuj elementy z obu tablic i dodawaj mniejszy do scal_tab
        while ((i < tab1.length) && (j < tab2.length)) {
            if (tab1[i] < tab2[j]) {
                scal_tab[k++] = tab1[i++];
            } else {
                scal_tab[k++] = tab2[j++];
            }
        }
        
        // Kopiuj pozostałe elementy z tab1, jeśli takie są
        while (i < tab1.length) {
            scal_tab[k++] = tab1[i++];
        }
        
        // Kopiuj pozostałe elementy z tab2, jeśli takie są
        while (j < tab2.length) {
            scal_tab[k++] = tab2[j++];
        }
    }
}