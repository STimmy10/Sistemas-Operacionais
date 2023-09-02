#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

// Exibe o prompt na tela
void type_prompt(const char *prompt) {
    printf("%s ", prompt);
}


//Lê o comando e os parâmetros da entrada padrão e os armazena nas variáveis command e parameters
int read_command(char *command, char *parameters[]) {
    char input[1000];
    if (fgets(input, sizeof(input), stdin) == NULL) {
        perror("Erro ao ler o comando");
        return 1;
    }

    // Remove o \n do final da string
    size_t input_length = strlen(input);
    if (input_length > 0 && input[input_length - 1] == '\n') {
        input[input_length - 1] = '\0';
    }

    // Separa o comando e os parâmetros
    char *token = strtok(input, " ");
    if (token == NULL) {
        fprintf(stderr, "Comando ausente\n");
        return 1;
    }

    strcpy(command, token); // Armazena o comando

    // Armazena os parâmetros
    int param_index = 1;
    while ((token = strtok(NULL, " ")) != NULL) {
        parameters[param_index] = (char *)malloc(strlen(token) + 1);
        if (parameters[param_index] == NULL) {
            perror("Erro na alocação de memória");
            return 1;
        }
        strcpy(parameters[param_index], token);
        param_index++;
    }

    parameters[param_index] = NULL; // Marca o fim da lista de parâmetros

    return 0;
}

int main(int argc, char *argv[]) {
    int status;
    char command[100];
    char *parameters[100];

    while (1) {
        type_prompt("@"); // Exibe o prompt
        if (read_command(command, parameters) != 0) {
            continue; // Trata os erros de entrada e continua
        }

        if (fork() != 0) { // Cria um processo filho
            // Código do pai
            waitpid(-1, &status, 0); // Aguarda o filho sair

            // Libere a memória alocada para os parâmetros, depois que sair do processo filho
            for (int i = 1; parameters[i] != NULL; i++) {
                free(parameters[i]);
            }
        } else {
            // Código do filho
            execve(command, parameters, NULL); // Executa o comando command com os parâmetros parameters
            perror(execvp(command, parameters)); // Se execve falhar (o programa não está no diretório atual), roda o programa com o caminho absoluto
            exit(1);
        }
    }

    return 0;
}