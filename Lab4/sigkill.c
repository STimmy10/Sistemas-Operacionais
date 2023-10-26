#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigkill_handler(int signe) {
    printf("Recebi o sinal SIGKILL(%d). Isso não deveria acontecer.\n", signe);
}

int main() {
    void (*p)(int);
    p = signal(SIGKILL, sigkill_handler);

    printf("Endereco do manipulador anterior %p\n", p);

    while (1) {
    }

    return 0;
}
