#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define TAM_MAX 900000



int main(void){
    int vetor[TAM_MAX];
    long int somaVet = 0;

    //Estruturas para medir tempo de execução
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    for(int i = 0; i< TAM_MAX ; i++ ){
        vetor[i] = 5;
    }
    for(int k = 0; k <TAM_MAX; k++){
        somaVet+= 2*vetor[k];
    }


    gettimeofday(&end_time, NULL);
    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("Soma Total: %d\n", somaVet);
    printf("Tempo de execução: %.6f seg\n", execution_time);

    return 0;

}