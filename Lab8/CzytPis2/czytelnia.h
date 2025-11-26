
#ifndef _czytelnia_
#define _czytelnia_

#include <pthread.h>


typedef struct {

  pthread_rwlock_t rw_lock;
  

  int l_p;
  int l_c;

  pthread_mutex_t licznik_mutex;

} cz_t;


void inicjuj(cz_t* czytelnia_p);
void czytam(cz_t* czytelnia_p);
void pisze(cz_t* czytelnia_p);

int my_read_lock_lock(cz_t* czytelnia_p);
int my_read_lock_unlock(cz_t* czytelnia_p);
int my_write_lock_lock(cz_t* czytelnia_p);
int my_write_lock_unlock(cz_t* czytelnia_p);

#endif