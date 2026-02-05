#!/bin/bash

# Script do uruchamiania testów wydajnościowych
# Uruchamia programy z różną liczbą procesów/wątków i zbiera dane

MPIRUN="mpiexec"
PROCS=(1 2 4 8)
THREADS=(1 2 4 8)

echo "=== Testy wydajnościowe mat_vec_row_MPI ===" > performance_mat_vec.dat
echo "# processes time speedup efficiency" >> performance_mat_vec.dat

for np in "${PROCS[@]}"; do
    echo "Uruchamianie mat_vec_row_MPI z $np procesami..."
    $MPIRUN -np $np --oversubscribe ./mat_vec_row_MPI 1 2>&1 | grep "#PERF_DATA:" | sed 's/#PERF_DATA: //' >> performance_mat_vec.dat
done

echo ""
echo "=== Testy wydajnościowe calka_omp ===" > performance_calka.dat
echo "# threads time speedup efficiency" >> performance_calka.dat

for nt in "${THREADS[@]}"; do
    echo "Uruchamianie calka_omp z $nt wątkami..."
    ./calka_omp $nt 2>&1 | grep "#PERF_DATA:" | sed 's/#PERF_DATA: //' >> performance_calka.dat
done

echo ""
echo "Dane zapisane w performance_mat_vec.dat i performance_calka.dat"
