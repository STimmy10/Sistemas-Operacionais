#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {
    // Use caminhos completos ou relativos para os programas que deseja executar
    char *program1 = "./processo1";
    char *program2 = "./processo2";
    char *program3 = "./processo3";

    FILE *arquivo = fopen("exec.txt", "r"); // Abrindo o arquivo em modo de leitura ("r")

    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }

    char line[100]; // Uma string para armazenar cada linha do arquivo (ajuste o tamanho conforme necessário)

    while (fgets(line, sizeof(line), arquivo) != NULL) {
    char processo[50];

    // Use sscanf para extrair a parte desejada
    if (sscanf(line, "Run %s", processo) == 1) {
        printf("%s\n", processo);

        int pid = fork();

        if (pid == 0) {
            printf("AAA\n");
            char *const args[] = {processo, NULL};
            execve(processo, args, NULL);
            perror("execve");
            exit(1);
        }
        sleep(10);
        printf("Matei o processo do %s\n", processo);
        kill(pid, SIGKILL);
    }
}
/*
    if (fork() == 0) {
        // Filho 1
        char *const args[] = {program1, NULL};
        execve(program1, args, NULL);
        perror("execve");
        exit(1);
    } else if (fork() == 0) {
        // Filho 2
        char *const args[] = {program2, NULL};
        execve(program2, args, NULL);
        perror("execve");
        exit(1);
    } else if (fork() == 0) {
        // Filho 3
        char *const args[] = {program3, NULL};
        execve(program3, args, NULL);
        perror("execve");
        sleep(5);
        
        exit(1);
    }

    // Esperar todos os filhos terminarem
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }
*/
    fclose(arquivo); // Fecha o arquivo quando terminar de usá-lo

    return 0;
}
