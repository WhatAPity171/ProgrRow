#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 10

int main ()
{
  double a[WYMIAR][WYMIAR];

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
  printf("==================================================\n\n\n");

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
    printf("Wiersz %2d -> wątek %1d: ", i, id_w); //wydruk for "fun"
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
  //4.2D (zrównoleglenie obu pętli po wierszach i po kolumnach)
  printf("--- 4. Dekompozycja 2D (zagnieżdżona, schedule static, chunk=2 dla obu) ---\n");
  printf("   Użyto num_threads(3) dla zewn. i num_threads(2) dla wewn.\n");
  
  double suma_parallel_2d_nested = 0.0;
  int num_threads_outer = 3; // watki dla wierszy (zewn)
  int num_threads_inner = 2; // watki dla kolumn (wewn)

#pragma omp parallel for default(none) shared(a, stdout, num_threads_inner) \
                         reduction(+:suma_parallel_2d_nested) \
                         schedule(static, 2) num_threads(num_threads_outer) ordered firstprivate(num_threads_outer)
  for(int i = 0; i < WYMIAR; i++) { //zewn
    int id_w_outer = omp_get_thread_num();
    #pragma omp ordered
    printf("Wiersz %2d (wątek zewn. W_%d): \n", i, id_w_outer);

    //obszar zagnieżdżony
#pragma omp parallel for default(none) shared(a, stdout, i, id_w_outer) \
                         reduction(+:suma_parallel_2d_nested) \
                         schedule(static, 2) num_threads(num_threads_inner) ordered
    for(int j = 0; j < WYMIAR; j++) { //wewn
      int id_w_inner = omp_get_thread_num();
      #pragma omp ordered
      printf("  [%2d,%2d] -> W_zewn=%d, W_wewn=%d\n", i, j, id_w_outer, id_w_inner);
      suma_parallel_2d_nested += a[i][j];
    }
  }
  printf("Suma wyrazów tablicy (dekompozycja 2D zagnieżdżona, bloki 2x2): %lf\n", suma_parallel_2d_nested);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_2d_nested - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n\n");


  // ----------------------------------------------------------------------------------------------------
  // schedule static (prof. Banaś chciał a nie zobaczyłem na Upel)
  printf("--- 5. Dekompozycja 2D (zagnieżdżona, schedule static bez chunk) ---\n");
  printf("   Użyto num_threads(3) dla zewn. i num_threads(2) dla wewn.\n");

  double suma_parallel_2d_nested_no_chunk = 0.0;

#pragma omp parallel for default(none) shared(a, stdout, num_threads_inner) \
                         reduction(+:suma_parallel_2d_nested_no_chunk) \
                         schedule(static) num_threads(num_threads_outer) ordered firstprivate(num_threads_outer)
  for(int i = 0; i < WYMIAR; i++) { // Pętla zewnętrzna po wierszach
    int id_w_outer = omp_get_thread_num();
    #pragma omp ordered
    printf("Wiersz %2d (wątek zewn. W_%d): \n", i, id_w_outer);

#pragma omp parallel for default(none) shared(a, stdout, i, id_w_outer) \
                         reduction(+:suma_parallel_2d_nested_no_chunk) \
                         schedule(static) num_threads(num_threads_inner) ordered
    for(int j = 0; j < WYMIAR; j++) { // Pętla wewnętrzna po kolumnach
      int id_w_inner = omp_get_thread_num();
      #pragma omp ordered
      printf("  [%2d,%2d] -> W_zewn=%d, W_wewn=%d\n", i, j, id_w_outer, id_w_inner);
      suma_parallel_2d_nested_no_chunk += a[i][j];
    }
  }
  printf("Suma wyrazów tablicy (dekompozycja 2D zagnieżdżona, static - domyślny chunk): %lf\n", suma_parallel_2d_nested_no_chunk);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_2d_nested_no_chunk - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");
  printf("==================================================\n\n\n");


  // ----------------------------------------------------------------------------------------------------
  //dekompozycja wierszowa ale inna
  printf("--- 4.0. Dekompozycja wierszowa (pętla zewnętrzna j, wewnętrzna i) - alternatywna redukcja ---\n");

  //tablica do czesciowych sum
  double *thread_sums_array = (double*) calloc(omp_get_max_threads(), sizeof(double));
  if (thread_sums_array == NULL) {
      perror("Failed to allocate memory for thread_sums_array");
      return 1;
  }

  //każdy wątek sumuje w swojej części współdzielonej tablicy 1D o rozmiarze == liczbie wątków
  printf("\n---a) Alokacja tablicy do sumowania częściowego dla każdego wątku (wymiar = liczba wątków) ---\n");
  double suma_parallel_alt_red1 = 0.0;
  for(int j = 0; j < WYMIAR; j++) { //sekwencyjna po kolumnach
    printf("Przetwarzanie kolumny %2d:\n", j);
#pragma omp parallel for default(none) shared(a, stdout, j, thread_sums_array) \
                         schedule(static, 1) num_threads(4) ordered
    for(int i = 0; i < WYMIAR; i++) { //rownoległa po wierszach
      int id_w = omp_get_thread_num();
      #pragma omp ordered
      printf("  [%2d,%2d] -> W_kolumny=%d\n", i, j, id_w);
      thread_sums_array[id_w] += a[i][j]; // Każdy wątek dodaje do swojej pozycji w tablicy
    }
    //tu jest w sumie bariera wynikajaca z konca rownoleglych obliczen
  }

  //sumowanko
  for(int k = 0; k < omp_get_max_threads(); k++) {
    suma_parallel_alt_red1 += thread_sums_array[k];
  }
  printf("Suma wyrazów tablicy (alternatywna redukcja 1): %lf\n", suma_parallel_alt_red1);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_alt_red1 - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");

  for(int k = 0; k < omp_get_max_threads(); k++) {
      thread_sums_array[k] = 0.0;
  }


  //każdy wątek dodaje do wiersza w tablicy wyników o rozmiarze == liczbie wierszy
  printf("\n---b) Alokacja tablicy do sumowania wierszowego (wymiar = liczba wierszy) ---\n");
  double *row_sums_array = (double*) calloc(WYMIAR, sizeof(double));
  if (row_sums_array == NULL) {
      perror("Failed to allocate memory for row_sums_array");
      free(thread_sums_array);
      return 1;
  }
  double suma_parallel_alt_red2 = 0.0;

  for(int j = 0; j < WYMIAR; j++) {
    printf("Przetwarzanie kolumny %2d:\n", j);
#pragma omp parallel for default(none) shared(a, stdout, j, row_sums_array) \
                         schedule(dynamic) num_threads(4) ordered
    for(int i = 0; i < WYMIAR; i++) {
      int id_w = omp_get_thread_num();
      #pragma omp ordered
      printf("  [%2d,%2d] -> W_kolumny=%d\n", i, j, id_w);
      #pragma omp atomic
      row_sums_array[i] += a[i][j];
    }
  }

  //sumowanko
  for(int k = 0; k < WYMIAR; k++) {
    suma_parallel_alt_red2 += row_sums_array[k];
  }
  printf("Suma wyrazów tablicy (alternatywna redukcja 2): %lf\n", suma_parallel_alt_red2);
  printf("Sprawdzenie poprawności: %s\n", fabs(suma_parallel_alt_red2 - suma_sekwencyjna) < 1e-9 ? "OK" : "BŁĄD");

  free(thread_sums_array);
  free(row_sums_array);
  printf("==================================================\n\n");

  return 0;
}