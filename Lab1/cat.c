#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 200

int main(int argc, char*argv[]){
    FILE *arquivo;
    arquivo = fopen(argv[1], "r");
    
    if(!arquivo){
        printf("Erro ao abrir o arquivo");
        exit(2);
    }

    char buffer[MAX_SIZE];

    while(!feof(arquivo)){
        fscanf(arquivo, "%s", buffer);
        printf("%s\n", buffer);
    }

    return 0;
}