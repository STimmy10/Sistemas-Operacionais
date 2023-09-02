#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_command(char *command, char **parameters) {

    char *command = argv[1]; // O primeiro argumento é o comando

    int contador = 0;
    while (argv[contador + 2] != NULL) {
        contador++;
    }

    char **parametros = (char **)malloc((contador + 1) * sizeof(char *));
    if (parametros == NULL) {
        perror("Erro na alocação de memória para os parâmetros");
        return 1;
    }

    for (int i = 0; i < contador; i++) {
        parametros[i] = argv[i + 2];
    }

    parametros[contador] = NULL;

    free(parametros);

    return 0;
}
