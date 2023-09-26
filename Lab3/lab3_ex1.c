#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 3
#define TAM_MAX 30
#define TAM_PARTES TAM_MAX / NUM_THREADS

void *operaVet(void *threadid);

typedef struct dadosVet{
    int valoresVet[TAM_MAX];
    int somaProcesso[NUM_THREADS];
}DadosVet;

DadosVet vetorGeral;

int main(void){
    pthread_t threads[NUM_THREADS];
    int i = 0;
    
    for(i=0; i<TAM_MAX;i++){
        vetorGeral.valoresVet[i] = 5;
    }
    
    for(i = 0; i<NUM_THREADS; i++){

        pthread_create(&threads[i], NULL, operaVet,(void*)i);
    }
    for(i = 0; i<NUM_THREADS;i++){
        pthread_join(threads[i], NULL);
    }
    

    for(i = 0; i<NUM_THREADS; i++){
        printf("Thread %d com soma: %d\n", i, vetorGeral.somaProcesso[i]);
    }
    return 0; 
}

void *operaVet(void *threadid){
    
    for(int i = 0; i<NUM_THREADS; i++){
        int comeco = i*TAM_PARTES;
        int fim = (i+1) * TAM_PARTES;
        int somaAux = 0;

        for(int j = comeco; j < fim; j++){
            vetorGeral.valoresVet[j]*=2;
            somaAux+=vetorGeral.valoresVet[j];
        }
        vetorGeral.somaProcesso[i] = somaAux;
        somaAux = 0;        
    }
}
