#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char*argv[]){
    while (*++argv) {
        printf("%s", *argv);
        if (argv[1]){
            printf(" ");
        }
    }
    printf("\n");
    
    return 0;
}