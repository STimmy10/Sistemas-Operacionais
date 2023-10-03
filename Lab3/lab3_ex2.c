#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TAM_MAX 10000
#define NUM_THREADS 2
#define TAM_PARTES TAM_MAX / NUM_THREADS

int vetorGeral[TAM_MAX];

void *operaVet(void *threadID){
    int thread_id = *((int *)threadID);
    int comeco = thread_id * TAM_PARTES;
    int fim = comeco + TAM_PARTES;

    for(int i = comeco; i < fim; i++){
        vetorGeral[i] = 2*vetorGeral[i] + 2;
    }
    return NULL;
}

int main(void){
    for(int i = 0; i < TAM_MAX; i++){
        vetorGeral[i] = 5;
    }
    
    pthread_t threads[NUM_THREADS];
    int thread_id[NUM_THREADS];
    int i;
    
    for(i = 0; i < NUM_THREADS; i++){
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, operaVet, &thread_id[i]);
    }

    for(i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    int igual = 1;  // Assumimos que todos os valores são iguais
    

    for (i = 0; i < TAM_PARTES; i++) {
        if (vetorGeral[i] != vetorGeral[i + TAM_PARTES]) {
            igual = 0;  // Se encontrarmos um valor diferente, definimos igual como 0
            break;      // Saímos do loop
        }
    }
    if (igual) {
        printf("Todos os valores do vetor são iguais.\n");
    } else {
        printf("Pelo menos um valor do vetor é diferente.\n");
    }
    

    return 0;
}