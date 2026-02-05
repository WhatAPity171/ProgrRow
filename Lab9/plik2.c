#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 10 // Rozmiar tablicy N x N

int main ()
{
  double a[WYMIAR][WYMIAR];

  // Inicjalizacja tablicy
  for(int i = 0; i < WYMIAR; i++) {
    for(int j = 0; j < WYMIAR; j++) {
      a[i][j] = 1.02 * i + 1.01 * j;
    }
  }

  double suma_sekwencyjna = 0.0;
  for(int i = 0; i < WYMIAR; i++) {
    for(int j = 0; j < WYMIAR; j++) {
      suma_sekwencyjna += a[i][j];
    }
  }
  printf("Suma wyrazów tablicy (sekwencyjnie): %lf\n", suma_sekwencyjna);
  printf("==================================================\n\n");

  //potrzebne do 2d
  omp_set_nested(1);

  // ----------------------------------------------------------------------------------------------------
  // 1. Dekompozycja wierszowa
  printf("--- 1. Dekompozycja wierszowa (pętla zewnętrzna i, schedule static, chunk=2) ---\n");
  double suma_parallel_wierszowa = 0.0;
#pragma omp parallel for default(none) shared(a, stdout) reduction(+:suma_parallel_wierszowa) \
                         schedule(static, 2) num_threads(4) ordered
  for(int i = 0; i < WYMIAR; i++) { // Pętla zewnętrzna po wierszach
    int id_w = omp_get_thread_num();
    #pragma omp ordered //do wizualizacji
    printf("Wiersz %2d -> wątek %1d: ", i, id_w); // Wydruk przypisania wiersza do wątku
    for(int j = 0; j < WYMIAR; j++) {
      suma_parallel_wierszowa += a[i][j];
      // printf("(%1d,%1d)-W_%1d ",i,j,id_w);
    }
    #pragma omp ordered
    printf("\n");
  }
  printf("Suma wyrazów tablicy (dekompozycja wierszowa): %lf\n", suma_parallel_wierszowa);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_wierszowa - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n");


  // ----------------------------------------------------------------------------------------------------
  // 2. Dekompozycja kolumnowa
  printf("--- 2. Dekompozycja kolumnowa (pętla wewnętrzna j, schedule dynamic) ---\n");
  double suma_parallel_kolumnowa_wewn = 0.0;
  for(int i = 0; i < WYMIAR; i++) { // Pętla zewnętrzna (sekwencyjna) po wierszach
    printf("Przetwarzanie wiersza %2d:\n", i);
    // Tutaj dla każdego wiersza tworzony jest nowy obszar równoległy
#pragma omp parallel for default(none) shared(a, stdout, i) reduction(+:suma_parallel_kolumnowa_wewn) \
                         schedule(dynamic) num_threads(4) ordered
    for(int j = 0; j < WYMIAR; j++) { // Pętla wewnętrzna po kolumnach
      int id_w_zewn = omp_get_thread_num(); // Numer wątku dla pętli wewnętrznej
      #pragma omp ordered
      printf("  Kolumna %2d -> wątek %1d (w wierszu %d)\n", j, id_w_zewn, i);
      suma_parallel_kolumnowa_wewn += a[i][j];
    }
  }
  printf("Suma wyrazów tablicy (dekompozycja kolumnowa - wewn. pętla): %lf\n", suma_parallel_kolumnowa_wewn);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_kolumnowa_wewn - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n");


  // ----------------------------------------------------------------------------------------------------
  // 3. Dekompozycja kolumnowa (reczne sumowanie)
  printf("--- 3. Dekompozycja kolumnowa (zamiana pętli, pętla zewnętrzna j, ręczne sumowanie) ---\n");
  double suma_parallel_kolumnowa_zewn_rec = 0.0;
  // Zmienne prywatne dla wątków do sumowania częściowych wyników
  double suma_prywatna[omp_get_max_threads()]; 

#pragma omp parallel default(none) shared(a, stdout, suma_parallel_kolumnowa_zewn_rec, suma_prywatna) num_threads(4)
  {
    int id_w = omp_get_thread_num();
    suma_prywatna[id_w] = 0.0; //Prywatna suma dla kazdego watku

#pragma omp for schedule(static) ordered
    for(int j = 0; j < WYMIAR; j++) { //po kolumnach
      #pragma omp ordered
      printf("Kolumna %2d -> wątek %1d: ", j, id_w);
      for(int i = 0; i < WYMIAR; i++) { //po wierszach
        suma_prywatna[id_w] += a[i][j];
      }
      #pragma omp ordered
      printf("\n");
    }

    //sekcja krytyczna i sumowanie
#pragma omp critical
    {
      suma_parallel_kolumnowa_zewn_rec += suma_prywatna[id_w];
    }
  }
  printf("Suma wyrazów tablicy (dekompozycja kolumnowa - zewn. pętla, ręczne sumowanie): %lf\n", suma_parallel_kolumnowa_zewn_rec);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_kolumnowa_zewn_rec - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n");


  // ----------------------------------------------------------------------------------------------------
  // 4. Dekompozycja 2D (dwie zrównoleglone pętle) - UWAGA: to nie jest część zadania,
  //    ale często mylone z dekompozycją kolumnową wewnętrzną, więc dodane dla kontekstu.
  //    Nie jest to idealne rozwiązanie dla sumowania reduction bez zagnieżdżonego reduction.
  //    Należy potraktować jako ilustrację podziału iteracji.
  printf("--- 4. Dekompozycja 2D (pętla zewnętrzna i, pętla wewnętrzna j) ---\n");
  double suma_parallel_2d = 0.0;
  // Dla dekompozycji 2D, aby sensownie użyć reduction, potrzebne jest zagnieżdżone reduction lub inne podejście,
  // ale dla celów demonstracji podziału iteracji to zadziała.
  // Ważne: `ordered` na pętli wewnętrznej z `parallel for` zadziała tylko, jeśli wątki są zagnieżdżone,
  // ale wydruk będzie pokazywał ID wątków z wewnętrznej pętli dla każdej iteracji (j).
#pragma omp parallel for default(none) shared(a, stdout) reduction(+:suma_parallel_2d) \
                         schedule(static) num_threads(4) ordered // schedule dla pętli I
  for(int i = 0; i < WYMIAR; i++) {
    int id_w_outer = omp_get_thread_num(); // ID wątku zewnętrznej pętli
    #pragma omp ordered
    printf("Wiersz %2d (w_zewn=%1d): ", i, id_w_outer);
    // Wewnętrzna pętla nie jest zrównoleglona dyrektywą OpenMP, więc jest wykonywana sekwencyjnie
    // przez ten sam wątek, który dostał wiersz 'i'.
    for(int j = 0; j < WYMIAR; j++) {
      suma_parallel_2d += a[i][j];
      #pragma omp ordered
      printf("(%1d,%1d)-W_%1d ", i, j, id_w_outer);
    }
    #pragma omp ordered
    printf("\n");
  }
  printf("Suma wyrazów tablicy (dekompozycja 2D - jako ilustracja): %lf\n", suma_parallel_2d);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_2d - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n");

  return 0;
}