#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>

#include"czytelnia.h"

int my_read_lock_lock(cz_t* cz_p){
    pthread_rwlock_rdlock(&cz_p->rw_lock);

    pthread_mutex_lock(&cz_p->licznik_mutex);
    cz_p->l_c++;
    pthread_mutex_unlock(&cz_p->licznik_mutex);
    
    return 0; 
}

int my_read_lock_unlock(cz_t* cz_p){

    pthread_mutex_lock(&cz_p->licznik_mutex);
    cz_p->l_c--; 
    pthread_mutex_unlock(&cz_p->licznik_mutex);
    
    pthread_rwlock_unlock(&cz_p->rw_lock);

    return 0;
}


int my_write_lock_lock(cz_t* cz_p){
    pthread_rwlock_wrlock(&cz_p->rw_lock);
    

    pthread_mutex_lock(&cz_p->licznik_mutex);
    cz_p->l_p++; 
    pthread_mutex_unlock(&cz_p->licznik_mutex);
    
    return 0;
}

int my_write_lock_unlock(cz_t* cz_p){
    pthread_mutex_lock(&cz_p->licznik_mutex);
    cz_p->l_p--; 
    pthread_mutex_unlock(&cz_p->licznik_mutex);
  

    pthread_rwlock_unlock(&cz_p->rw_lock);
    
    return 0;
}

void inicjuj(cz_t* cz_p){

  cz_p->l_p = 0;
  cz_p->l_c = 0;  
  
  pthread_rwlock_init(&cz_p->rw_lock, NULL);
  pthread_mutex_init(&cz_p->licznik_mutex, NULL);
}



void czytam(cz_t* cz_p){

    pthread_mutex_lock(&cz_p->licznik_mutex); 

    #ifdef MY_DEBUG
    printf("\t\t\t\t\t[CZYTAM] l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p); 
    #endif

    if( cz_p->l_p > 0 || cz_p->l_p < 0 || cz_p->l_c <= 0 ) {
      fprintf(stderr, "BLAD KRYTYCZNY (CZYTELNIK)! l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p);
      exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&cz_p->licznik_mutex);
    
    usleep(rand()%10000); 
}

void pisze(cz_t* cz_p){

    pthread_mutex_lock(&cz_p->licznik_mutex); 

    #ifdef MY_DEBUG
    printf("\t\t\t\t\t[PISZE] l_c: %d, l_p: %d\n", cz_p->l_p, cz_p->l_c); 
    #endif


    if( cz_p->l_p != 1 || cz_p->l_c > 0 || cz_p->l_p < 0 || cz_p->l_c < 0 ) {
      fprintf(stderr, "BLAD KRYTYCZNY (PISARZ)! l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p);
      exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&cz_p->licznik_mutex);
    
    usleep(rand()%20000);
}