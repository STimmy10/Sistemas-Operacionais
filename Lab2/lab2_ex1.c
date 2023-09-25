#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_TRAB 10
#define TAM_MAX 10000
#define TAM_PARTES TAM_MAX / NUM_TRAB

typedef struct{
    int valorInicial[TAM_MAX];
    long somaSegmento[NUM_TRAB];
}InfoVetor;

int main(void){
    InfoVetor *vetor;
    long int somaTotal = 0;
    int segmento;

    //Estrutura para calcular tempo de execução
    struct timeval start_time, end_time;
    gettimeofday(&start_time, NULL);

    //Memorial Compartilhada
    segmento = shmget(IPC_PRIVATE, sizeof(InfoVetor), IPC_CREAT | 0666);
    if (segmento == -1) {
        puts("Erro ao criar a memória compartilhada1");
        exit(1);
    }

    vetor = (InfoVetor*)shmat(segmento, 0, 0);
    if (vetor ==(InfoVetor*) -1) {
        puts("Erro ao criar a memória compartilhada2");
        exit(1);
    }

    for(int i =0; i<TAM_MAX; i++){
        vetor->valorInicial[i] = 5;
    }

    for(int j = 0; j < NUM_TRAB; j++){
        pid_t pid = fork();
    
        if(pid == 0){
            int comeco = j*TAM_PARTES;
            int fim = (j+1) * TAM_PARTES;
            int somaAux = 0;

            for(int a = comeco; a < fim; a++){
                vetor->valorInicial[a] *= 2;
                somaAux += vetor->valorInicial[a];
            }
                vetor->somaSegmento[j] = somaAux;

            shmdt(vetor);
            exit(0);
        }else if(pid == -1){
            puts("Erro ao criar a memória compartilhada2");
            exit(1);
        }
    }
    for (int i = 0; i < NUM_TRAB; i++) {
        wait(NULL);
    }
    for(int l = 0; l < NUM_TRAB; l++){
        somaTotal+= vetor->somaSegmento[l];

    }

    gettimeofday(&end_time, NULL);
    double execution_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    int numTrab = NUM_TRAB;
    int tamVet = TAM_MAX;

    printf("Número de Trabalhadores: %d\n", numTrab);
    printf("Tamanho Vetor: %d\n", tamVet);
    printf("Tempo de execução: %.8f seg\n", execution_time);
    printf("Soma total: %d\n", somaTotal);

    // Libera a memória compartilhada
    shmctl(segmento, IPC_RMID, NULL);
    return 0;
}
