#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    pid_t child1, child2;
    int status;

    // Crie o primeiro filho
    child1 = fork();
    if (child1 == 0) {
        // Código do primeiro filho
        execvp("./filho1", NULL);
        perror("execvp");
    }

    // Crie o segundo filho
    child2 = fork();
    if (child2 == 0) {
        // Código do segundo filho
        execvp("./filho2", NULL);
        perror("execvp");
    }

    // Pai coordena a execução dos filhos
    int running_child = 1;
    int running_time = 0;

    while (running_time < 15) {
        if (running_child == 1) {
            // Se o filho 1 estiver executando, pare ele e execute o filho 2
            kill(child1, SIGCONT);
            kill(child2, SIGSTOP);
            sleep(1);
            kill(child1, SIGSTOP);
        } else {
            // Se o filho 2 estiver executando, pare ele e execute o filho 1
            kill(child1, SIGSTOP);
            kill(child2, SIGCONT);
            sleep(1);
            kill(child2, SIGSTOP);
        }

        running_child = (running_child == 1) ? 2 : 1;
        running_time += 1;
    }
    printf("\nPai encerrando os filhos\n");
    // Pai encerra os filhos
    kill(child1, SIGKILL);
    kill(child2, SIGKILL);

    // Espere os filhos terminarem
    waitpid(child1, &status, 0);
    waitpid(child2, &status, 0);

    return 0;
}
