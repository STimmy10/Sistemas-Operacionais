#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include "estruturas.h"

#define MAX_COMANDOS 100

typedef struct {
    Comando comandos[MAX_COMANDOS];
    int tamanho;
} FilaComandos;

void inicializarFila(FilaComandos *fila) {
    fila->tamanho = 0;
}

void adicionarComando(FilaComandos *fila, Comando comando) {
    if (fila->tamanho < MAX_COMANDOS) {
        fila->comandos[fila->tamanho] = comando;
        fila->tamanho++;
    } else {
        printf("A fila de comandos está cheia. O comando %s não foi adicionado.\n", comando.nome_programa);
    }
}

int main() {
    key_t chave = 7000;
    int fila_mensagens;
    FilaComandos fila;
    int rodando = 0;
    pid_t pid_filho;
    // Inicializa a fila de comandos
    inicializarFila(&fila);

    // Cria ou obtém a fila de mensagens com a mesma chave
    fila_mensagens = msgget(chave, 0666);
    if (fila_mensagens == -1) {
        perror("Erro ao acessar a fila de mensagens");
        exit(1);
    }
    rodando = 1;
    signal (SIGALRM, rodando == 0);
    alarm (12);
    while (rodando) {
        Comando mensagem;
        
        // Recebe mensagens da fila de mensagens
        if (msgrcv(fila_mensagens, &mensagem, sizeof(Comando), 0, 0) == -1) {
            perror("Erro ao receber mensagem da fila de mensagens");
            exit(1);
        }

        // Adiciona o comando à fila de comandos
        adicionarComando(&fila, mensagem);

        if(mensagem.tipo == REAL_TIME){ // RealTime
            // Cria um processo filho para executar o comando
            pid_t pid_filho = fork();
            if (pid_filho == 0) {
                // Processo filho
                printf("Executando o comando %s por %d segundos\n", mensagem.nome_programa, mensagem.tempo_duracao);
                execl(mensagem.nome_programa, mensagem.nome_programa, NULL);
                exit(0);
            } else {
                // Processo pai
                // Aguarda o tempo de execução do comando
                sleep(mensagem.tempo_duracao);
                // Termina o processo filho
                kill(pid_filho, 9);
            }
        } else { // RoundRobin
            // Cria um processo filho para executar o comando
            pid_t pid_filho = fork();
            if (pid_filho == 0) {
                // Processo filho
                printf("Executando o comando %s\n", mensagem.nome_programa);
                execl(mensagem.nome_programa, mensagem.nome_programa, NULL);
                exit(0);
            } else {
                // Processo pai
                // Aguarda o tempo de execução do comando
                sleep(10);
                // Termina o processo filho
                kill(pid_filho, 9);
            }
        }
    }

    printf("Tempo de execucao do escalonador esgotado\n");
    kill(pid_filho, SIGKILL);

    for (int i = 0; i < fila.tamanho; i++) {
        printf("Comando %s\n", fila.comandos[i].nome_programa);
    }

    return 0;
}
