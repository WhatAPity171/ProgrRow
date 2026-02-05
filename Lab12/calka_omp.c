#include<stdio.h>
#include<stdlib.h>
#include<omp.h>

double funkcja ( double x );

double funkcja ( double x ){ return( x*x*x ); }

int main( int argc, char *argv[] ){

  printf("\nProgram obliczania całki metodą trapezów.\n");

  double a = 0.0;
  double b = 1.0;
  int N = 100000000;
  double dx_adjust = (b-a)/N;

  // Get number of threads from command line or use 1
  int num_threads = 1;
  if(argc > 1){
    num_threads = atoi(argv[1]);
  }

  double t_seq = 0.0; // sequential time
  double t_par = 0.0;  // parallel time
  
  int i;
  double calka = 0.0;

  // First run with 1 thread to get sequential time
  printf("\nPoczatek obliczeń sekwencyjnych (1 wątek)\n");
  omp_set_num_threads(1);
  double t1 = omp_get_wtime();
  calka = 0.0;
#pragma omp parallel for default(none) firstprivate(N, a, dx_adjust) reduction(+:calka)
  for(i=0; i<N; i++){
    double x1 = a + i*dx_adjust;
    calka += 0.5*dx_adjust*(funkcja(x1)+funkcja(x1+dx_adjust));
  }
  t1 = omp_get_wtime() - t1;
  t_seq = t1;
  printf("\tCzas wykonania sekwencyjny: %lf. \tObliczona całka = %.15lf\n", t1, calka);

  // Now run with specified number of threads
  if(num_threads > 1){
    printf("\nPoczatek obliczeń równoległych OpenMP (%d wątków)\n", num_threads);
    omp_set_num_threads(num_threads);
    t1 = omp_get_wtime();
    calka = 0.0;
#pragma omp parallel for default(none) firstprivate(N, a, dx_adjust) reduction(+:calka)
    for(i=0; i<N; i++){
      double x1 = a + i*dx_adjust;
      calka += 0.5*dx_adjust*(funkcja(x1)+funkcja(x1+dx_adjust));
    }
    t1 = omp_get_wtime() - t1;
    t_par = t1;
    printf("\tCzas wykonania równoległy: %lf. \tObliczona całka = %.15lf\n", t1, calka);
    
    double speedup = t_seq / t_par;
    double efficiency = speedup / num_threads;
    printf("\tPrzyspieszenie: %lf, Efektywność: %lf\n", speedup, efficiency);
    // Output for plotting: threads time speedup efficiency
    printf("#PERF_DATA: %d %lf %lf %lf\n", num_threads, t_par, speedup, efficiency);
  } else {
    // For 1 thread, speedup and efficiency are 1.0
    printf("#PERF_DATA: %d %lf %lf %lf\n", 1, t_seq, 1.0, 1.0);
  }

  return 0;
}

