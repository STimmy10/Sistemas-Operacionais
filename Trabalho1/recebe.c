#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "estruturas.h"

int main() {
    key_t chave = 7000;
    int fila_mensagens;

    // Cria ou obtém a fila de mensagens com a mesma chave
    fila_mensagens = msgget(chave, 0666);
    if (fila_mensagens == -1) {
        perror("Erro ao acessar a fila de mensagens");
        exit(1);
    }

    while (1) {
        Comando mensagem;
        
        // Recebe mensagens da fila de mensagens
        if (msgrcv(fila_mensagens, &mensagem, sizeof(Comando), 0, 0) == -1) {
            perror("Erro ao receber mensagem da fila de mensagens");
            exit(1);
        }

        // Imprime as mensagens recebidas
        printf("Mensagem Recebida:\n");
        printf("Nome do Programa: %s\n", mensagem.nome_programa);
        printf("Tipo: %s\n", (mensagem.tipo == REAL_TIME) ? "RealTime" : "RoundRobin");
        printf("Momento de Início: %d\n", mensagem.momento_inicio);
        printf("Tempo de Duração: %d\n", mensagem.tempo_duracao);
        printf("Índice: %d\n", mensagem.index);
        printf("\n");
    }

    return 0;
}
