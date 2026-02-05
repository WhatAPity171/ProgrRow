#include<stdlib.h>
#include<stdio.h>
#include<omp.h>



static int f_threadprivate; 
#pragma omp threadprivate(f_threadprivate)

int main(){
  
#ifdef   _OPENMP
  printf("\nKompilator rozpoznaje dyrektywy OpenMP\n");
#endif

  int liczba_watkow;
  
  int a_shared = 1;
  int b_private = 2;
  int c_firstprivate = 3;
  int e_atomic=5;
  


  omp_set_num_threads(5);
  
  printf("przed wejsciem do obszaru rownoleglego -  nr_threads %d, thread ID %d\n",
   omp_get_num_threads(), omp_get_thread_num());
  printf("\ta_shared \t= %d\n", a_shared);
  printf("\tb_private \t= %d\n", b_private);
  printf("\tc_firstprivate \t= %d\n", c_firstprivate);
  printf("\te_atomic \t= %d\n", e_atomic);
  printf("\tf_threadprivate (globalna, przed pierwszym regionem) = %d\n", f_threadprivate);
    
  



#pragma omp parallel default(none) shared(a_shared, e_atomic) private(b_private) firstprivate(c_firstprivate )
  {
    int i;
    int d_local_private;
    

    // d_local_private jest zmienną lokalną dla każdego wątku, więc nie ma tu WAW między wątkami.
    d_local_private = a_shared + c_firstprivate; 
    //WAR a 46-55
    // zależność RAW na zmiennej a_shared w linii 55 (odczyt a_shared z main) - potencjalne race condition
    // zależność WAW na zmiennej a_shared w linii 55 (zapis a_shared przez wątek) - potencjalne race condition



    #pragma omp critical(a_shared_crit)
    for(i=0;i<10;i++){
      a_shared ++; 
    }

 



    for(i=0;i<10;i++){
      c_firstprivate += omp_get_thread_num();
    }

    // zależność RAW na zmiennej e_atomic w linii 74 (odczyt e_atomic z globalnej pamięci) - potencjalne race condition 
    // zależność WAW na zmiennej e_atomic w linii 74 (zapis e_atomic przez wątek) - potencjalne race condition 
    

    // Te zależności prowadzą do race conditions i w konsekwencji do niedeterministycznych i błędnych wyników.

    for(i=0;i<10;i++){
      #pragma omp atomic update
      e_atomic+=omp_get_thread_num();
    }



    
    f_threadprivate = omp_get_thread_num();


//bariera
    #pragma omp critical(printer)
    {
      
      printf("\n--- W PIERWSZYM obszarze równoległym (watek %d z %d) ---\n",
       omp_get_thread_num(), omp_get_num_threads());
      
      // zależność RAW na zmiennej a_shared w linii 91 (odczyt a_shared)
      printf("\ta_shared \t= %d\n", a_shared); 
      // zależność RAW na zmiennej b_private w linii 93 (odczyt b_private) 
      printf("\tb_private \t= %d\n", b_private);

      printf("\tc_firstprivate \t= %d\n", c_firstprivate);
      // zależność RAW na zmiennej d_local_private w linii 97 (odczyt d_local_private) 
      printf("\td_local_private = %d\n", d_local_private);
      // zależność RAW na zmiennej e_atomic w linii 99 (odczyt e_atomic)
      printf("\te_atomic \t= %d\n", e_atomic); 
      // zależność RAW na zmiennej f_threadprivate w linii 101 (odczyt f_threadprivate) 
      printf("\tf_threadprivate = %d\n", f_threadprivate); 
      printf("--------------------------------------------\n");

    }
  } // Koniec pierwszego obszaru równoległego
  
  printf("\npo zakonczeniu PIERWSZEGO obszaru rownoleglego:\n");
  // zależność RAW na zmiennej a_shared w linii 109 (odczyt a_shared)
  printf("\ta_shared \t= %d\n", a_shared);
  // zależność RAW na zmiennej b_private w linii 111 (odczyt b_private)
  printf("\tb_private \t= %d\n", b_private);
  // zależność RAW na zmiennej c_firstprivate w linii 113 (odczyt c_firstprivate)
  printf("\tc_firstprivate \t= %d\n", c_firstprivate); 
  // zależność RAW na zmiennej e_atomic w linii 115 (odczyt e_atomic)
  printf("\te_atomic \t= %d\n", e_atomic);
  printf("\tf_threadprivate (globalna, po pierwszym regionie) = %d\n", f_threadprivate); 




#pragma omp parallel default(none)
  {


    #pragma omp critical(printer2)
    {
      // zależność RAW na zmiennej f_threadprivate w linii 128 (odczyt f_threadprivate)
      printf("\n--- W DRUGIM obszarze równoległym (watek %d z %d) ---\n",
       omp_get_thread_num(), omp_get_num_threads());
      printf("\tf_threadprivate = %d (powinna być taka sama jak thread ID z pierwszego regionu)\n", f_threadprivate);
      printf("--------------------------------------------\n");
    }
  } // Koniec drugiego obszaru równoległego

  printf("\npo zakonczeniu DRUGIEGO obszaru rownoleglego:\n");
  printf("\tf_threadprivate (globalna, po drugim regionie) = %d\n", f_threadprivate);
  
  return 0;
}