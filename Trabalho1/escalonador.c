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
#include <semaphore.h>

#define MAX_COMANDOS 100
int rodando = 1;

typedef struct {
    Comando comandos[MAX_COMANDOS];
    int tamanho;
} FilaComandos;

void trataAlarme(int signum){
    printf("Tempo de execucao do escalonador esgotado\n");
    rodando = 0;
}

sem_t fila_mutex; // Declare um semáforo para proteger a fila

void inicializarFila(FilaComandos *fila) {
    fila->tamanho = 0;
    sem_init(&fila_mutex, 0, 1); // Inicializa o semáforo com um recurso
}

void adicionarComando(FilaComandos *fila, Comando comando) {
    if(comando.tipo == REAL_TIME && comando.momento_inicio+comando.tempo_duracao > 60){
        printf("Comando RealTime ultrapassaria do minuto e não foi adicionado à fila.\n");
        return;
    }
    
    printf("Adicionando comando %s à fila de comandos\n", comando.nome_programa);
    comando.pid = fork();
    if (comando.pid == 0) {
        // Processo filho
        execve(comando.nome_programa, NULL, NULL);
        perror("Erro ao executar o comando");
        signal(comando.pid, SIGSTOP);
        exit(1);
    } else if (comando.pid > 0) {
        // Processo pai
        sem_wait(&fila_mutex); // Bloqueia o semáforo para atualizar a fila
        fila->comandos[fila->tamanho] = comando;
        fila->tamanho++;
        sem_post(&fila_mutex); // Libera o semáforo após a atualização
    } else {
        perror("Erro ao criar o processo filho");
    }
}

int main() {
    key_t chave = 7000;
    int fila_mensagens;
    FilaComandos fila;
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

    printf("Escalonador iniciado\n");

    signal (SIGALRM, trataAlarme);
    alarm (120);


    while (rodando) {
        sleep(1);
        Comando mensagem;
        if (msgrcv(fila_mensagens, &mensagem, sizeof(mensagem), 0, IPC_NOWAIT) != -1) {
            // Recebeu uma mensagem
            printf("Recebeu um comando\n");
            adicionarComando(&fila, mensagem);
            }

        time(&tempo_atual);
        info_tempo = localtime(&tempo_atual);
        segundos_atuais = info_tempo->tm_sec;

        for (int i = 0; i < fila.tamanho; i++) {
            if (fila.comandos[i].tipo == REAL_TIME) {
                time(&tempo_atual);
                int diferenca_tempo = fila.comandos[i].momento_inicio - tempo_atual;
                if (diferenca_tempo <= 0) {
                    // É hora de executar o comando RealTime
                    comando_atual = &fila.comandos[i];
                    signal(fila.comandos[i].pid, SIGCONT);
                    sleep(fila.comandos[i].tempo_duracao);
                    signal(fila.comandos[i].pid, SIGSTOP);
                    }
            } else if (fila.comandos[i].tipo == ROUND_ROBIN) {
                // Rodando os comandos RoundRobin
                printf("Sem nada de maior prioridade, rodando round robin\n");
                comando_atual = &fila.comandos[i];
                signal(fila.comandos[i].pid, SIGCONT);
                sleep(1);
                signal(fila.comandos[i].pid, SIGSTOP);
            }
            sleep(1);
        }

    }

    for (int i = 0; i < fila.tamanho; i++) {
        kill(fila.comandos[i].pid, SIGKILL);
    }

    return 0;
}
