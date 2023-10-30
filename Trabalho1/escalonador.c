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

sem_t fila_mutex; // Semáforo para proteger a fila
sem_t exec_mutex; // Semáforo para controlar a execução dos processos

void inicializarFila(FilaComandos *fila) {
    fila->tamanho = 0;
    sem_init(&fila_mutex, 0, 1); // Inicializa o semáforo com um recurso
    sem_init(&exec_mutex, 0, 1); // Inicializa o semáforo de execução com um recurso
}

void pararComando(Comando* comando) {
    signal(comando->pid, SIGSTOP);
}

int conflitoEscalonamento(const Comando* novo, const FilaComandos* fila) {
    for (int i = 0; i < fila->tamanho; i++) {
        const Comando* existente = &fila->comandos[i];
        if (existente->tipo == REAL_TIME && novo->tipo == REAL_TIME) {
            if (novo->momento_inicio >= existente->momento_inicio && novo->momento_inicio < (existente->momento_inicio + existente->tempo_duracao)) {
                return 1; // Conflito de tempo de início
            }
        }
    }
    return 0;
}

void adicionarComando(FilaComandos *fila, Comando comando) {
    if(comando.tipo == REAL_TIME && comando.momento_inicio+comando.tempo_duracao > 60){
        printf("Comando RealTime ultrapassaria do minuto e não foi adicionado à fila.\n");
        return;
    }
    if (conflitoEscalonamento(&comando, fila)) {
        printf("Conflito de escalonamento para o comando %s\n", comando.nome_programa);
        return; 
    }
    printf("Adicionando comando %s à fila de comandos\n", comando.nome_programa);
    comando.pid = fork();
    if (comando.pid == 0) {
        // Processo filho
        execve(comando.nome_programa, NULL, NULL);
        perror("Erro ao executar o comando");
        sleep(1);
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

    // Comunicando com o escalonador
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

        int executando_real_time = 0;

        for (int i = 0; i < fila.tamanho; i++) {
            time(&tempo_atual);
            info_tempo = localtime(&tempo_atual);
            segundos_atuais = info_tempo->tm_sec;
            if (fila.comandos[i].tipo == REAL_TIME) {
                time(&tempo_atual);
                segundos_atuais = info_tempo->tm_sec;
                int diferenca_tempo = fila.comandos[i].momento_inicio - segundos_atuais;
                if (diferenca_tempo <= 0 && diferenca_tempo >= -1) {
                    sem_wait(&exec_mutex); // Bloqueia o semáforo de execução
                    printf("Segundo de início: %d - segundo atual: %d \n", fila.comandos[i].momento_inicio, segundos_atuais);
                    printf("Diferença de tempo: %d\n", abs(diferenca_tempo));
                    // É hora de executar o comando RealTime
                    executando_real_time = 1;
                    signal(fila.comandos[i].pid, SIGCONT);
                    sleep(fila.comandos[i].tempo_duracao);
                    signal(fila.comandos[i].pid, SIGSTOP);
                    sem_post(&exec_mutex); // Libera o semáforo de execução
                    }
            } else if (fila.comandos[i].tipo == ROUND_ROBIN && executando_real_time == 0) {
                sem_wait(&exec_mutex); // Bloqueia o semáforo de execução
                // Rodando os comandos RoundRobin
                signal(fila.comandos[i].pid, SIGCONT);
                sleep(1);
                signal(fila.comandos[i].pid, SIGSTOP);
                sem_post(&exec_mutex); // Libera o semáforo de execução
            }
            sleep(1);
            for(int j = 0; j < fila.tamanho; j++){
                pararComando(&fila.comandos[j]);
            }
            executando_real_time = 0;
        }

    }

    for (int i = 0; i < fila.tamanho; i++) {
        kill(fila.comandos[i].pid, SIGKILL);
    }

    return 0;
}
