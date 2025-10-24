#include<stdlib.h>
#include<stdio.h>
#include<time.h>

#include"pomiar_czasu.h"
 
const int liczba = 100000;

int main(void){
  double a, b, c;
  int i,j,k,l;

  k = -50000;
  #ifdef POM
  double t1 = czas_zegara(), t2 = czas_CPU();
  #endif
  for(i=0;i<liczba;i++){

    printf("%d ",k+i);

  }
  printf("\n");
  #ifdef POM
  t1=czas_zegara()-t1;
  t2 = czas_CPU()- t2;
  printf("Czas wykonania %d operacji wejscia/wyjscia: \nZegarowy= %lf \nCPU=      %lf\n",liczba, t1, t2);

  inicjuj_czas();
  #endif
  a = 1.000001;
  for(i=0;i<liczba;i++){

    a = 1.000001*a+0.000001; 

  }
  #ifdef POM
  printf("Wynik operacji arytmetycznych: %lf\n", a);
  printf("Czas wykonania %d operacji arytmetycznych: \n",liczba);
  drukuj_czas();
  #endif
}
