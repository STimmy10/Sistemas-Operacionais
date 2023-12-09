//Aluno: Lucas Daibes Rachid de Lucena -2010796
//Aluno: Leo Lomardo - 2020201

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

typedef struct pageframe{
    int page_id; 
    int M;
    int R;
    int timeIn;
}PF;  

typedef struct page{
    int frame_index; 
}Page;

 
PF* create_pf(int pageSize, int totalMainMem){ 

    int size = (totalMainMem * 1024) / pageSize;
    printf("total de itens na page frame: %d\n",size);

    PF* memoria = (PF*) malloc(sizeof(PF) * size);

    for(int i = 0; i<size; i++){
        memoria[i].page_id = -1;
        memoria[i].M = 0;
        memoria[i].R = 0;
        memoria[i].timeIn = 0;
    }

    return memoria;
}

Page* create_pageTable(int pageSize){ 
 
    int totalPages = pow(2, 32 - (int)(ceil(log2(pageSize * 1024))));
    
    Page* pageTable = (Page*) malloc(sizeof(Page) * totalPages);
    
    for(int i = 0; i<totalPages; i++){
        pageTable[i].frame_index = -1;
    }

    return pageTable;
}

int find_next_insert(PF *pf, int size){ 

    for(int i =0; i< size; i++){
        if(pf[i].page_id == -1){
            return i;
        } 
    }
    return -1;
}

int NRU(PF *pf, int tam_pf){
    int tempo_ma = -1;
    int target_index;

    for(int i = 0; i<tam_pf; i++){ 
        if(tempo_ma != -1){
            if(pf[i].timeIn < tempo_ma){
              tempo_ma = pf[i].timeIn;
              target_index = i;  
            } 
        }
        else{
          tempo_ma = pf[i].timeIn;
          target_index = i;
        } 
    }
    return target_index;
}


int swap(PF *pf, Page *pt, int old_pageTableIndex, int new_pageTableIndex){

    int qg = pt[old_pageTableIndex].frame_index; 
    pt[old_pageTableIndex].frame_index = -1;
    pt[new_pageTableIndex].frame_index = qg;

    return qg;
}

int LRU(PF *pt, int tam_pf){

    int menos_referenciada = -1;
    int target_index;

    for(int i = 0; i < tam_pf; i++){
        if(menos_referenciada != -1){
            if(pt[i].R < menos_referenciada){
                menos_referenciada = pt[i].R;
                target_index = i;
            }

        }else{
            menos_referenciada = pt[i].R;
            target_index = i;
        }
    }

    return target_index;
}

int troca_de_paginas(PF *pf, Page* pt, int tam_pf, char* criterio, int to_insert_index){
    int target_index;

    if(strcmp(criterio,"LRU") == 0){ 
        target_index = LRU(pf,tam_pf);
    }
    else if(strcmp(criterio,"NRU") == 0){
        target_index = NRU(pf,tam_pf);
    }
    else{
        perror("ALGORITMO INVALIDO");
        exit(1);
    }

    return swap(pf,pt,target_index,to_insert_index); 
}


void go_simulator(int totalMemory, int pageSize, char **argv){ 
    unsigned int pageTableIndex, addr; 
    unsigned int next_insert = 0; 
    int countPFault = 0;
    int countPageW = 0;
    int tam_pf = 0; 
    int time = 0; 
    char rw;

    PF* pf = create_pf(pageSize,totalMemory);
    Page *pt = create_pageTable(pageSize);

    FILE *f = fopen(argv[2],"r");
    if(f == NULL){
        perror("ARQUIVO NAO ENCONTRADO\n");
        exit(1);
    }
    
    tam_pf = (totalMemory * 1024) / pageSize;

    while(fscanf(f,"%x %c\n", &addr, &rw) == 2){
        
        pageTableIndex = addr >> (int)(ceil(log2(pageSize*1024)));
    
        if(pt[pageTableIndex].frame_index == -1){ 
            countPFault++;
            next_insert = find_next_insert(pf,tam_pf); 

            if(next_insert != -1){ 
                pt[pageTableIndex].frame_index = next_insert;
                pf[next_insert].page_id = addr;
                pf[next_insert].timeIn = time;
                
                if(rw == 'W'){
                    pf[next_insert].M = 1;
                    countPageW++;
                }
            }
            else{
                int qg = troca_de_paginas(pf,pt,tam_pf,argv[1],pageTableIndex);
                pf[qg].page_id = addr;
                pf[qg].timeIn = time;

                if(rw == 'W'){
                    pf[qg].M = 1;
                    countPageW++;
                }
            }
        }else{
            pf[pt[pageTableIndex].frame_index].R++;
        }
    }
    time++;
    fclose(f);

    printf("Numero de Faltas de Paginas: %d\n" , countPFault);
    printf("Numero de Paginas Escritas: %d\n", countPageW);
}

