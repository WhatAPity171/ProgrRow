#include<stdlib.h>
#include<stdio.h>
#include<omp.h>

int main(){
  
#ifdef   _OPENMP
  printf("\nKompilator rozpoznaje dyrektywy OpenMP\n");
#endif

  int liczba_watkow;
  
  int a_shared = 1;
  int b_private = 2;
  int c_firstprivate = 3;
  int e_atomic=5;
  
  
  printf("przed wejsciem do obszaru rownoleglego -  nr_threads %d, thread ID %d\n",
   omp_get_num_threads(), omp_get_thread_num());
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
    
  
#pragma omp parallel default(none) shared(a_shared, e_atomic) private(b_private) firstprivate(c_firstprivate )
  {
    int i;
    int d_local_private;
    // zależność RAW na zmiennej a_shared w linii 35 (odczyt a_shared z main, zapis do d_local_private)
    // zależność RAW na zmiennej c_firstprivate w linii 35 (odczyt c_firstprivate z main, zapis do d_local_private)
    // d_local_private jest zmienną lokalną dla każdego wątku, więc nie ma tu WAW między wątkami.
    d_local_private = a_shared + c_firstprivate; 
    
    // zależność RAW na zmiennej a_shared w linii 38 (odczyt a_shared z main) - potencjalne race condition
    // zależność WAW na zmiennej a_shared w linii 39 (zapis a_shared przez wątek) - potencjalne race condition
    // Przed poprawkami: Biorąc pod uwagę równoczesne dostęp do a_shared z wielu wątków, występuje WAR (write-after-read)
    // na a_shared, ponieważ inne wątki mogłyby odczytać a_shared, zanim bieżący wątek wykona inkrementację.
    // To wszystko są race conditions, które prowadzą do niedeterminizmu i błędnych wyników.



    #pragma omp critical(a_shared_crit)
    for(i=0;i<10;i++){
      a_shared ++; 
    }

    // zależność RAW na zmiennej c_firstprivate w linii 42 (odczyt c_firstprivate z firstprivate)
    // zależność WAW na zmiennej c_firstprivate w linii 43 (zapis c_firstprivate przez wątek)
    // c_firstprivate jest zmienną firstprivate, więc każdy wątek ma swoją niezależną kopię.
    // Zależności RAW/WAW występują tylko w ramach pojedynczego wątku, są bezpieczne.
    for(i=0;i<10;i++){
      c_firstprivate += omp_get_thread_num(); // c_firstprivate jest pierwszym prywatnym, więc każdy wątek ma swoją kopię niezależnie
    }

    // zależność RAW na zmiennej e_atomic w linii 46 (odczyt e_atomic z globalnej pamięci) - potencjalne race condition
    // zależność WAW na zmiennej e_atomic w linii 47 (zapis e_atomic przez wątek) - potencjalne race condition
    // Przed poprawkami: Występuje również WAR na e_atomic, ponieważ inne wątki mogłyby odczytać e_atomic, zanim 
    // bieżący wątek zakończy operację dodawania.
    // Te zależności prowadzą do race conditions i w konsekwencji do niedeterministycznych i błędnych wyników.

    for(i=0;i<10;i++){
      #pragma omp atomic update
      e_atomic+=omp_get_thread_num();
    }

    

    // Użycie sekcji krytycznej do drukowania, aby wydruki nie były przeplatane
    #pragma omp critical(printer)
    {
      
      printf("\n--- W obszarze równoległym (watek %d z %d) ---\n",
       omp_get_thread_num(), omp_get_num_threads());
      
      // zależność RAW na zmiennej a_shared w linii 73 (odczyt a_shared)
      printf("\ta_shared \t= %d\n", a_shared);
      // zależność RAW na zmiennej b_private w linii 74 (odczyt b_private)
      printf("\tb_private \t= %d\n", b_private);
      // zależność RAW na zmiennej c_firstprivate w linii 75 (odczyt c_firstprivate)
      printf("\tc_firstprivate \t= %d\n", c_firstprivate);
      // zależność RAW na zmiennej d_local_private w linii 76 (odczyt d_local_private)
      printf("\td_local_private = %d\n", d_local_private);
      // zależność RAW na zmiennej e_atomic w linii 77 (odczyt e_atomic)
      printf("\te_atomic \t= %d\n", e_atomic);
      printf("--------------------------------------------\n");

    }
    
    //#pragma omp single
/* #pragma omp master */
/*         { */
    
/*           printf("\ninside single: nr_threads %d, thread ID %d\n", */
/*           omp_get_num_threads(), omp_get_thread_num()); */
/*           /\* Get environment information *\/ */
/*           int procs = omp_get_num_procs(); */
/*           int nthreads = omp_get_num_threads(); // Poprawiono na omp_get_num_threads() */
/*           int maxt = omp_get_max_threads(); */
/*           int inpar = omp_in_parallel(); */
/*           int dynamic = omp_get_dynamic(); */
/*           int nested = omp_get_nested(); */
    
/*           /\* Print environment information *\/ */
/*           printf("Number of processors = %d\n", procs); */
/*           printf("Number of threads = %d\n", nthreads); */
/*           printf("Max threads = %d\n", maxt); */
/*           printf("In parallel? = %d\n", inpar); */
/*           printf("Dynamic threads enabled? = %d\n", dynamic); */
/*           printf("Nested parallelism supported? = %d\n", nested); */
    
/*         } */
    
  }
  
  printf("po zakonczeniu obszaru rownoleglego:\n");
  // zależność RAW na zmiennej a_shared w linii 108 (odczyt a_shared)
  printf("\ta_shared \t= %d\n", a_shared);
  // zależność RAW na zmiennej b_private w linii 109 (odczyt b_private)
  printf("\tb_private \t= %d\n", b_private);
  // zależność RAW na zmiennej c_firstprivate w linii 110 (odczyt c_firstprivate)
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  // zależność RAW na zmiennej e_atomic w linii 111 (odczyt e_atomic)
  printf("\te_atomic \t= %d\n", e_atomic);
  
}