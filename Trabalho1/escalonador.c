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

    printf("Escalonador\n");

    // Acessa a memória compartilhada com a chave 7000
    segmento = shmget(chave, 1024, 0666); // Use as mesmas informações de chave e tamanho da memória compartilhada
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

    // Exiba os comandos RealTime e RoundRobin armazenados na memória compartilhada
    printf("%d bytes de memória compartilhada:\n", segmento);
    printf("Quantidade de comandos: %d\n", segmento / sizeof(Comando));

    int num_comandos = segmento / sizeof(Comando); // Calcule o número de comandos na memória
    for (int i = 0; i < num_comandos; i++) {
        printf("Comando %d:\n", i);

        if (comandos[i].tipo == 1) {
            printf("Achei um Real time\n");
            printf("RealTime - Nome: %s, Início: %d, Duração: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        } else {
            printf("Achei um RoundRobin\n");
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    // Libere a memória compartilhada após o uso
    shmdt(comandos);

    return 0;
}
