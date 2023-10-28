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
    char nome_programa[50];
    int momento_inicio;
    int tempo_duracao;
    int tipo; // 0 para RoundRobin, 1 para RealTime
} Comando;


int main() {
    key_t chave = 7000;
    int segmento;
    char *p;
    char texto[100];
    int num_comandos = 0;
    char line[100];
    Comando *comandos;

    // Cria a memória compartilhada com a chave 7000
    segmento = shmget(chave, 1024, IPC_CREAT);
    if (segmento == -1) {
        perror("Erro ao criar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    comandos = (Comando *)shmat(segmento, NULL, 0);
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }


    FILE *arquivo = fopen("exec.txt", "r"); // Abrindo o arquivo em modo de leitura ("r")

    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }

    while (fgets(line, sizeof(line), arquivo) != NULL) {
        char comando[50];

        if (sscanf(line, "Run %s", comando) == 1) {
            Comando novo_comando;
            strcpy(novo_comando.nome_programa, comando);

            if (strstr(line, "I=") && strstr(line, "D=")) {
                // É um comando RealTime
                int momento_inicio, tempo_duracao;
                sscanf(line, "Run %*s I=%d D=%d", &momento_inicio, &tempo_duracao);
                novo_comando.momento_inicio = momento_inicio;
                novo_comando.tempo_duracao = tempo_duracao;
                novo_comando.tipo = 1; // RealTime
            } else {
                // É um comando RoundRobin
                novo_comando.momento_inicio = 0; // Valor padrão para RoundRobin
                novo_comando.tempo_duracao = 0; // Valor padrão para RoundRobin
                novo_comando.tipo = 0; // RoundRobin
            }

            comandos[num_comandos] = novo_comando;
            num_comandos++;
        }
    }

    // Agora, você tem um array com todos os comandos (RealTime e RoundRobin)
    for (int i = 0; i < num_comandos; i++) {
        if (comandos[i].tipo == 1) {
            printf("RealTime - Nome: %s, Início: %d, Duração: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        } else {
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    fclose(arquivo); // Fecha o arquivo quando terminar de usá-lo

    return 0;
}
