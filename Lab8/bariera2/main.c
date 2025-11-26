#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include "bariera.h" 

#define LICZBA_W 4

bariera_t bariera1;
bariera_t bariera2;

pthread_t watki[LICZBA_W];

typedef struct {
    int id;
    bariera_t *b1;
    bariera_t *b2;
} thread_args_t;

void *cokolwiek( void *arg);

int main( int argc, char *argv[] ){

  thread_args_t args[LICZBA_W]; 

  bariera_init(&bariera1, LICZBA_W);
  bariera_init(&bariera2, LICZBA_W);

  for(int i=0; i<LICZBA_W; i++ ) {
    args[i].id = i;
    args[i].b1 = &bariera1;
    args[i].b2 = &bariera2;
    pthread_create( &watki[i], NULL, cokolwiek, (void *) &args[i] );
  }

  for(int i=0; i<LICZBA_W; i++ ) pthread_join( watki[i], NULL );


  return 0;
}


void *cokolwiek( void *arg){

  thread_args_t *args = (thread_args_t *) arg;
  int moj_id = args->id; 

  printf("przed bariera 1 (instancja A) - watek %d\n", moj_id);

  bariera(args->b1); // Użycie instancji bariera1

  printf("po bariera 1, przed bariera 2 (instancja B) - watek %d\n", moj_id);

  bariera(args->b2); // Użycie instancji bariera2

  printf("po ostatniej barierze - watek %d\n", moj_id);  

  pthread_exit( (void *)0);
}