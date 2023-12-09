//Aluno: Lucas Daibes Rachid de Lucena -2010796
//Aluno: Leo Lomardo - 2020201

typedef struct page Page;
typedef struct pageframe PF;

int find_next_insert(PF *pf, int size);
int NRU(PF *pf, int tam_pf);
int swap(PF *pf, Page *pt, int old_pt_index, int new_pt_index);
int LRU(PF *pt, int tam_pf);
int troca_de_paginas(PF *pf, Page* pt, int tam_pf, char* criterio, int to_insert_index);
void go_simulator(int totalMemory,int pageSize, char **argv);
