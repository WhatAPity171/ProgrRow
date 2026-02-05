#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

#define MAX_NAME_LEN 64

// Struktura danych do przetwarzania potokowego
typedef struct {
    int id;
    float value;
    char name[MAX_NAME_LEN];
} DataStruct;

int main(int argc, char** argv) {
    int rank, size, i;
    DataStruct data;
    char buffer[1024];
    int position;
    int buffer_size;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (size < 2) {
        if (rank == 0) {
            fprintf(stderr, "Wymagane minimum 2 procesy\n");
        }
        MPI_Finalize();
        return 1;
    }
    
    // SPMD: Single Program Multiple Data
    // Każdy proces wykonuje własne operacje na strukturze
    
    if (rank == 0) {
        // Proces początkowy inicjalizuje strukturę
        data.id = 1;
        data.value = 10.0;
        strcpy(data.name, "Pipeline");
        
        printf("Proces %d: Inicjalizuję strukturę (id=%d, value=%.2f, name=%s)\n",
               rank, data.id, data.value, data.name);
        
        // Wykonaj operację na strukturze
        data.value *= 2.0;
        data.id += rank;
        printf("Proces %d: Po operacji (id=%d, value=%.2f)\n",
               rank, data.id, data.value);
        
        // Pakuj i wyślij do następnego procesu
        position = 0;
        MPI_Pack(&data.id, 1, MPI_INT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(&data.value, 1, MPI_FLOAT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(data.name, MAX_NAME_LEN, MPI_CHAR, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        
        MPI_Send(buffer, position, MPI_PACKED, rank + 1, 0, MPI_COMM_WORLD);
        
    } else if (rank == size - 1) {
        // Proces końcowy odbiera i wykonuje finalne operacje
        MPI_Recv(buffer, sizeof(buffer), MPI_PACKED, rank - 1, MPI_ANY_TAG, 
                MPI_COMM_WORLD, &status);
        
        MPI_Get_count(&status, MPI_PACKED, &buffer_size);
        
        position = 0;
        MPI_Unpack(buffer, buffer_size, &position, &data.id, 1, MPI_INT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, &data.value, 1, MPI_FLOAT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, data.name, MAX_NAME_LEN, 
                   MPI_CHAR, MPI_COMM_WORLD);
        
        printf("Proces %d: Otrzymałem strukturę (id=%d, value=%.2f, name=%s)\n",
               rank, data.id, data.value, data.name);
        
        // Wykonaj finalne operacje
        data.value = sqrt(data.value);
        data.id *= rank;
        printf("Proces %d: Finalne operacje (id=%d, value=%.2f)\n",
               rank, data.id, data.value);
        
    } else {
        // Procesy środkowe: odbierają, przetwarzają, przekazują dalej
        MPI_Recv(buffer, sizeof(buffer), MPI_PACKED, rank - 1, MPI_ANY_TAG, 
                MPI_COMM_WORLD, &status);
        
        MPI_Get_count(&status, MPI_PACKED, &buffer_size);
        
        position = 0;
        MPI_Unpack(buffer, buffer_size, &position, &data.id, 1, MPI_INT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, &data.value, 1, MPI_FLOAT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, data.name, MAX_NAME_LEN, 
                   MPI_CHAR, MPI_COMM_WORLD);
        
        printf("Proces %d: Otrzymałem strukturę (id=%d, value=%.2f, name=%s)\n",
               rank, data.id, data.value, data.name);
        
        // Każdy proces wykonuje własne operacje
        data.value += rank * 1.5;
        data.id += rank * 10;
        printf("Proces %d: Po operacjach (id=%d, value=%.2f)\n",
               rank, data.id, data.value);
        
        // Pakuj i przekaż dalej
        position = 0;
        MPI_Pack(&data.id, 1, MPI_INT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(&data.value, 1, MPI_FLOAT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(data.name, MAX_NAME_LEN, MPI_CHAR, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        
        MPI_Send(buffer, position, MPI_PACKED, rank + 1, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
