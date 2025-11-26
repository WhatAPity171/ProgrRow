
#ifndef _czytelnia_
#define _czytelnia_

#include <pthread.h>

typedef struct {
  int l_p; // liczba piszacych
  int l_c; // liczba czytajacych
  
  pthread_mutex_t mutex;
  pthread_cond_t czytelnicy_czekaja; 
  pthread_cond_t pisarze_czekaja;    
  int czytelnicy_czekajacy;          // Liczba czytelników czekających (Wymagane do zamienienia empty())
  int pisarze_czekajacy;             // Liczba pisarzy czekających

} cz_t;


/*** Deklaracje procedur interfejsu ***/
void inicjuj(cz_t* czytelnia_p);
void czytam(cz_t* czytelnia_p);
void pisze(cz_t* czytelnia_p);

int my_read_lock_lock(cz_t* czytelnia_p);
int my_read_lock_unlock(cz_t* czytelnia_p);
int my_write_lock_lock(cz_t* czytelnia_p);
int my_write_lock_unlock(cz_t* czytelnia_p);

#endif
