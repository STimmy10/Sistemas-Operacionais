#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/time.h>

int main() {
    
    pid_t pid = getpid();
    printf("Programa 4 - pid: %d\n", pid);
    int tempo_execucao = 0;

    while (1) {
        printf("Tempo em execução do processo 4: %d segundos\n", tempo_execucao);
        sleep(1);
        tempo_execucao++;
    }

    return 0;
}