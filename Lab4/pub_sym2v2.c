#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <sched.h>

#define ILE_MUSZE_WYPIC 3

struct struktura_t {
    int l_wkf;                 // liczba wolnych kufli
    pthread_mutex_t *tab_kuf;  // tablica mutexów dla kufli
    int l_kr;                  // liczba kranów
    pthread_mutex_t *tab_kran; // tablica mutexów dla kranów
};
struct struktura_t pub_wsk;

void *watek_klient(void *arg);

int main(void) {
    pthread_t *tab_klient;
    int *tab_klient_id;

    int l_kl, l_kf, l_kr, i;

    printf("\nLiczba klientow: ");
    scanf("%d", &l_kl);

    printf("\nLiczba kufli: ");
    scanf("%d", &l_kf);
    pub_wsk.l_wkf = l_kf;

    printf("\nLiczba kranow: ");
    scanf("%d", &l_kr);
    pub_wsk.l_kr = l_kr;

    pub_wsk.tab_kuf = malloc(l_kf * sizeof(pthread_mutex_t));
    for (i = 0; i < l_kf; i++)
        pthread_mutex_init(&pub_wsk.tab_kuf[i], NULL);

    pub_wsk.tab_kran = malloc(l_kr * sizeof(pthread_mutex_t));
    for (i = 0; i < l_kr; i++)
        pthread_mutex_init(&pub_wsk.tab_kran[i], NULL);

    tab_klient = malloc(l_kl * sizeof(pthread_t));
    tab_klient_id = malloc(l_kl * sizeof(int));
    for (i = 0; i < l_kl; i++)
        tab_klient_id[i] = i;

    printf("\nOtwieramy pub 2.0!\n");

    for (i = 0; i < l_kl; i++)
        pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]);

    for (i = 0; i < l_kl; i++)
        pthread_join(tab_klient[i], NULL);

    printf("\nZamykamy pub!\n");

    for (i = 0; i < l_kf; i++)
        pthread_mutex_destroy(&pub_wsk.tab_kuf[i]);
    for (i = 0; i < l_kr; i++)
        pthread_mutex_destroy(&pub_wsk.tab_kran[i]);

    free(pub_wsk.tab_kuf);
    free(pub_wsk.tab_kran);
    free(tab_klient);
    free(tab_klient_id);

    return 0;
}



void *watek_klient(void *arg) {
    int moj_id = *((int *)arg);
    int i_wypite, kufel = -1, kran = -1;
    long int wykonana_praca = 0;

    printf("\nKlient %d: wchodzę do pubu\n", moj_id);

    for (i_wypite = 0; i_wypite < ILE_MUSZE_WYPIC; i_wypite++) {


        int sukces = 0;
        do {
            for (int i = 0; i < pub_wsk.l_wkf; i++) {
                if (pthread_mutex_trylock(&pub_wsk.tab_kuf[i]) == 0) {
                    kufel = i;
                    sukces = 1;
                    break;
                } else {
                    wykonana_praca++;  
                    sched_yield();
                }
            }
        } while (!sukces);

        printf("Klient %d: wziąłem kufel %d\n", moj_id, kufel);

        sukces = 0;
        do {
            for (int j = 0; j < pub_wsk.l_kr; j++) {
                if (pthread_mutex_trylock(&pub_wsk.tab_kran[j]) == 0) {
                    kran = j;
                    sukces = 1;
                    break;
                } else {
                    wykonana_praca++;
                    sched_yield();
                }
            }
        } while (!sukces);

        printf("Klient %d: nalewam z kranu %d\n", moj_id, kran);
        usleep(100);

        const char *piwa[] = {"Guinness", "Żywiec", "Heineken", "Okocim", "Karlsberg"};
        const char *marka = (kran < 5) ? piwa[kran] : "inne piwo";
        printf("Klient %d: piję piwo marki %s z kufla %d\n", moj_id, marka, kufel);
        nanosleep((struct timespec[]){{0, 70000000L}}, NULL);


        pthread_mutex_unlock(&pub_wsk.tab_kran[kran]);
        pthread_mutex_unlock(&pub_wsk.tab_kuf[kufel]);
        printf("Klient %d: odłożyłem kufel %d\n", moj_id, kufel);
    }

    printf("Klient %d: wychodzę z pubu, wykonana_praca = %ld\n",
           moj_id, wykonana_praca);
    return NULL;
}
