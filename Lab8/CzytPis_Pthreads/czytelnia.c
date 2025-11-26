#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include"czytelnia.h"

void signal_pisarzy(cz_t* cz_p) {
    if (cz_p->pisarze_czekajacy > 0) {
        pthread_cond_signal(&cz_p->pisarze_czekaja);
    }
}

void signal_czytelnikow(cz_t* cz_p) {
    if (cz_p->czytelnicy_czekajacy > 0) {
        pthread_cond_broadcast(&cz_p->czytelnicy_czekaja);
    }
}

int my_read_lock_lock(cz_t* cz_p){
    pthread_mutex_lock(&cz_p->mutex);
    
    while (cz_p->l_p > 0 || cz_p->pisarze_czekajacy > 0) {
        cz_p->czytelnicy_czekajacy++;
        pthread_cond_wait(&cz_p->czytelnicy_czekaja, &cz_p->mutex);
        cz_p->czytelnicy_czekajacy--;
    }
    
    cz_p->l_c++;

    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}


int my_read_lock_unlock(cz_t* cz_p){
    pthread_mutex_lock(&cz_p->mutex);
   
    cz_p->l_c--;

    if (cz_p->l_c == 0) {
        signal_pisarzy(cz_p);
    }

    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}


int my_write_lock_lock(cz_t* cz_p){
    pthread_mutex_lock(&cz_p->mutex);

    while (cz_p->l_c > 0 || cz_p->l_p > 0) {
        cz_p->pisarze_czekajacy++;
        pthread_cond_wait(&cz_p->pisarze_czekaja, &cz_p->mutex);
        cz_p->pisarze_czekajacy--;
    }

    cz_p->l_p = 1;

    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}

int my_write_lock_unlock(cz_t* cz_p){
    pthread_mutex_lock(&cz_p->mutex);
    
    cz_p->l_p = 0; 
    
    if (cz_p->pisarze_czekajacy > 0) {
        signal_pisarzy(cz_p);
    } else {
        signal_czytelnikow(cz_p);
    }
  
    pthread_mutex_unlock(&cz_p->mutex);
    return 0;
}


void inicjuj(cz_t* cz_p){

  cz_p->l_p = 0;
  cz_p->l_c = 0;  
  
  pthread_mutex_init(&cz_p->mutex, NULL);
  pthread_cond_init(&cz_p->czytelnicy_czekaja, NULL);
  pthread_cond_init(&cz_p->pisarze_czekaja, NULL);
  cz_p->czytelnicy_czekajacy = 0;
  cz_p->pisarze_czekajacy = 0;
}

void czytam(cz_t* cz_p){

    pthread_mutex_lock(&cz_p->mutex);

    #ifdef MY_DEBUG
    printf("\t\t\t\t\t[CZYTAM] l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p); 
    #endif

    if( cz_p->l_p > 1 || (cz_p->l_p == 1 && cz_p->l_c >= 1) || cz_p->l_p < 0 || cz_p->l_c <= 0 ) {
      fprintf(stderr, "BLAD KRYTYCZNY (CZYTELNIK)! l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p);
      exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&cz_p->mutex);
    
    usleep(rand()%10000); 
}

void pisze(cz_t* cz_p){

    pthread_mutex_lock(&cz_p->mutex);

    #ifdef MY_DEBUG
    printf("\t\t\t\t\t[PISZE] l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p); 
    #endif

    if( cz_p->l_p != 1 || cz_p->l_c > 0 || cz_p->l_p < 0 || cz_p->l_c < 0 ) {
      fprintf(stderr, "BLAD KRYTYCZNY (PISARZ)! l_c: %d, l_p: %d\n", cz_p->l_c, cz_p->l_p);
      exit(EXIT_FAILURE);
    }
    
    pthread_mutex_unlock(&cz_p->mutex);
    
    usleep(rand()%20000);
}