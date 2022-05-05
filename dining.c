#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int garfos[5] = {1, 1, 1, 1, 1};

void * filosofo(void *);

int main() {
    int i, a[5];
    pthread_t tid[5];
    
    for(i = 0; i < 5; i++) {
      a[i] = i;
      pthread_create(&tid[i],NULL,filosofo,(void *)&a[i]);
    }

    for(i=0;i<5;i++) {
      pthread_join(tid[i],NULL);
    }
}

void pegar_garfo_esperando(int * tem_garfo, int idx) {
  while(*tem_garfo == 0) {
    if(garfos[idx] == 1) {
      garfos[idx] = 0;
      *tem_garfo = 1;
    }  
  }
}

void comer(int num) {
  
  int tem_garfo_esquerdo = 0;
  int tem_garfo_direito = 0;

  int garfo_esquerdo_idx = num;
  int garfo_direito_idx = (num+1) % 5;

  printf("Filosofo %d quer começar a comer\n", num);
  
  if(num % 2 == 0) {

    pegar_garfo_esperando(&tem_garfo_esquerdo, garfo_esquerdo_idx);
    printf("Filosofo %d pegou o garfo esquerdo\n", num);
    pegar_garfo_esperando(&tem_garfo_direito, garfo_direito_idx);
    printf("Filosofo %d pegou o garfo direito\n", num);

  } else { 

    pegar_garfo_esperando(&tem_garfo_direito, garfo_direito_idx);
    printf("Filosofo %d pegou o garfo direito\n", num);
    pegar_garfo_esperando(&tem_garfo_esquerdo, garfo_esquerdo_idx);
    printf("Filosofo %d pegou o garfo esquerdo\n", num);
    
  }

  printf("Filosofo %d está comendo\n", num);
  sleep(1);
  
  printf("Filosofo %d terminou de comer, devolvendo os garfos\n", num);
  garfos[garfo_esquerdo_idx] = 1;
  garfos[garfo_direito_idx] = 1;
  
}

void pensar(int num) {
  sleep(2);
}

void * filosofo(void * arg) {
  
  int num = *(int *)arg;

  while(1) {
    comer(num);
    pensar(num);
  }
  
}

