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
    printf("Adicionando comando %s à fila de comandos\n", comando.nome_programa);
    comando.pid = fork();
    signal(comando.pid, SIGSTOP);
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
    Comando* comando_atual;

    // Inicializa a fila de comandos
    inicializarFila(&fila);

    // Variáveis para obter o tempo atual
    time_t tempo_atual;
    struct tm* info_tempo;
    int segundos_atuais;

    // Cria ou obtém a fila de mensagens com a mesma chave
    fila_mensagens = msgget(chave, 0666);
    if (fila_mensagens == -1) {
        perror("Erro ao acessar a fila de mensagens");
        exit(1);
    }

    rodando = 1;

    printf("Escalonador iniciado\n");

    while (1) {
        Comando mensagem;
        
        time(&tempo_atual);
        info_tempo = localtime(&tempo_atual);
        segundos_atuais = info_tempo->tm_sec;

        printf("segundos_atuais: %d\n", segundos_atuais);
        printf("vou receber mensagem\n");

        // Recebe mensagens da fila de mensagens
        if (msgrcv(fila_mensagens, &mensagem, sizeof(Comando), 0, 0) == -1) {
            perror("Erro ao receber mensagem da fila de mensagens");
            exit(1);
        }
        
        printf("Recebi mensagem\n");

        // Adiciona o comando à fila de comandos
        adicionarComando(&fila, mensagem);
        
        // Percorrendo a fila de comandos:
        if (segundos_atuais == 0) {
            printf("A fila de comandos está vazia.\n");
        }

        for (int i = 0; i < fila.tamanho; i++) {
            if(fila.comandos[i].tipo == REAL_TIME && segundos_atuais == fila.comandos[i].momento_inicio){
                //rodando os comandos RealTime
                comando_atual = &fila.comandos[i];
                signal(fila.comandos[i].pid, SIGCONT);
                sleep(fila.comandos[i].tempo_duracao);
                signal(fila.comandos[i].pid, SIGSTOP);
            } else {
                //rodando os comandos RoundRobin
                comando_atual = &fila.comandos[i];
                signal(fila.comandos[i].pid, SIGCONT);
                sleep(1);
                signal(fila.comandos[i].pid, SIGSTOP);
            }
        }
    }

    printf("Tempo de execucao do escalonador esgotado\n");
    return 0;
}
