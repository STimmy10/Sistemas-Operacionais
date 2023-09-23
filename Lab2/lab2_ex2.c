#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_TRAB 10
#define TAM_MAX 10

int opera(int a){
    return (a*2) + 2;
}

int main(int argc, char* argv[]) {
    int status = 0;
    int shmid;
    int *vetCompartilhado;
    pid_t worker1, worker2;
    int resultado;

    // Criação da memória compartilhada
    shmid = shmget(IPC_PRIVATE, TAM_MAX * sizeof(int), IPC_CREAT);
    if (shmid == -1) {
        perror("Erro ao criar a memória compartilhada");
        exit(1);
    }

    // Associação da memória compartilhada ao espaço de endereço do processo
    vetCompartilhado = (int*)shmat(shmid, NULL, 0);
    if (vetCompartilhado == (int*)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    // Inicialização do vetor compartilhado
    for(int i = 0; i < TAM_MAX; i++){
        vetCompartilhado[i] = 5;
    }

    worker1 = fork();

    if (worker1 == 0) {
        // Este é o processo filho 1
        printf("entrei no filho 1 \n");
        for (int i = 0; i < TAM_MAX; i++) {
            resultado = opera(vetCompartilhado[i]);
            printf("resultado: %d \n", resultado);
            vetCompartilhado[i] = resultado;
        }
        exit(0);
    } else {
        worker2 = fork();

        if (worker2 == 0) {
            // Este é o processo filho 2
            printf("entrei no filho 2 \n");
            for (int i = 0; i < TAM_MAX; i++) {
                resultado = opera(vetCompartilhado[i]);
                printf("resultado: %d \n", resultado);
                vetCompartilhado[i] = resultado;
            }
            exit(0);
        } else {
            // Este é o processo pai
            waitpid(worker1, &status, 0);
            waitpid(worker2, &status, 0);
            printf("entrei no pai \n");
            for (int i = 0; i < TAM_MAX; i++) {
                printf("%d \n", vetCompartilhado[i]);
            }

            // Desassociação da memória compartilhada
            shmdt(vetCompartilhado);
        }
    }

    return 0;
}
