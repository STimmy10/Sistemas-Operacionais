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
#include "estruturas.h"

void rodaProcesso(Comando p, pid_t* pid);
char* montaComando(const char* str1, const char* str2);
Comando *comandos;
Comando *comandos_em_execucao;
int comandos_em_execucao_tamanho = 0;

void paraEscalonador(int signal){
    printf("Tempo de execucao do escalonador esgotado\n");
    for (int i = 0; i < comandos_em_execucao_tamanho; i++){
        kill(comandos_em_execucao[i].index, SIGKILL);
        printf("Terminei de executar o programa %s\n", comandos_em_execucao[i].nome_programa);
    }
    exit(0);
}

int main() {
    key_t chave = 7000;
    int segmento;


    time_t tempo_atual;
    struct tm* info_tempo;
    int segundos_atuais;
    
    // Acessa a memoria compartilhada com a chave 7000
    segmento = shmget(chave, 0, 0); // Use as mesmas informa��es de chave e tamanho da memoria compartilhada
    if (segmento == -1) {
        perror("Erro ao acessar a memoria compartilhada");
        exit(1);
    }

    // Anexa a memoria compartilhada ao processo
    comandos = (Comando *)shmat(segmento, NULL, 0);
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memoria compartilhada");
        exit(1);
    }

    struct shmid_ds shmid_ds;
    if (shmctl(segmento, IPC_STAT, &shmid_ds) == -1) {
        perror("Erro ao obter informacoes do segmento de memoria compartilhada");
        exit(1);
    }


    int tamanho = (long)(shmid_ds.shm_segsz);

    comandos_em_execucao = (Comando *)malloc(tamanho*sizeof(Comando));

    printf("Tamanho da memoria compartilhada: %d bytes\n", tamanho);

    // Exiba os comandos RealTime e RoundRobin armazenados na memoria compartilhada
    printf("Quantidade de comandos: %ld\n", tamanho / sizeof(Comando));

    int num_comandos = tamanho / sizeof(Comando); // Calcule o n�mero de comandos na memoria
    for (int i = 0; i < num_comandos; i++) {
        if (comandos[i].tipo == 1) {
            printf("RealTime - Nome: %s, Inicio: %d, Duracao: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        } else {
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    // Crie um processo filho para cada comando armazenado na memoria compartilhada
    pid_t pid;
    rodaProcesso(comandos[0], &pid);
    comandos_em_execucao[comandos_em_execucao_tamanho] = comandos[0];
    comandos_em_execucao_tamanho++;


    signal (SIGALRM, paraEscalonador);
    alarm(10);


    while(1){
        time(&tempo_atual);
        info_tempo = localtime(&tempo_atual);
        segundos_atuais = info_tempo->tm_sec;
        printf("Segundos atuais: %d\n", segundos_atuais);
        sleep(1);
    }

    // Libere a memoria compartilhada ap�s o uso
    shmdt(comandos);

    return 0;
}

void rodaProcesso(Comando p, pid_t* pid){
	char inicioPath[] = "./";
	char *path;

	path = montaComando(inicioPath, p.nome_programa);
	
	char *argv[] = {NULL};
	
	if(fork() == 0){
		pid = getpid();
		printf("pid no escalonador = %d\n", pid);
		printf("Iniciando o programa %s\n", path);
		execvp(path, argv);
	} 
	return;
}

char* montaComando(const char* str1, const char* str2) {
	int tamanhoStr1 = strlen(str1);
	int tamanhoStr2 = strlen(str2);
	int tamanhoTotal = tamanhoStr1 + tamanhoStr2 + 1; //adiciona /n

	char* resultado = (char*)malloc(tamanhoTotal);

	if (resultado == NULL) {
		perror("Erro ao alocar memoria");
		exit(1);
	}

	strcpy(resultado, str1);
	strcat(resultado, str2);

	return resultado;
}