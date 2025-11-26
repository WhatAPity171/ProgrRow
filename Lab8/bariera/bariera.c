#include <stdio.h>
#include <pthread.h>


static int liczba_watkow_calkowita = 0; 
static int liczba_watkow_na_barierze = 0; 
static int generacja = 0; 
static pthread_mutex_t bariera_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t bariera_var_cond = PTHREAD_COND_INITIALIZER;

void bariera_init(int liczba_w) {
    if (liczba_w <= 0) {
        fprintf(stderr, "Blad: Liczba watkow musi byc wieksza niz 0.\n");
        return;
    }
    liczba_watkow_calkowita = liczba_w;
    liczba_watkow_na_barierze = 0;
    printf("Bariera zainicjalizowana dla %d watkow.\n", liczba_w);
}


void bariera(void) {
    pthread_mutex_lock(&bariera_mutex);

    int moja_generacja = generacja;

    liczba_watkow_na_barierze++;

    if (liczba_watkow_na_barierze == liczba_watkow_calkowita) {
        
        printf("Watek %lu: OSTATNI WATEK! Otwieram bariere generacji %d.\n", 
               (unsigned long)pthread_self(), generacja);

        generacja++;
        liczba_watkow_na_barierze = 0; 

        pthread_cond_broadcast(&bariera_var_cond);
        
    } else {
        while (moja_generacja == generacja) {
             pthread_cond_wait(&bariera_var_cond, &bariera_mutex);
        }
    }
    pthread_mutex_unlock(&bariera_mutex);
}