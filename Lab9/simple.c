#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

#define WYMIAR 18

int main ()
{
  double a[WYMIAR];

#pragma omp parallel for default(none) shared(a)
  for(int i = 0; i < WYMIAR; i++) a[i] = 1.02 * i;

  // pętla sekwencyjna
  double suma = 0.0;
  for(int i = 0; i < WYMIAR; i++) {
      suma += a[i];
  }
  
  printf("Suma wyrazów tablicy (sekwencyjnie): %lf\n", suma);
  //zmienna srodowiskowa ustawia liczbe watkow
  // pętla do modyfikacji - docelowo równoległa w OpenMP
  double suma_parallel_default = 0.0;
#pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel_default) ordered 
  for(int i = 0; i < WYMIAR; i++) {
    int id_w = omp_get_thread_num();
    #pragma omp ordered
    printf("Domyślnie - a[%2d]->W_%1d\n", i, id_w); 
    suma_parallel_default += a[i];
  }
  printf("\nSuma wyrazów tablicy równolegle (default, ordered): %lf\n", suma_parallel_default);
  printf("--------------------------------------------------\n");


  // Testowanie klauzul schedule
  omp_set_num_threads(4);
  //funkcja ustawia liczbe watkow
  // 1. static, rozmiar porcji=3
  double suma_parallel_static_chunk3 = 0.0;
  printf("\n--- Klauzula schedule(static, 3) ---\n");
#pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel_static_chunk3) ordered schedule(static, 3)
  for(int i = 0; i < WYMIAR; i++) {
    int id_w = omp_get_thread_num();
    #pragma omp ordered
    printf("static, 3 - a[%2d]->W_%1d\n", i, id_w); 
    suma_parallel_static_chunk3 += a[i];
  }
  printf("Suma wyrazów tablicy równolegle (static, 3): %lf\n", suma_parallel_static_chunk3);
  printf("--------------------------------------------------\n");

  //dalej liczbe watkow ustawia dyrektywa openMP
  // 2. static, rozmiar porcji domyślny
  double suma_parallel_static_default = 0.0;
  printf("\n--- Klauzula schedule(static) ---\n");
#pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel_static_default) ordered schedule(static) num_threads(4)
  for(int i = 0; i < WYMIAR; i++) {
    int id_w = omp_get_thread_num();
    #pragma omp ordered
    printf("static, domyślny - a[%2d]->W_%1d\n", i, id_w); 
    suma_parallel_static_default += a[i];
  }
  printf("Suma wyrazów tablicy równolegle (static, domyślny): %lf\n", suma_parallel_static_default);
  printf("--------------------------------------------------\n");


  // 3. dynamic, rozmiar porcji=2
  double suma_parallel_dynamic_chunk2 = 0.0;
  printf("\n--- Klauzula schedule(dynamic, 2) ---\n");
#pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel_dynamic_chunk2) ordered schedule(dynamic, 2) num_threads(4)
  for(int i = 0; i < WYMIAR; i++) {
    int id_w = omp_get_thread_num();
    #pragma omp ordered
    printf("dynamic, 2 - a[%2d]->W_%1d\n", i, id_w); 
    suma_parallel_dynamic_chunk2 += a[i];
  }
  printf("Suma wyrazów tablicy równolegle (dynamic, 2): %lf\n", suma_parallel_dynamic_chunk2);
  printf("--------------------------------------------------\n");


  // 4. dynamic, rozmiar porcji domyślny
  double suma_parallel_dynamic_default = 0.0;
  printf("\n--- Klauzula schedule(dynamic) ---\n");
#pragma omp parallel for default(none) shared(a) reduction(+:suma_parallel_dynamic_default) ordered schedule(dynamic) num_threads(4)
  for(int i = 0; i < WYMIAR; i++) {
    int id_w = omp_get_thread_num();
    #pragma omp ordered
    printf("dynamic, domyślny - a[%2d]->W_%1d\n", i, id_w); 
    suma_parallel_dynamic_default += a[i];
  }
  printf("Suma wyrazów tablicy równolegle (dynamic, domyślny): %lf\n", suma_parallel_dynamic_default);
  printf("--------------------------------------------------\n");

  return 0;
}