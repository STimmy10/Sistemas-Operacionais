#include <stdio.h>
#include <stdlib.h>
#include "simulador.h"


int main (int args, char* argv[]){
    if(args != 5){
        perror("Quantidade de argumentos inválida.");
        exit(1);
    }

    int pageSize = atoi(argv[3]);
    int totalMem = atoi(argv[4]);
    
    printf("Arquivo de entrada: %s\n",argv[2]);
    printf("Tamanho de memoria fisica: %s MB\n", argv[4]);
    printf("Tamanho das paginas: %s KB\n",argv[3]);
    printf("Algoritmo de substituicao: %s\n",argv[1]);

    go_simulator(totalMem, pageSize,argv);


    return 0;
}
