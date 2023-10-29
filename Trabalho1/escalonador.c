#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "estruturas.h"

int main() {
    key_t chave = 7000;
    int segmento;
    Comando *comandos;
    
    // Acessa a memoria compartilhada com a chave 7000
    segmento = shmget(chave, sizeof(Comandos), IPC_CREAT | 0666); // Use as mesmas informacoes de chave e tamanho da memoria compartilhada
    if (segmento == -1) {
        perror("Erro ao acessar a memoria compartilhada");
        exit(1);
    }

    // Anexa a memoria compartilhada ao processo
    comandos = (Comando *)shmat(segmento, NULL, 0);
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memoria compartilhada");
        exit(1);
    }

    struct shmid_ds shmid_ds;
    if (shmctl(segmento, IPC_STAT, &shmid_ds) == -1) {
        perror("Erro ao obter informacoes do segmento de memoria compartilhada");
        exit(1);
    }


    int tamanho = (long)(shmid_ds.shm_segsz);

    printf("Tamanho da memoria compartilhada: %d bytes\n", tamanho);

    // Exiba os comandos RealTime e RoundRobin armazenados na memoria compartilhada
    printf("Quantidade de comandos: %ld\n", tamanho / sizeof(Comando));

    int num_comandos = tamanho / sizeof(Comando); // Calcule o número de comandos na memoria
    for (int i = 0; i < num_comandos; i++) {
        printf("Comando %d:\n", i);

        if (comandos[i].tipo == 1) {
            printf("RealTime - Nome: %s, Início: %d, Duração: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        } else {
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    // Libere a memoria compartilhada após o uso
    shmdt(comandos);

    return 0;
}