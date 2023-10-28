#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Definir as estruturas dos objetos de comando
typedef struct {
    char* nome_programa; // Nome do programa
    int momento_inicio; // Momento de início
    int tempo_duracao; // Tempo de duração
} Comando_RealTime;

typedef struct {
    char* nome_programa; // Nome do programa 
} Comando_RoundRobin;


int main() {
    key_t chave = 7000;
    int segmento;
    char *p;
    char texto[100];
    Comando_RealTime* comandos_realtime;
    Comando_RoundRobin* comandos_roundrobin;

    // Cria a memória compartilhada com a chave 7000
    segmento = shmget(chave, 1024, IPC_CREAT);
    if (segmento == -1) {
        perror("Erro ao criar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    comandos_realtime = (Comando_RealTime *)shmat(segmento, NULL, 0);
    if (comandos_realtime == (Comando_RealTime *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    comandos_roundrobin = (Comando_RoundRobin *)shmat(segmento, NULL, 0);
    if (comandos_roundrobin == (Comando_RoundRobin *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    FILE *arquivo = fopen("exec.txt", "r"); // Abrindo o arquivo em modo de leitura ("r")

    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }

    char line[100];
    int index_realtime = 0;
    int index_roundrobin = 0;

    while (fgets(line, sizeof(line), arquivo) != NULL) {
        char comando[50];

        if (sscanf(line, "Run %s", comando) == 1) {
            if (strstr(line, "I=") && strstr(line, "D=")) {
                // É um comando RealTime
                Comando_RealTime comando_realtime;
                sscanf(line, "Run %s I=%d D=%d", comando, &comando_realtime.momento_inicio, &comando_realtime.tempo_duracao);
                printf("Comando RealTime: %s I=%d D=%d\n", comando, comando_realtime.momento_inicio, comando_realtime.tempo_duracao);
                index_realtime++;
            } else {
                // É um comando RoundRobin
                Comando_RoundRobin comando_roundrobin;
                printf("Comando RoundRobin: %s\n", comando);
                index_roundrobin++;
            }
            
        }
    }
    printf("Quantidade de comandos RealTime: %d\n", index_realtime);
    printf("Quantidade de comandos RoundRobin: %d\n", index_roundrobin);

    fclose(arquivo); // Fecha o arquivo quando terminar de usá-lo

    return 0;
}
