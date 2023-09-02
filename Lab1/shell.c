#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

void type_prompt(const char *prompt) {
    printf("%s ", prompt);
}

int read_command(char *command, char *parameters[]) {
    char input[1000];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Erro ao ler o comando");
        return 1;
    }

    size_t input_length = strlen(input);
    if (input_length > 0 && input[input_length - 1] == '\n') {
        input[input_length - 1] = '\0';
    }

    char *token = strtok(input, " ");
    if (token == NULL) {
        fprintf(stderr, "Comando ausente\n");
        return 1;
    }

    strcpy(command, token); // Aqui armazenamos o comando
    printf("Comando: %s\n", command);


    int param_index = 0;
    while ((token = strtok(NULL, " ")) != NULL) {
        parameters[param_index++] = token;
    }
    parameters[param_index] = NULL;

    return 0;
}

int main(int argc, char *argv[]) {
    int status;
    char command[100];
    char *parameters[100];

    while (1) {
        type_prompt("@"); // Exibe o prompt
        read_command(command, parameters);

        if (fork() != 0) { // Cria um processo filho
            // Código do pai
            waitpid(-1, &status, 0); // Aguarda o filho sair
        } else {
            // Código do filho
            execve(command, parameters, NULL); // Executa o comando
            perror("Erro ao executar o comando"); // Se execvp falhar
            exit(1);
        }
    }

    return 0;
}
