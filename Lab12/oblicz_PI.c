#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mpi.h"

#define SCALAR double
//#define SCALAR float

#ifndef M_PI 
#define M_PI (3.14159265358979323846)
#endif

int main( int argc, char** argv ){ // program obliczania przybliżenia PI za pomocą wzoru Leibniza
                  // PI = 4 * ( 1 - 1/3 + 1/5 - 1/7 + 1/9 - 1/11 + itd. )
  
  int rank, size;
  int max_liczba_wyrazow=0;
  double t_start, t_end, t_total;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  if(rank==0){
    printf("Podaj maksymalną liczbę wyrazów do obliczenia przybliżenia PI\n");
    scanf("%d", &max_liczba_wyrazow);
  }
  
  // Broadcast liczby wyrazów do wszystkich procesów
  MPI_Bcast(&max_liczba_wyrazow, 1, MPI_INT, 0, MPI_COMM_WORLD);
  
  t_start = MPI_Wtime();
  
  // wzór: PI/4 = 1 - 1/3 + 1/5 - 1/7 + 1/9 itd. itp.  
  // Podział pracy między procesy
  int start = rank * (max_liczba_wyrazow / size);
  int end = (rank == size - 1) ? max_liczba_wyrazow : (rank + 1) * (max_liczba_wyrazow / size);
  
  SCALAR suma_plus=0.0;
  SCALAR suma_minus=0.0;
  int i=0;
  for(i=start; i<end; i++){  
    int j = 1 + 4*i; 
    suma_plus += 1.0/j;
    suma_minus += 1.0/(j+2.0);
  }
  
  SCALAR local_sum = suma_plus - suma_minus;
  SCALAR global_sum = 0.0;
  
  // Reduce - sumowanie częściowych wyników
  MPI_Reduce(&local_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
  
  t_end = MPI_Wtime();
  t_total = t_end - t_start;
  
  if(rank==0){
    double pi_approx = 4*global_sum;
    printf("PI obliczone: \t\t\t%20.15lf\n", pi_approx);
    printf("PI z biblioteki matematycznej: \t%20.15lf\n", M_PI);
    printf("Błąd bezwzględny: \t\t%20.15lf\n", fabs(pi_approx - M_PI));
    printf("Czas obliczeń: \t\t\t%lf sekund\n", t_total);
    printf("Liczba procesów: \t\t%d\n", size);
    // Output for plotting: processes time
    printf("#PERF_DATA: %d %lf\n", size, t_total);
  }
  
  MPI_Finalize();
  return 0;
}


