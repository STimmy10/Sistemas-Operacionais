#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    int tempo_execucao = 0;
    printf("Executando processo 3\n");
    while (1) {
        printf("Tempo em execução do processo 3: %d segundos\n", tempo_execucao);
        sleep(1);
        tempo_execucao++;
    }

    return 0;
}