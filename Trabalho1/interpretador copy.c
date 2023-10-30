#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include "estruturas.h"

#define MAX_COMANDOS 100

typedef struct {
    Comando comandos[MAX_COMANDOS];
    int tamanho;
} FilaComandos;

void inicializarFila(FilaComandos *fila) {
    fila->tamanho = 0;
}

int main() {
    key_t chave = 7000;
    int segmento;
    FilaComandos *fila;
    pid_t escalonador_pid;

    // Cria ou obtém a memória compartilhada com a mesma chave
    segmento = shmget(chave, sizeof(FilaComandos), IPC_CREAT | 0666);
    if (segmento == -1) {
        perror("Erro ao acessar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada à memória deste processo
    fila = (FilaComandos *)shmat(segmento, NULL, 0);

    // Obtém o PID do escalonador (certifique-se de que o escalonador já está em execução)
    escalonador_pid = ...; // Preencha com a lógica para obter o PID do escalonador

    // Exemplo de comando a ser enviado
    Comando novo_comando;
    strcpy(novo_comando.nome_programa, "meu_programa");
    novo_comando.tipo = ROUND_ROBIN;
    novo_comando.momento_inicio = 0;
    novo_comando.tempo_duracao = 5;

    // Envie o comando para a fila compartilhada
    if (fila->tamanho < MAX_COMANDOS) {
        fila->comandos[fila->tamanho] = novo_comando;
        fila->tamanho++;

        // Envie um sinal para o escalonador para notificá-lo sobre o novo comando
        kill(escalonador_pid, SIGUSR1);

        printf("Comando adicionado à fila de comandos\n");
    } else {
        printf("A fila de comandos está cheia. O comando não foi adicionado.\n");
    }

    // Libera a memória compartilhada quando não for mais necessária
    shmdt(fila);

    return 0;
}
