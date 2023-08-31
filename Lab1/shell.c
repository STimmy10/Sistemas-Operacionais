#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char*argv[]){
    int status;
    while (TRUE) { /* repeat forever */
        type_prompt("@"); /* display prompt */
        read_command (command, parameters); /* input from terminal */
        if (fork() != 0) { /* fork off child process */
        /* Parent code */
            waitpid( -1, &status, 0); /* wait for child to exit */
        } else {
        /* Child code */
            execve (command, parameters, 0); /* execute command */
        }   
    }
}