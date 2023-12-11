//Aluno: Lucas Daibes Rachid de Lucena -2010796
//Aluno: Leo Lomardo - 2020201

typedef struct page Page;
typedef struct pageframe PF;

int findNextIns(PF *pf, int size);
int NRU(PF *pf, int pfSize);
int swap(PF *pf, Page *pt, int PTIndexOld, int PTIndexNew);
int LRU(PF *pt, int pfSize);
int changePage(PF *pf, Page* pt, int pfSize, char* criterio, int incertIndex);
void simVirtual(int totalMemory,int pageSize, char **argv);
