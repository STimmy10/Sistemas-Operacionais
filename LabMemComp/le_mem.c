#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main() {
    key_t chave = 7000;
    int segmento;
    char *p;

    // Obtém a memória compartilhada existente
    segmento = shmget(chave, 0, 0);
    if (segmento == -1) {
        perror("Erro ao obter a memória compartilhada");
        exit(1);
    }

    // Anexa a memória compartilhada ao processo
    p = (char *)shmat(segmento, 0, 0);
    if (p == (char *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    printf("Mensagem do dia: %s\n", p);

    // Libera a memória compartilhada do processo
    shmdt(p);

    // Libera a memória compartilhada
    shmctl(segmento, IPC_RMID, 0);

    return 0;
}
