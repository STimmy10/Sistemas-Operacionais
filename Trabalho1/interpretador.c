#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "estruturas.h"
int contaComandos(FILE *arquivo);

int main() {
    key_t chave = 7000;
    size_t segmento;
    pid_t pid;
    int num_comandos = 0;
    char line[100];
    

    // Conta o número total de comandos no arquivo
    FILE *arquivo1 = fopen("exec.txt", "r");
    if (arquivo1 == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return 1;
    }
    num_comandos = contaComandos(arquivo1);
    Comando *comandos;
    printf("Numero total de comandos: %d\n", num_comandos);
    fclose(arquivo1);

    FILE *arquivo = fopen("exec.txt", "r");
    if (arquivo == NULL) {
        printf("Nao foi possivel abrir o arquivo.\n");
        return 1;
    }

    // Cria a memória compartilhada com base no número total de comandos
    segmento = shmget(chave, num_comandos * sizeof(Comando), IPC_CREAT | 0666);
    if (segmento == -1) {
        perror("Erro ao criar a memoria compartilhada");
        exit(1);
    }

    // Verifica o tamanho do segmento após a criação
    struct shmid_ds shmid_ds;

    if (shmctl(segmento, IPC_STAT, &shmid_ds) == -1) {
        perror("Erro ao obter informacoes do segmento de memoria compartilhada");
        exit(1);
    }

    printf("Criando a memoria compartilhada com chave %d e tamanho %ld bytes\n", chave, (long)(shmid_ds.shm_segsz));

    // Anexa a memória compartilhada ao processo
    
    comandos = (Comando*)shmat(segmento, NULL, 0);
    
    
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memoria compartilhada");
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
        }
    }
    

    // Agora, você tem um array com todos os comandos (RealTime e RoundRobin) na memória compartilhada
    for (int i = 0; i < num_comandos; i++) {
        if (comandos[i].tipo == REAL_TIME){
            printf("RealTime - Nome: %s, ID:%d, Inicio: %d, Duracao: %d\n", comandos[i].nome_programa,comandos[i].index, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        }
        else{
            printf("RoundRobin - Nome: %s, ID:%d\n", comandos[i].nome_programa, comandos[i].index);
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