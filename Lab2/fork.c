#define _GNU_SOURCE
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <linux/sched.h>


#include "pomiar_czasu.h"


int zmienna_globalna=0;

#define ROZMIAR_STOSU 1024*64

int funkcja_watku( void* argument )
{

  zmienna_globalna++;

  int wynik;
  char* arg[] = {"./a.out","Tak", "novak" ,NULL};
  wynik=execv(arg[0],arg);
  if(wynik==-1)
     printf("Proces potomny nie wykonal programu\n");

  return 0;
}
int funkcja_watku2(void* argument){
  return 0;
}

int main()
{
  int zmienna = 0;
  pid_t pidFork = fork();
  if (pidFork == 0) {
        zmienna = 5;
        printf("Potomek: zmienna = %d\n", zmienna);
        exit(0);
    } else {
        wait(NULL);
        printf("Rodzic: zmienna = %d\n", zmienna);
    }
    return 0;




  //WhatAPity171
  void *stos;
  pid_t pid;
  int i; 
  
  stos = malloc( ROZMIAR_STOSU );
  if (stos == 0) {
    printf("Proces nadrzędny - blad alokacji stosu\n");
    exit( 1 );
  }
  inicjuj_czas();
  for(i=0;i<1000;i++){

    //pid = clone( &funkcja_watku, (void *) stos+ROZMIAR_STOSU, 
    // CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0 );
  pid = clone( &funkcja_watku, (void *) stos+ROZMIAR_STOSU, SIGCHLD, 0);

    waitpid(pid, NULL, __WCLONE);

  }

  free( stos );
  drukuj_czas();
}