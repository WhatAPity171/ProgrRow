#include<stdlib.h>
#include<stdio.h>
#include <time.h>
#include<pthread.h>
#include <unistd.h>

#define ILE_MUSZE_WYPIC 3

pthread_mutex_t lock;
pthread_cond_t cond;//to replace busy waiting and empty loop
int l_kf = 0;


void * watek_klient (void * arg);

int main( void ){

  pthread_t *tab_klient;
  int *tab_klient_id;

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&cond, NULL);
  


  int l_kl, l_kr, i;

  printf("\nLiczba klientow: "); scanf("%d", &l_kl);

  printf("\nLiczba kufli: "); scanf("%d", &l_kf);

  //printf("\nLiczba kranow: "); scanf("%d", &l_kr);
  l_kr = 1000000000; // wystarczająco dużo, żeby nie było rywalizacji 

  tab_klient = (pthread_t *) malloc(l_kl*sizeof(pthread_t));
  tab_klient_id = (int *) malloc(l_kl*sizeof(int));
  for(i=0;i<l_kl;i++) tab_klient_id[i]=i;


  printf("\nOtwieramy pub (simple)!\n");
  printf("\nLiczba wolnych kufli %d\n", l_kf); 

  for(i=0;i<l_kl;i++){
    pthread_create(&tab_klient[i], NULL, watek_klient, &tab_klient_id[i]); 
  }
  for(i=0;i<l_kl;i++){
    pthread_join( tab_klient[i], NULL);
  }
  printf("\nZamykamy pub! liczba kufli: %d\n", l_kf);
  pthread_cond_destroy(&cond);
  pthread_mutex_destroy(&lock);


}


void * watek_klient(void *arg_wsk) {
    int moj_id = *((int *)arg_wsk);
    int i, j;
    int ile_musze_wypic = ILE_MUSZE_WYPIC;
    long int wykonana_praca = 0;

    printf("\nKlient %d, wchodzę do pubu\n", moj_id);

    for (i = 0; i < ile_musze_wypic; i++) {
        pthread_mutex_lock(&lock);

        // Wait while no mugs available
        while (l_kf <= 0) {
            pthread_cond_wait(&cond, &lock);
        }

        // Take a mug
        l_kf--;
        pthread_mutex_unlock(&lock);

        printf("\nKlient %d, wybieram kufel\n", moj_id);
        j = 0;

        printf("\nKlient %d, nalewam z kranu %d\n", moj_id, j);
        usleep(30);

        printf("\nKlient %d, pije\n", moj_id);
        nanosleep((struct timespec[]){{0, 50000000L}}, NULL);

        printf("\nKlient %d, odkladam kufel\n", moj_id);

        pthread_mutex_lock(&lock);
        l_kf++;
        pthread_mutex_unlock(&lock);

        // Wake one waiting thread (if any)
        pthread_cond_signal(&cond);
    }

    printf("\nKlient %d, wychodzę z pubu; wykonana praca %ld\n",
           moj_id, wykonana_praca);

    return NULL;
}



