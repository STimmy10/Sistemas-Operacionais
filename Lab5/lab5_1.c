#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>   //adiciona flags adicionais para mmap
#include <fcntl.h>      //define O_CREATE
#include <semaphore.h>
#include <unistd.h>
#include <sys/stat.h>
#define NUM_MENS 128
#define BUFFER_SIZE 2048
#define SEM_NAME1 "/sem1_name"
#define SEM_NAME2 "/sem2_name"
#define SEM_GERAL "shared_sem"


int main(void){
    sem_t *sem1;
    sem_t *sem2;
    int sem_Geral;
    void *sharedMem_ptr;
    int count = 0;

    sem_Geral = shm_open(SEM_GERAL, O_CREAT | O_RDWR, 0666);
    /*
        FLAG O_RDWR utilizada para abrir a regiao com permissão de leitura e de escrita
    */
    if(sem_Geral == -1){
        perror("sem_open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(sem_Geral, BUFFER_SIZE) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    sem1 = sem_open(SEM_NAME1, O_CREAT, 0666, 0);
    if (sem1 == SEM_FAILED) {
        perror("sem_open1");
        exit(EXIT_FAILURE);
    }

    sem2 = sem_open(SEM_NAME2, O_CREAT, 0666, 0);
    if (sem2 == SEM_FAILED) {
        perror("sem_open2");
        exit(EXIT_FAILURE);
    }

    sharedMem_ptr = mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sem_Geral,0 );
    if (sharedMem_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();

    if(pid == -1){
        perror("Process Error");
        exit(EXIT_FAILURE);
    }else if(pid ==0){ //processo filho

        for(count =  0; count < NUM_MENS; count++){
            sem_wait(sem1);
            printf("\t[FILHO]LEITURA> %s  \n",(char*)sharedMem_ptr);
            //fflush(stdout); 
            sem_post(sem2);
        }
    }else{
        for(count =  0; count < NUM_MENS ; count++){
            sprintf((char *) sharedMem_ptr, "Mensagem %d", count+1);
            printf("[PAI]ESCRITA> %s\n",(char*)sharedMem_ptr);
            //fflush(stdout); 
            
            sem_post(sem1);
            sem_wait(sem2);
        }
    }   
    
    munmap(sharedMem_ptr, BUFFER_SIZE); //Libera memoria compartilhada alocada
    close(sem_Geral);
    sem_unlink(SEM_GERAL);

    sem_close(sem1);
    sem_unlink(SEM_NAME1);

    sem_close(sem2);
    sem_unlink(SEM_NAME2);

    return 0;
}


//LINKS DE CONSULTA
/*
    https://stackoverflow.com/questions/21311080/linux-shared-memory-shmget-vs-mmap#:~:text=mmap%20method%20is%20a%20little,would%20suggest%20going%20with%20that.
    https://man7.org/linux/man-pages/man3/shm_open.3.html    
    https://stackoverflow.com/questions/68741707/what-is-the-different-of-using-fflushstdout-and-not-using-it
*/