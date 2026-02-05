#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "mpi.h"

#define MAX_HOSTNAME_LEN 256

int main(int argc, char** argv) {
    int rank, size, i;
    char hostname[MAX_HOSTNAME_LEN];
    char received_hostname[MAX_HOSTNAME_LEN];
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    // Pobierz nazwę hosta
    gethostname(hostname, MAX_HOSTNAME_LEN);
    
    if (size > 1) {
        if (rank == 0) {
            // Proces główny zbiera informacje od innych
            printf("Proces %d (zbierający) działa na: %s\n", rank, hostname);
            
            for (i = 1; i < size; i++) {
                MPI_Recv(received_hostname, MAX_HOSTNAME_LEN, MPI_CHAR,
                        MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                printf("Otrzymano od procesu %d: %s\n", 
                       status.MPI_SOURCE, received_hostname);
            }
        } else {
            // Pozostałe procesy wysyłają swoją nazwę hosta
            MPI_Send(hostname, strlen(hostname) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        }
    } else {
        printf("Pojedynczy proces %d działa na: %s\n", rank, hostname);
    }
    
    MPI_Finalize();
    return 0;
}
