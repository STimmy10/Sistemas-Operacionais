#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/time.h>

int main(){
    time_t tempo_atual;
    struct tm* info_tempo;
    int segundos_atuais;
    int momento_inicio = 10;
    while(1){
        time(&tempo_atual);
        info_tempo = localtime(&tempo_atual);
        segundos_atuais = info_tempo->tm_sec;
        printf("Segundos atuais: %d\n", segundos_atuais);
        if (segundos_atuais == momento_inicio){
            printf("Estamos no segundo 10!\n");
        }
        sleep(1);
    }
    return 0;
}