#include <stdlib.h>
#include <stdio.h>
#include "mpi.h"

int main(int argc, char** argv) {
    int rank, size, i;
    int message;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 3) {
        if (rank == 0) {
            fprintf(stderr, "Wymagane minimum 3 procesy. Uruchom z -np >= 3\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // Podział na 3 grupy:
    // - Proces początkowy: rank 0
    // - Procesy środkowe: ranky 1 do size-2
    // - Proces końcowy: rank size-1
    
    if (rank == 0) {
        // PROCES POCZĄTKOWY
        message = 100; // Wartość początkowa
        printf("Proces początkowy (rank %d): wysyłam wiadomość %d do procesu %d\n", 
               rank, message, rank + 1);
        MPI_Send(&message, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        
    } else if (rank == size - 1) {
        // PROCES KOŃCOWY
        MPI_Recv(&message, 1, MPI_INT, rank - 1, MPI_ANY_TAG, 
                MPI_COMM_WORLD, &status);
        printf("Proces końcowy (rank %d): otrzymałem wiadomość %d od procesu %d\n", 
               rank, message, status.MPI_SOURCE);
        
    } else {
        // PROCESY ŚRODKOWE
        MPI_Recv(&message, 1, MPI_INT, rank - 1, MPI_ANY_TAG, 
                MPI_COMM_WORLD, &status);
        printf("Proces środkowy (rank %d): otrzymałem %d od %d, przekazuję do %d\n", 
               rank, message, status.MPI_SOURCE, rank + 1);
        
        message += rank; // Modyfikacja wiadomości przez proces środkowy
        MPI_Send(&message, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
