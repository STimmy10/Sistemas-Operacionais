#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[]){
    int status = 0;
    int n = 3; 


    if (fork() != 0) { //Cria um processo filho
        //Código do pai
        for (int i = 0; i < 10000; i++){
            n += 1;
        }
        printf("processo pai, pid = %d, n = %d\n", getpid(), n);
        waitpid(-1, &status, 0); // Aguarda o filho sair
    }
     else {
        // Código do filho
        for (int i = 0; i < 10000; i++){
            n += 10;
        }
        printf("processo filho, pid = %d, n = %d\n", getpid(), n);
     }
     return 0;
}