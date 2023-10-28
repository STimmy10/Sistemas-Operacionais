#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Definir a estrutura do objeto "comando"
typedef struct {
    char* nome_programa; // Nome do programa
    int momento_inicio; // Momento de início
    int tempo_duracao; // Tempo de duração
} Comando_RealTime;

typedef struct {
    char* nome_programa; // Nome do programa 
} Comando_RoundRobin;

int pid = fork();

        if (pid == 0) {
            printf("AAA\n");
            char *const args[] = {processo, NULL};
            execve(processo, args, NULL);
            perror("execve");
            exit(1);
        }
        sleep(10);
        printf("Matei o processo do %s\n", processo);
        kill(pid, SIGKILL);