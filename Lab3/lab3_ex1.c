#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TAM_MAX 50000
#define NUM_THREADS 100
#define TAM_PARTES TAM_MAX / NUM_THREADS

int vetorGeral[TAM_MAX];
int vetorSeqGeral[TAM_MAX];

void *operaVet(void *threadID){
    int thread_id = *((int *)threadID);
    int comeco = thread_id * (TAM_PARTES);
    int fim = comeco + (TAM_PARTES);
    int somaTemp = 0;

    for(int i = comeco; i < fim; i++){
        vetorGeral[i] *= 2;
        somaTemp += vetorGeral[i];
    }
    
    return (void *)somaTemp;
}

int main(void){

    for(int i = 0; i < TAM_MAX; i++){
        vetorGeral[i] = 5;
    }
    
    pthread_t threads[NUM_THREADS];
    int thread_id[NUM_THREADS];
    int somaTotal,somaSeqTotal = 0;
    int i;
    clock_t start_time = clock();

    for(i = 0; i < NUM_THREADS; i++){
        thread_id[i] = i;
        pthread_create(&threads[i], NULL, operaVet, &thread_id[i]);
    }

    for(i = 0; i < NUM_THREADS; i++){
        int somaAux = 0;
        pthread_join(threads[i], (void**)&somaAux);
        somaTotal += somaAux;
    }

    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Paralelo\nTamanho Vetor:%d\nNúmero Threads:%d\nSoma Total: %d - Tempo de Execução: %f segundos\n",TAM_MAX, NUM_THREADS, somaTotal, execution_time);

    start_time = clock();

    for(i = 0; i < TAM_MAX; i++){
        vetorSeqGeral[i] = 5;
        vetorSeqGeral[i] *= 2;
        somaSeqTotal += vetorSeqGeral[i];
    }

    end_time = clock();
    execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Sequencial\nTamanhoVetor:%d\nSoma Total: %d - Tempo de Execução: %f segundos\n",TAM_MAX, somaSeqTotal, execution_time);

    return 0; 

}
