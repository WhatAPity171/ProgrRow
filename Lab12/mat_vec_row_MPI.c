#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mpi.h"


#define WYMIAR 10080 
#define ROZMIAR (WYMIAR*WYMIAR)

void mat_vec(double* a, double* x, double* y, int n, int nt);

int
main ( int argc, char** argv )
{
  static double x[WYMIAR],y[WYMIAR],z[WYMIAR]; // z is sized for column decomposition
  double *a;
  double t1;
  int n,nt,i,j;
  const int ione=1;
  const double done=1.0;
  const double dzero=0.0;
  
  int rank, size, source, dest, tag=0; 
  int n_wier, n_wier_last;
  MPI_Status status;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  
  n=WYMIAR;
  
  // x is locally initialized to zero
  for(i=0;i<WYMIAR;i++) x[i]=0.0;

  double t_seq = 0.0; // sequential time
  double t_par_row = 0.0; // parallel row decomposition time
  
  if(rank==0){  
    
    a = (double *) malloc((ROZMIAR+1)*sizeof(double));
    
    for(i=0;i<ROZMIAR;i++) a[i]=1.0*i;
    for(i=0;i<WYMIAR;i++) x[i]=1.0*(WYMIAR-i);
    
    
    // Get number of threads from command line or use 1
    nt = 1;
    if(argc > 1){
      nt = atoi(argv[1]);
    }
    
    printf("poczatek (wykonanie sekwencyjne)\n");
    
    t1 = MPI_Wtime();
    mat_vec(a,x,y,n,nt);
    t1 = MPI_Wtime() - t1;
    t_seq = t1;
    
    printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n",  
	   t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
    
  }
  
  // Broadcast sequential time for speedup calculation
  MPI_Bcast(&t_seq, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  
  if(size>1){
    
    /************** || block row decomposition || *******************/
    
    // z is initialized for all ranks
    for(i=0;i<WYMIAR;i++) z[i]=0.0;

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD );
    // podzial wierszowy
    n_wier = ceil(WYMIAR / size);
    n_wier_last = WYMIAR - n_wier*(size-1);
    
    // for n_wier!=n_wier_last arrays should be oversized to avoid problems
    if(n_wier!=n_wier_last){
      
      printf("This version does not work with WYMIAR not a multiple of size!\n");
      MPI_Finalize(); 
      exit(0);
      
    }

    // local matrices a_local form parts of a big matrix a
    double *a_local = (double *) malloc(WYMIAR*n_wier*sizeof(double)); 
    for(i=0;i<WYMIAR*n_wier;i++) a_local[i]=0.0;
    
    // Collective communication: Scatterv for distributing matrix rows
    int *sendcounts_a = NULL;
    int *displs_a = NULL;
    if(rank==0){
      sendcounts_a = (int *) malloc(size*sizeof(int));
      displs_a = (int *) malloc(size*sizeof(int));
      for(i=0;i<size;i++){
	sendcounts_a[i] = n_wier*WYMIAR;
	displs_a[i] = i*n_wier*WYMIAR;
      }
    }
    
    MPI_Scatterv(rank==0 ? a : NULL, sendcounts_a, displs_a, MPI_DOUBLE,
		  a_local, n_wier*WYMIAR, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    if(rank==0){
      free(sendcounts_a);
      free(displs_a);
    }
    
    // Broadcast vector x to all processes
    MPI_Bcast(x, WYMIAR, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    

    if(rank==0) {
      printf("Starting MPI matrix-vector product with block row decomposition!\n");
      t1 = MPI_Wtime();
    }
    
    // Note: After Broadcast, all processes have the full x vector, so Allgather is not needed
    // But we demonstrate MPI_IN_PLACE usage here (though redundant in this case)
    // In a real scenario without Broadcast, Allgather would be needed
    if(rank==0){
      MPI_Allgather( MPI_IN_PLACE, n_wier, MPI_DOUBLE, x, n_wier, MPI_DOUBLE, MPI_COMM_WORLD );
    } else {
      MPI_Allgather( &x[rank*n_wier], n_wier, MPI_DOUBLE, x, n_wier, MPI_DOUBLE, MPI_COMM_WORLD );
    }

    
    for(i=0;i<n_wier;i++){
            
      double t=0.0;
      int ni = n*i;
      
      for(j=0;j<n;j++){
	t+=a_local[ni+j]*x[j];
	//if(i==1){
	//  printf("rank %d: row %d, column %d, a %lf, x %lf, current y %lf\n", 
	//         rank, i, j, a_local[ni+j], x[j], t);
	//}
      }
      //printf("rank %d: row %d, final y %lf\n", rank, i, t);
      z[i]=t;
    }
    
    // just to measure time
    MPI_Barrier(MPI_COMM_WORLD);        
    if(rank==0) {
      t1 = MPI_Wtime() - t1;
      t_par_row = t1;
      double speedup = t_seq / t1;
      double efficiency = speedup / size;
      printf("Wersja rownolegla MPI z dekompozycją wierszową blokową\n");
      printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n",  
	     t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
      printf("\tprzyspieszenie: %lf, efektywność: %lf\n", speedup, efficiency);
      // Output for plotting: processes time speedup efficiency
      printf("#PERF_DATA: %d %lf %lf %lf\n", size, t1, speedup, efficiency);
    }
    
    // Collective communication: Gatherv for collecting results
    int *recvcounts_z = NULL;
    int *displs_z = NULL;
    if(rank==0){
      recvcounts_z = (int *) malloc(size*sizeof(int));
      displs_z = (int *) malloc(size*sizeof(int));
      for(i=0;i<size;i++){
	recvcounts_z[i] = n_wier;
	displs_z[i] = i*n_wier;
      }
    }
    
    MPI_Gatherv(z, n_wier, MPI_DOUBLE,
		 rank==0 ? z : NULL, recvcounts_z, displs_z, MPI_DOUBLE,
		 0, MPI_COMM_WORLD);
    
    if(rank==0){
      free(recvcounts_z);
      free(displs_z);
    }

      if(rank==0){
      
      for(i=0;i<WYMIAR;i++){
	if(fabs(y[i]-z[i])>1.e-9*z[i]) {
	  printf("Blad! i=%d, y[i]=%lf, z[i]=%lf\n",i, y[i], z[i]);
	}
      }
      
    }


    /************** || block column decomposition (collective only) || *******************/
    int n_col = n_wier; // each process processes ncol columns

    // z is initialized for all ranks
    for(i=0;i<WYMIAR;i++) z[i]=0.0;

    // local a is initialized to zero - now local a stores several columns (not rows as before)
    // Reallocate a_local for column storage: WYMIAR rows x n_col columns
    free(a_local);
    a_local = (double *) malloc(WYMIAR*n_col*sizeof(double)); 
    for(i=0;i<WYMIAR*n_col;i++) a_local[i]=0.0;
    
    // Distribute columns of matrix a using Scatterv
    // Each process gets n_col consecutive columns
    int *sendcounts_col = NULL;
    int *displs_col = NULL;
    if(rank==0){
      sendcounts_col = (int *) malloc(size*sizeof(int));
      displs_col = (int *) malloc(size*sizeof(int));
      for(i=0;i<size;i++){
	sendcounts_col[i] = WYMIAR*n_col; // each process gets WYMIAR*n_col elements
	displs_col[i] = i*n_col; // starting column index
      }
    }
    
    // Distribute columns using Scatterv for each row
    // For row-major storage, we need to extract columns
    int *sendcounts_row = (int *) malloc(size*sizeof(int));
    int *displs_row = (int *) malloc(size*sizeof(int));
    for(i=0;i<size;i++){
      sendcounts_row[i] = n_col;
      displs_row[i] = i*n_col;
    }
    
    // Distribute columns row by row
    double *row_send = NULL;
    if(rank==0) row_send = (double *) malloc(WYMIAR*sizeof(double));
    
    for(i=0;i<WYMIAR;i++){
      if(rank==0){
	// Extract row i from matrix a
	for(j=0;j<WYMIAR;j++) row_send[j] = a[i*WYMIAR + j];
      }
      MPI_Scatterv(rank==0 ? row_send : NULL, sendcounts_row, displs_row, MPI_DOUBLE,
		    &a_local[i*n_col], n_col, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
    
    if(rank==0) free(row_send);
    free(sendcounts_row);
    free(displs_row);
    
    // Distribute corresponding parts of vector x
    double *x_local = (double *) malloc(n_col*sizeof(double));
    MPI_Scatter(x, n_col, MPI_DOUBLE, x_local, n_col, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    if(rank==0){
      free(sendcounts_col);
      free(displs_col);
    }

    if(rank==0) {
      printf("Starting MPI matrix-vector product with block column decomposition!\n");
      t1 = MPI_Wtime();
    }

    // loop over rows (optimal since matrix a stored row major)
    for(i=0;i<n;i++){      
      double t=0.0;
      int ni = i*n_col; // row i starts at position i*n_col in a_local
      
      // Multiply local columns with corresponding x elements
      for(j=0;j<n_col;j++){
	t += a_local[ni+j]*x_local[j];
      }
      z[i]=t;
    }

    // Use Reduce with MPI_IN_PLACE to combine partial results from all processes
    // MPI_IN_PLACE can only be used on the receiving process (root)
    // For root, the input buffer is also the output buffer
    if(rank==0){
      MPI_Reduce(MPI_IN_PLACE, z, WYMIAR, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    } else {
      MPI_Reduce(z, NULL, WYMIAR, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
    }
    
    free(x_local);

    // just to measure time
    MPI_Barrier(MPI_COMM_WORLD);        
    if(rank==0) {
      t1 = MPI_Wtime() - t1;
      printf("Werja rownolegla MPI z dekompozycją kolumnową blokową\n");
      printf("\tczas wykonania: %lf, Gflop/s: %lf, GB/s> %lf\n",  
      	     t1, 2.0e-9*ROZMIAR/t1, (1.0+1.0/n)*8.0e-9*ROZMIAR/t1);
      
    }
    
    // testing - switch on after completing calculations and communcation
    if(rank==0){
      
      for(i=0;i<WYMIAR;i++){
	if(fabs(y[i]-z[i])>1.e-9*z[i]) {
	  printf("Blad! i=%d, y[i]=%lf, z[i]=%lf - complete the code for column decomposition\n",
		 i, y[i], z[i]);
	  break;
	  //printf("Blad! i=%d, y[i]=%lf, z[i]=%lf\n",i, y[i], z[i]);
	}
      }
      
    }
  

    // the end if size > 1
  }


  MPI_Finalize(); 
  
  return(0);
  
  
}

// slightly optimized version of matrix-vector product with possible OpenMP parallelization
void mat_vec(double* a, double* x, double* y, int nn, int nt)
{
  
  register int n=nn;
  register int i;
#ifdef _OPENMP
#pragma omp parallel for num_threads(nt) default(none) shared (a,x,y,n)
#endif
  for(i=0;i<n;i+=2){
    register double ty1 = 0;
    register double ty2 = 0;
    register int j;
    for(j=0;j<n;j+=2){
      register double t0=x[j];
      register double t1=x[j+1];
      register int k= i*n+j;
      ty1  +=a[k]*t0    +a[k+1]*t1 ;
      ty2  +=a[k+n]*t0  +a[k+1+n]*t1;
    }
    y[i]  = ty1;
    y[i+1]+=ty2;
  }
}
