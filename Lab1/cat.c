#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 200

/*corrigir porque ele ta printando 2x cada linha do arquivo*/

int main(int argc, char*argv[]){
    for(int i =i; i<argc; i++){
        FILE *arquivo;
        arquivo = fopen(argv[i], "r");
        
        if(!arquivo){
            printf("Erro ao abrir o arquivo");
            exit(2);
        }
    
        char buffer[MAX_SIZE];
    
        while(!feof(arquivo)){
            fscanf(arquivo, "%s", buffer);
            printf("%s\n", buffer);
        }
    }

    return 0;
}
