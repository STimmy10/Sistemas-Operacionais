#include <sys/shm.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

#define NUM_TRAB 10
#define TAM_MAX 10000

int main(int argc, char*argv[]){
  int vetCompartilhado[TAM_MAX];
  pid_t workers[NUM_TRAB];

  //preenche vetor inicial com valor 5
  for(int i = 0; i< TAM_MAX; i++){
    vetCompartilhado[i] = 5;
  }
  
  // Criação dos processos trabalhadores
    for (int i = 0; i < NUM_TRAB; i++) {
        pid_t pid = fork();
        if (pid == 0) {
            // Processo filho
            worker(shared_array, i * chunk_size, (i + 1) * chunk_size);
            exit(0);
        } else if (pid < 0) {
            perror("fork");
            exit(1);
        } else {
            workers[i] = pid;
        }
    }
}
