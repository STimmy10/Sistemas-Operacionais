//Aluno: Lucas Daibes Rachid de Lucena -2010796
//Aluno: Leo Lomardo - 2020201

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

typedef struct pageframe{
    int idPage; 
    int M;
    int R;
    int timeIn;
}PF;  

typedef struct page{
    int frameIndex; 
}Page;

 
PF* createPageFrame(int pageSize, int totalMainMem){ 

    int size = (totalMainMem * 1024) / pageSize;
    printf("total de itens na page frame: %d\n",size);

    PF* memoria = (PF*) malloc(sizeof(PF) * size);

    for(int i = 0; i<size; i++){
        memoria[i].idPage = -1;
        memoria[i].M = 0;
        memoria[i].R = 0;
        memoria[i].timeIn = 0;
    }

    return memoria;
}

Page* createPageTable(int pageSize){ 
 
    int totalPages = pow(2, 32 - (int)(ceil(log2(pageSize * 1024))));
    
    Page* pageTable = (Page*) malloc(sizeof(Page) * totalPages);
    
    for(int i = 0; i<totalPages; i++){
        pageTable[i].frameIndex = -1;
    }

    return pageTable;
}

int findNextIns(PF *pf, int size){ 

    for(int i =0; i< size; i++){
        if(pf[i].idPage == -1){
            return i;
        } 
    }
    return -1;
}

int NRU(PF *pf, int pfSize){
    int tempo_ma = -1;
    int target_index;

    for(int i = 0; i<pfSize; i++){ 
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


int swap(PF *pf, Page *pt, int PTIndexOld, int PTIndexNew){

    int qg = pt[PTIndexOld].frameIndex; 
    pt[PTIndexOld].frameIndex = -1;
    pt[PTIndexNew].frameIndex = qg;

    return qg;
}

int LRU(PF *pt, int pfSize){

    int menos_referenciada = -1;
    int target_index;

    for(int i = 0; i < pfSize; i++){
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

int changePage(PF *pf, Page* pt, int pfSize, char* criterio, int incertIndex){
    int target_index;

    if(strcmp(criterio,"LRU") == 0){ 
        target_index = LRU(pf,pfSize);
    }
    else if(strcmp(criterio,"NRU") == 0){
        target_index = NRU(pf,pfSize);
    }
    else{
        perror("ALGORITMO INVALIDO");
        exit(1);
    }

    return swap(pf,pt,target_index,incertIndex); 
}


void simVirtual(int totalMemory, int pageSize, char **argv){ 
    unsigned int pageTableIndex, addr; 
    unsigned int next_insert = 0; 
    int countPFault = 0;
    int countPageW = 0;
    int pfSize = 0; 
    int time = 0; 
    char rw;

    PF* pf = createPageFrame(pageSize,totalMemory);
    Page *pt = createPageTable(pageSize);

    FILE *arq = fopen(argv[2],"r");
    if(arq == NULL){
        perror("ARQUIVO NAO ENCONTRADO\n");
        exit(1);
    }
    
    pfSize = (totalMemory * 1024) / pageSize;

    while(fscanf(arq,"%x %c\n", &addr, &rw) == 2){
        
        pageTableIndex = addr >> (int)(ceil(log2(pageSize*1024)));
    
        if(pt[pageTableIndex].frameIndex == -1){ 
            countPFault++;
            next_insert = findNextIns(pf,pfSize); 

            if(next_insert != -1){ 
                pt[pageTableIndex].frameIndex = next_insert;
                pf[next_insert].idPage = addr;
                pf[next_insert].timeIn = time;
                
                if(rw == 'W'){
                    pf[next_insert].M = 1;
                    countPageW++;
                }
            }
            else{
                int qg = changePage(pf,pt,pfSize,argv[1],pageTableIndex);
                pf[qg].idPage = addr;
                pf[qg].timeIn = time;

                if(rw == 'W'){
                    pf[qg].M = 1;
                    countPageW++;
                }
            }
        }else{
            pf[pt[pageTableIndex].frameIndex].R++;
        }
    }
    time++;
    fclose(arq);

    printf("Numero de Faltas de Paginas: %d\n" , countPFault);
    printf("Numero de Paginas Escritas: %d\n", countPageW);
}

