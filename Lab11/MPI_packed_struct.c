#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mpi.h"

#define MAX_NAME_LEN 64

// Struktura danych do przesyłania
typedef struct {
    int id;
    float value;
    char name[MAX_NAME_LEN];
} DataStruct;

int main(int argc, char** argv) {
    int rank, size, i;
    DataStruct send_data, recv_data;
    char buffer[1024]; // Bufor dla pakowanych danych
    int position = 0;
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
    
    if (rank == 0) {
        // Proces 0 przygotowuje strukturę i pakuje ją
        send_data.id = 42;
        send_data.value = 3.14159;
        strcpy(send_data.name, "Testowa struktura");
        
        printf("Proces %d: Pakuję strukturę (id=%d, value=%.2f, name=%s)\n",
               rank, send_data.id, send_data.value, send_data.name);
        
        // Pakowanie struktury do bufora
        position = 0;
        MPI_Pack(&send_data.id, 1, MPI_INT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(&send_data.value, 1, MPI_FLOAT, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        MPI_Pack(send_data.name, MAX_NAME_LEN, MPI_CHAR, buffer, sizeof(buffer), 
                 &position, MPI_COMM_WORLD);
        
        buffer_size = position;
        
        // Wysyłanie pakowanych danych do procesu 1
        MPI_Send(buffer, buffer_size, MPI_PACKED, 1, 0, MPI_COMM_WORLD);
        printf("Proces %d: Wysłałem %d bajtów pakowanych danych\n", rank, buffer_size);
        
    } else if (rank == 1) {
        // Proces 1 odbiera i rozpakowuje strukturę
        MPI_Recv(buffer, sizeof(buffer), MPI_PACKED, 0, MPI_ANY_TAG, 
                MPI_COMM_WORLD, &status);
        
        MPI_Get_count(&status, MPI_PACKED, &buffer_size);
        printf("Proces %d: Otrzymałem %d bajtów pakowanych danych\n", rank, buffer_size);
        
        // Rozpakowywanie struktury z bufora
        position = 0;
        MPI_Unpack(buffer, buffer_size, &position, &recv_data.id, 1, MPI_INT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, &recv_data.value, 1, MPI_FLOAT, 
                   MPI_COMM_WORLD);
        MPI_Unpack(buffer, buffer_size, &position, recv_data.name, MAX_NAME_LEN, 
                   MPI_CHAR, MPI_COMM_WORLD);
        
        printf("Proces %d: Rozpakowałem strukturę (id=%d, value=%.2f, name=%s)\n",
               rank, recv_data.id, recv_data.value, recv_data.name);
    }
    
    MPI_Finalize();
    return 0;
}
