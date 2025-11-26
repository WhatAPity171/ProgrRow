#ifndef __BARIERA_H__
#define __BARIERA_H__

#include <pthread.h>


typedef struct {
    int total_threads;      
    int current_wait_count; 
    int generation;         
    pthread_mutex_t mutex;
    pthread_cond_t condition;
} bariera_t;



void bariera_init(bariera_t *b_ptr, int count); 


void bariera(bariera_t *b_ptr); 

#endif