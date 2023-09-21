#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    key_t chave = 7000;
    int segmento;
    char *p;
    char texto[100];

    // Lê da entrada a mensagem do dia
    printf("Digite a mensagem do dia: ");
    fgets(texto, sizeof(texto), stdin);

    // Cria a memória compartilhada com a chave 7000
    segmento = shmget(chave, 1024, IPC_CREAT);
    if (segmento == -1) {
        perror("Erro ao criar a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    p = (char *)shmat(segmento, NULL, 0);
    if (p == (char *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    // Copia a mensagem para a memória compartilhada
    strcpy(p, texto);
    printf("Mensagem salva na memória compartilhada.\n");

    return 0;
}
