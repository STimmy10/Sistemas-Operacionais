#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <string.h>

#define BUFFER_SIZE 2048
#define NUM_MENS 128
#define MAX_MEN_NUM 8
#define SEM_NAME1 "/my_sem1"
#define SEM_NAME2 "/my_sem2"
#define SEM_MUTEX "/my_sem3"
#define SEM_GERAL "/sem_Geral1"

typedef struct {
    char buffer[MAX_MEN_NUM][15];
    int read_index;
    int write_index;
} shared_data;

int main() {
    int sem_Geral;
    sem_t *sem1;
    sem_t *sem2;
    sem_t *sem_mutex;
    shared_data *sharedMem_ptr;


    sem_Geral = shm_open(SEM_GERAL, O_CREAT | O_RDWR, 0666);

    if(sem_Geral == -1){
        perror("sem_open");
        exit(1);
    }

    if (ftruncate(sem_Geral, BUFFER_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    sharedMem_ptr = (shared_data *) mmap(NULL, BUFFER_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, sem_Geral, 0);
    if (sharedMem_ptr == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    sem1 = sem_open(SEM_NAME1, O_CREAT, 0666, 8);
    sem2 = sem_open(SEM_NAME2, O_CREAT, 0666, 0);
    sem_mutex = sem_open(SEM_MUTEX, O_CREAT, 0666, 1);


    pid_t pid = fork();
    if(pid == -1){
        perror("Process Error");
        exit(1);
    }else if(pid == 0){
        for(int i =  0; i < NUM_MENS; i++){
            sem_wait(sem2);
            sem_wait(sem_mutex);
            printf("[FILHO]LEITURA> %s\n", sharedMem_ptr->buffer[sharedMem_ptr->read_index]);
            sharedMem_ptr->read_index = (sharedMem_ptr->read_index + 1) % MAX_MEN_NUM;
            sem_post(sem_mutex);
            sem_post(sem1);
        }
    }else{
        for(int i =  0; i < NUM_MENS ; i++){
            sem_wait(sem1);
            sem_wait(sem_mutex);
            sprintf(sharedMem_ptr->buffer[sharedMem_ptr->write_index], "Mensagem %d", i+1);
            printf("[PAI]ESCRITA> %s\n", sharedMem_ptr->buffer[sharedMem_ptr->write_index]);
            sharedMem_ptr->write_index = (sharedMem_ptr->write_index + 1) % MAX_MEN_NUM;
            sem_post(sem_mutex);
            sem_post(sem2);
        }
    }   

    munmap(sharedMem_ptr, BUFFER_SIZE); 
    close(sem_Geral);
    shm_unlink(SEM_GERAL);

    sem_close(sem1);
    sem_unlink(SEM_NAME1);

    sem_close(sem2);
    sem_unlink(SEM_NAME2);

    sem_close(sem_mutex);
    sem_unlink(SEM_MUTEX);


    return 0;
}
