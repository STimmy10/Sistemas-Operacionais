#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "estruturas.h"

int main() {
    key_t chave = 7000;
    int fila_mensagens;
    int num_comandos = 0;
    char line[100];

    // Conta o número total de comandos no arquivo
    FILE *arquivo1 = fopen("exec.txt", "r");
    if (arquivo1 == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }
    num_comandos = contaComandos(arquivo1);
    Comando comandos[num_comandos];
    printf("Número total de comandos: %d\n", num_comandos);
    fclose(arquivo1);

    FILE *arquivo = fopen("exec.txt", "r");
    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }

    // Cria uma fila de mensagens para comunicação com o escalonador
    fila_mensagens = msgget(chave, IPC_CREAT | 0666);
    if (fila_mensagens == -1) {
        perror("Erro ao criar a fila de mensagens");
        exit(1);
    }

    int comando_idx = 0;

    while (fgets(line, sizeof(line), arquivo)) {
        char comando[8];

        if (sscanf(line, "Run %s", comando) == 1) {
            Comando novo_comando;
            strcpy(novo_comando.nome_programa, comando);
            if (strstr(line, "I=") && strstr(line, "D=")) {
                // É um comando RealTime
                int momento_inicio, tempo_duracao;
                sscanf(line, "Run %*s I=%d D=%d", &momento_inicio, &tempo_duracao);
                novo_comando.momento_inicio = momento_inicio;
                novo_comando.tempo_duracao = tempo_duracao;
                novo_comando.tipo = REAL_TIME; // RealTime
                novo_comando.index = comando_idx;
            } else {
                // É um comando RoundRobin
                novo_comando.momento_inicio = 0; // Valor padrão para RoundRobin
                novo_comando.tempo_duracao = 0; // Valor padrão para RoundRobin
                novo_comando.tipo = ROUND_ROBIN; // RoundRobin
                novo_comando.index = comando_idx;
            }

            comandos[comando_idx] = novo_comando;
            comando_idx++;

            // Envia o comando para o escalonador através da fila de mensagens
            if (msgsnd(fila_mensagens, &novo_comando, sizeof(Comando), 0) == -1) {
                perror("Erro ao enviar mensagem para o escalonador");
                exit(1);
            }
        }
    }

    fclose(arquivo); // Fecha o arquivo quando terminar de usá-lo

    return 0;
}

int contaComandos(FILE *arquivo){
    char line[35];
    int num_comandos = 0;

    while (fgets(line, sizeof(line), arquivo) != NULL) {
        if (sscanf(line, "Run %*s") == 0) {
            num_comandos++;
        }
    }
    return num_comandos;
}