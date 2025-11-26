#include <stdio.h>
#include <unistd.h>
#include "bariera.h"



void bariera_init(bariera_t *b_ptr, int count) {
    if (count <= 0) {
        fprintf(stderr, "Blad: Liczba watkow musi byc wieksza niz 0.\n");
        return;
    }
    b_ptr->total_threads = count;
    b_ptr->current_wait_count = 0;
    b_ptr->generation = 0;
    
    pthread_mutex_init(&b_ptr->mutex, NULL);
    pthread_cond_init(&b_ptr->condition, NULL);

    printf("Instancja bariery zainicjalizowana dla %d watkow.\n", count);
}



void bariera(bariera_t *b_ptr) {
    pthread_mutex_lock(&b_ptr->mutex);

    int moja_generacja = b_ptr->generation;

    b_ptr->current_wait_count++;

    if (b_ptr->current_wait_count == b_ptr->total_threads) {

        b_ptr->generation++;
        
        b_ptr->current_wait_count = 0; 
        
        pthread_cond_broadcast(&b_ptr->condition);
        
    } else {

        while (moja_generacja == b_ptr->generation) {
             pthread_cond_wait(&b_ptr->condition, &b_ptr->mutex);
        }
    }

    pthread_mutex_unlock(&b_ptr->mutex);
}


void bariera_destroy(bariera_t *b_ptr) {
    pthread_mutex_destroy(&b_ptr->mutex);
    pthread_cond_destroy(&b_ptr->condition);
}