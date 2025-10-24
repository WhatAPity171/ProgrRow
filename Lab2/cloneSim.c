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

int funkcja_watku(void* argument)
{
	int* z = (int*)argument;
  zmienna_globalna++;
  (*z)++;

  return 0;
}

int main(){
	void* stos1 = malloc(ROZMIAR_STOSU);
	void* stos2 = malloc(ROZMIAR_STOSU);

	int zmienna = 0;

	pid_t pid1 = clone( &funkcja_watku, (void*) stos1+ROZMIAR_STOSU, 
		 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &zmienna);
	pid_t pid2 = clone( &funkcja_watku, (void*) stos2+ROZMIAR_STOSU, 
		 CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, &zmienna );
	waitpid(pid1, NULL, __WCLONE);
  	waitpid(pid2, NULL, __WCLONE);
  	printf("%d\n", zmienna);
  	free(stos1);
  	free(stos2);

	return 0;
}