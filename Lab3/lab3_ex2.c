#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define TAM_MAX 100
#define NUM_THREADS 2
#define TAM_PARTES TAM_MAX / NUM_THREADS

int vetorGeral[TAM_MAX];

void *operaVet(void *threadID){
    int thread_id = *((int *)threadID);
    int comeco  = thread_id * TAM_PARTES;
    int fim = comeco + TAM_MAX;

    for(int i = comeco; i < fim; i++){
        vetorGeral[i] *= 2;
        vetorGeral[i] += 2;
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

    
    
    for(i = 0; i < TAM_MAX; i++){
        if(vetorGeral[i] != vetorGeral[(TAM_MAX/2)+i]){
            printf("Posicao %d com valor %d diferente do esperado.\n", i, vetorGeral[i]);
            //break;
        }
    }
    

    return 0;
}