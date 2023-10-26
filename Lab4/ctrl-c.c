#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#define EVER ;;

void intHandler(int sinal);
void quitHandler(int sinal);

int main (void){
    void (*p)(int);// ponteiro para funcao que recebe int como parametro
    /*
        signal(numSinal, endereco da funcao de tratamento do sinal)
           |-> Retorna
                |->Sucesso: Endereco da antiga funcao de tratamento
                |->Falha: -1
        SIGINT(2) --> Interrompe pelo teclado 
    */
    
    //p = signal(SIGINT, intHandler);
    /*A funcao de tratamento do sinal apenas printa uma string, anulando o sinal*/
    printf("Endereco do manipulador anterior %p\n", p);
    /*
        SIGQUIT(3) --> quit pelo teclado
    */
    //p = signal(SIGQUIT, quitHandler);
    printf("Endereco do manipulador anterior %p\n", p);

    puts ("Ctrl-C desabilitado. Use Ctrl-\\ para terminar");

    for(EVER);
}

void intHandler(int sinal){
    printf("Voce pressionou Ctrl-C (%d)\n", sinal);
}

void quitHandler(int sinal){
    puts ("Terminando o processo...");
    exit (0);
}