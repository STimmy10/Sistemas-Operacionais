#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>

// Definir as estruturas dos objetos de comando
typedef struct {
    char nome_programa[50];
    int momento_inicio;
    int tempo_duracao;
    int tipo; // 0 para RoundRobin, 1 para RealTime
} Comando;

int main() {
    key_t chave = 7000;
    int segmento;

    int num_comandos = 0;
    char line[100];
    Comando *comandos;

    // Conta o número total de comandos no arquivo
    FILE *arquivo = fopen("exec.txt", "r");
    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }
    while (fgets(line, sizeof(line), arquivo) != NULL) {
        if (sscanf(line, "Run %*s") == 0) {
            num_comandos++;
        }
    }
    printf("Número total de comandos: %d\n", num_comandos);


    // Cria a memória compartilhada com base no número total de comandos
    segmento = shmget(chave, num_comandos * sizeof(Comando), IPC_CREAT);
    if (segmento == -1) {
        perror("Erro ao criar a memória compartilhada");
        exit(1);
    }

    // Verifica o tamanho do segmento após a criação
    struct shmid_ds shmid_ds;
    if (shmctl(segmento, IPC_STAT, &shmid_ds) == -1) {
        perror("Erro ao obter informações do segmento de memória compartilhada");
        exit(1);
    }

    printf("Criando a memória compartilhada com chave %d e tamanho %ld bytes\n", chave, (long)(shmid_ds.shm_segsz));

    // Anexa a memória compartilhada ao processo
    comandos = (Comando *)shmat(segmento, NULL, 0);
    if (comandos == (Comando *)-1) {
        perror("Erro ao anexar a memória compartilhada");
        exit(1);
    }

    // Lê o arquivo e preenche as estruturas de comando na memória compartilhada
    arquivo = fopen("exec.txt", "r");
    if (arquivo == NULL) {
        printf("Não foi possível abrir o arquivo.\n");
        return 1;
    }
    int comando_idx = 0;
    while (fgets(line, sizeof(line), arquivo) != NULL) {
        char comando[50];

        if (sscanf(line, "Run %s", comando) == 1) {
            Comando novo_comando;
            strcpy(novo_comando.nome_programa, comando);

            if (strstr(line, "I=") && strstr(line, "D=")) {
                // É um comando RealTime
                int momento_inicio, tempo_duracao;
                sscanf(line, "Run %*s I=%d D=%d", &momento_inicio, &tempo_duracao);
                novo_comando.momento_inicio = momento_inicio;
                novo_comando.tempo_duracao = tempo_duracao;
                novo_comando.tipo = 1; // RealTime
            } else {
                // É um comando RoundRobin
                novo_comando.momento_inicio = 0; // Valor padrão para RoundRobin
                novo_comando.tempo_duracao = 0; // Valor padrão para RoundRobin
                novo_comando.tipo = 0; // RoundRobin
            }

            comandos[comando_idx] = novo_comando;
            comando_idx++;
        }
    }

    // Agora, você tem um array com todos os comandos (RealTime e RoundRobin) na memória compartilhada
    for (int i = 0; i < num_comandos; i++) {
        if (comandos[i].tipo == 1) {
            printf("RealTime - Nome: %s, Início: %d, Duração: %d\n", comandos[i].nome_programa, comandos[i].momento_inicio, comandos[i].tempo_duracao);
        }
        else {
            printf("RoundRobin - Nome: %s\n", comandos[i].nome_programa);
        }
    }

    fclose(arquivo); // Fecha o arquivo quando terminar de usá-lo

    return 0;
}
