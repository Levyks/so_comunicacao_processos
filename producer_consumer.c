#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h> 

struct Fila* fila;
pthread_t consumidor_tid, produtor_tid;

pthread_mutex_t consumidor_mutex, produtor_mutex;
pthread_cond_t consumidor_cond, produtor_cond;

struct Fila {
    int frente, final, tamanho;
    unsigned capacidade;
    int* array;
};

struct Fila* criar_fila(unsigned capacidade) {

    struct Fila* fila = (struct Fila*) malloc(sizeof(struct Fila));
    
    fila->capacidade = capacidade;
    fila->tamanho = 0;
    fila->frente = 0;
    fila->final = fila->capacidade - 1;
    
    fila->array = (int*) malloc(capacidade * sizeof(int));

    return fila;

}

int fila_vazia(struct Fila* fila) {
    return fila->tamanho == 0;
}

int fila_cheia(struct Fila* fila) {
    return fila->tamanho == fila->capacidade;
}

int colocar_fila(struct Fila* fila, int elemento) {

    if(fila_cheia(fila)) {
        return 0;
    }

    fila->final = (fila->final + 1) % fila->capacidade;
    fila->array[fila->final] = elemento;
    fila->tamanho++;

    return 1;

}

int retirar_fila(struct Fila* fila) {

    if(fila_vazia(fila)) {
        return INT_MIN;
    }

    int elemento = fila->array[fila->frente];
    
    fila->frente = (fila->frente + 1) % fila->capacidade;
    fila->tamanho--;

    return elemento;

}

int frente_fila(struct Fila* fila) {
    if(fila_vazia(fila)) {
        return INT_MIN;
    }
    return fila->array[fila->frente];
}

int final_fila(struct Fila* fila) {
    if(fila_vazia(fila)) {
        return INT_MIN;
    }
    return fila->array[fila->final];
}

void * consumidor(void * arg) {

    while(1) {

        if(fila_vazia(fila)) {
            printf("Fila vazia, o consumidor vai dormir\n");
            
            pthread_mutex_lock(&consumidor_mutex);
            while (fila_vazia(fila)) {
                pthread_cond_wait(&consumidor_cond, &consumidor_mutex);
            }
            pthread_mutex_unlock(&consumidor_mutex);
        }

        pthread_mutex_lock(&produtor_mutex);

        int num = retirar_fila(fila);
        printf("Consumidor retirou o n° %d, n° de elementos restantes: %d\n", num, fila->tamanho);

        if(fila->tamanho == (fila->capacidade - 1)) {
            printf("Acordando o produtor\n");
            pthread_cond_signal(&produtor_cond);
        }

        pthread_mutex_unlock(&produtor_mutex);
        
        sleep(1);

    }

}

void * produtor(void * arg) {

    while(1) {

        if(fila_cheia(fila)) {
            printf("Fila cheia, o produtor vai dormir\n");
            
            pthread_mutex_lock(&produtor_mutex);
            while (fila_cheia(fila)) {
                pthread_cond_wait(&produtor_cond, &produtor_mutex);
            }
            pthread_mutex_unlock(&produtor_mutex);
        }

        int num = rand() % 5;


        pthread_mutex_lock(&consumidor_mutex);

        colocar_fila(fila, num);

        printf("Produtor colocou o n° %d na fila, n° de elementos na fila: %d\n", num, fila->tamanho);

        if(fila->tamanho == 1) {
            printf("Acordando o consumidor\n");
            pthread_cond_signal(&consumidor_cond);
        }
        
        pthread_mutex_unlock(&consumidor_mutex);

        /**
         * Usando 5 aqui para fazer com que o ritmo de consumo seja maior que o de produção
         * fazendo com que o consumidor fique ocioso as vezes, você pode alterar para algo
         * menor caso queira o comportamento inverso 
         */
        sleep(5);

    }

}

int main() {

    fila = criar_fila(5);

    pthread_create(&produtor_tid,NULL,produtor, NULL);
    pthread_create(&consumidor_tid,NULL,consumidor, NULL);

    pthread_join(produtor_tid, NULL);
    pthread_join(consumidor_tid, NULL);

    return 0;

}