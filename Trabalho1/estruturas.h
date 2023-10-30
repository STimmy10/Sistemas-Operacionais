#include <stdio.h>
#include <stdlib.h>

#define REAL_TIME 1
#define ROUND_ROBIN 0


typedef struct comando{
    char nome_programa[46];
    int momento_inicio;
    int tempo_duracao;
    int tipo; // 0 para RoundRobin, 1 para RealTime-
    int index;
    int pid;
} Comando;


typedef struct processoatual{
    int escalonado;
    Comando proc;
}ProcessoAtual;