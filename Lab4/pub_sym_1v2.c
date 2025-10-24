#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>   // sched_yield()

#define ILE_MUSZE_WYPIC 3

pthread_mutex_t lock;
int l_kf = 0;  


void * watek_klient(void * arg_wsk) {
    int moj_id = *((int *)arg_wsk);
    int i, j;
    int ile_musze_wypic = ILE_MUSZE_WYPIC;
    long int wykonana_praca = 0;
    printf("Klient %d: wchodzę do pubu\n", moj_id);

    for (i = 0; i < ile_musze_wypic; ++i) {
        int sukces = 0;

        do {
            if (pthread_mutex_trylock(&lock) == 0) {
                
                if (l_kf > 0) {
                    
                    --l_kf;
                    sukces = 1;
                    
                    pthread_mutex_unlock(&lock);
                    break;
                } else {
                    pthread_mutex_unlock(&lock);
                    
                    wykonana_praca++;
                    
                    sched_yield();
                }
            } else {
                
                wykonana_praca++;
                
                sched_yield();//oddaje CPU innym wątkom
            }
            
        } while (!sukces);

        printf("Klient %d: wybieram kufel (iter %d)\n", moj_id, i);
        j = 0;
        printf("Klient %d: nalewam z kranu %d\n", moj_id, j);
        usleep(30); 

        printf("Klient %d: pije\n", moj_id);
        nanosleep((struct timespec[]){{0, 50000000L}}, NULL); 

        printf("Klient %d: odkładam kufel\n", moj_id);


        pthread_mutex_lock(&lock);
        ++l_kf;
        pthread_mutex_unlock(&lock);

    }

    printf("Klient %d: wychodzę; wykonana_praca = %ld\n", moj_id, wykonana_praca);
    return NULL;
}

int main(void) {
    pthread_t *tab_klient;
    int *tab_klient_id;
    int l_kl;
    int i;

    if (pthread_mutex_init(&lock, NULL) != 0) {
        fprintf(stderr, "Mutex init failed\n");
        return 1;
    }

    printf("Liczba klientow: "); if (scanf("%d", &l_kl) != 1) return 1;
    printf("Liczba kufli: "); if (scanf("%d", &l_kf) != 1) return 1;

    tab_klient = malloc(sizeof(pthread_t) * l_kl);
    tab_klient_id = malloc(sizeof(int) * l_kl);
    for (i = 0; i < l_kl; ++i) tab_klient_id[i] = i;

    for (i = 0; i < l_kl; ++i) {
        pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]);
    }
    for (i = 0; i < l_kl; ++i) {
        pthread_join(tab_klient[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    free(tab_klient);
    free(tab_klient_id);

    printf("Zamykamy pub; kufli na stanie: %d\n", l_kf);
    return 0;
}
