#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct {
    char nome_programa[50];
    int momento_inicio;
    int tempo_duracao;
    int tipo; // 0 para RoundRobin, 1 para RealTime
} Comando;

int main() {
    key_t chave = 7000;
    int segmento;
    Comando *comandos;
    
    // Acessa a memória compartilhada com a chave 7000
    segmento = shmget(chave, 0, 0); // Use as mesmas informações de chave e tamanho da memória compartilhada
    if (segmento == -1) {
        perror("Erro ao acessar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    comandos = (Comando *)shmat(segmento, NULL, 0);
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    struct shmid_ds shmid_ds;
    if (shmctl(segmento, IPC_STAT, &shmid_ds) == -1) {
        perror("Erro ao obter informações do segmento de memória compartilhada");
        exit(1);
    }


    int tamanho = (long)(shmid_ds.shm_segsz);

    printf("Tamanho da memória compartilhada: %d bytes\n", tamanho);

    // Exiba os comandos RealTime e RoundRobin armazenados na memória compartilhada
    printf("Quantidade de comandos: %ld\n", tamanho / sizeof(Comando));

    int num_comandos = tamanho / sizeof(Comando); // Calcule o número de comandos na memória
    for (int i = 0; i < num_comandos; i++) {
        printf("Comando %d:\n", i);

        if (comandos[i].tipo == 1) {
            printf("RealTime - Nome: %s, Início: %d, Duração: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        } else {
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    // Libere a memória compartilhada após o uso
    shmdt(comandos);

    return 0;
}
