#include <stdio.h>
#include <stdlib.h>


#define MAXSIZE 200

int main(int argc, char *argv[]){
    for(int i = 1; i < argc; i++){
        FILE *arquivo;
        
        arquivo = fopen(argv[i], "r");
        
        char buffer[MAXSIZE];

        if(!arquivo){
            printf("Erro ao abrir arquivo");
            exit(1);
        }
        while(!feof(arquivo)){
            fscanf(arquivo, "%s", buffer);
            printf("%s\n", buffer);
        }
        fclose(arquivo);
    }
    return 0;
}