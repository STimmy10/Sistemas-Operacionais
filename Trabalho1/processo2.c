#include <stdio.h>

int main() {
    int tempo_execucao = 0;
    printf("Executando processo 2\n");
    while (1) {
        printf("Tempo em execução: %d segundos\n", tempo_execucao);
        sleep(1);
        tempo_execucao++;
    }

    return 0;
}