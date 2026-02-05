#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<omp.h>

#include "search_max_openmp.h"

/********************** linear search ************************/
double search_max(
          double *A, 
          int p,
          int k
          )
{

  double a_max = A[p];
  int i;
  for(i=p+1; i<=k; i++) if(a_max < A[i]) a_max = A[i];

  return(a_max);
}

/************* parallel linear search - openmp ****************/
double search_max_openmp_simple(
          double *A, 
          int p,
          int k
          )
{

  double a_max = A[p];
  double a_max_local = a_max;

#pragma omp parallel default(none) firstprivate(A, p, k, a_max_local) shared(a_max)
  {
    int i;
#pragma omp for
    for(i=p+1; i<=k; i++) if(a_max_local < A[i]) a_max_local = A[i];
    
#pragma omp critical (cs_a_max)
    {
      if(a_max < a_max_local) a_max = a_max_local;
    }
    
  }
  
  return(a_max);
}


/************* parallel linear search - openmp ****************/
double search_max_openmp_task(
          double *A, 
          int p,
          int k
          )
{

  double a_max_overall = A[p]; // Zmieniono nazwę, aby uniknąć kolizji z a_max w tasku
#pragma omp parallel default(none) shared(a_max_overall) firstprivate(A,p,k)
  {
#pragma omp single
    {
      int num_threads = omp_get_num_threads();
      float n_float = k-p+1;

      int num_tasks_to_create = 2 * num_threads;
      int n_loc = ceil(n_float / num_tasks_to_create);   

      for(int itask=0; itask<num_tasks_to_create; itask++){
      int p_task = p + itask * n_loc;
      if(p_task > k) {
          break; 
      }
      int k_task = p + (itask + 1) * n_loc - 1;
      if(k_task > k) k_task = k;

#pragma omp task default(none) firstprivate(p_task, k_task) shared(A, a_max_overall)
      {
          double a_max_local_task = A[p_task];
          for(int i = p_task + 1; i <= k_task; i++) {
              if(a_max_local_task < A[i]) {
                  a_max_local_task = A[i];
              }
          }
#pragma omp critical (cs_a_max_overall)
          {
              if(a_max_overall < a_max_local_task) {
                  a_max_overall = a_max_local_task;
              }
          }
      } // end task definition
      } // end loop over tasks
    } // end single region    
  } // end parallel region
  
  return(a_max_overall);
}


/************ binary search (array not sorted) ****************/
double bin_search_max(
          double *a, 
          int p,
          int k
)
{

  if(p<k) {

    int s=(p+k)/2;

    double a_max_1 = bin_search_max(a, p, s);

    double a_max_2 = bin_search_max(a, s+1, k);

    //printf("p %d  k %d, maximal elements %lf, %lf\n", p, k, a_max_1, a_max_2); 


    if(a_max_1 < a_max_2) return(a_max_2);
    else return(a_max_1);

  }
  else{

    return(a[p]);

  }

}


/*** single task for parallel binary search (array not sorted) - openmp ***/
#define  MAX_LEVEL 4 

// Funkcja pomocnicza do wyszukiwania liniowego, gdy zadanie osiągnie limit poziomu
double linear_search_for_task(double *A, int p, int k) {
    return search_max(A, p, k);
}


double bin_search_max_task(
  double* A,   
  int p,      
  int r,
  int level 
         )
{
    if (p == r) {
        return A[p];
    }

    // Dodanie warunku stopu dla tworzenia zadań na podstawie poziomu
    if (level >= MAX_LEVEL) {
        return linear_search_for_task(A, p, r);
    }

    int s = (p + r) / 2;
    double max1, max2;

    
    int s_plus_1 = s + 1; 
    int next_level = level + 1; 

#pragma omp task shared(max1) default(none) firstprivate(A, p, s, next_level)
    max1 = bin_search_max_task(A, p, s, next_level);

#pragma omp task shared(max2) default(none) firstprivate(A, s_plus_1, r, next_level) 
    max2 = bin_search_max_task(A, s_plus_1, r, next_level); 


#pragma omp taskwait
    if (max1 < max2) {
        return max2;
    } else {
        return max1;
    }
}

/********** parallel binary search (array not sorted) - openmp ***********/

double bin_search_max_openmp(
          double *A, 
          int p,
          int k
){

  double a_max_result; 
#pragma omp parallel default(none) firstprivate(A,p,k) shared(a_max_result)
  {
#pragma omp single
    { 
#pragma omp task
      {
    a_max_result = bin_search_max_task(A,p,k,0);
      }
    }
  }
#pragma omp taskwait // Czekaj na zakończenie wszystkich zadań głównych
  
  return(a_max_result);
}