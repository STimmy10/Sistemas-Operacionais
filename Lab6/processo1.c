#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 15

struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;

    // Cria uma chave única para a fila de mensagens
    key = ftok("processo1.c", 65);

    // Cria a fila de mensagens e obtém seu ID
    msgid = msgget(key, 0666 | IPC_CREAT);

    struct msg_buffer message;
    message.msg_type = 1;

    // Envia 128 mensagens
    for (int i = 1; i <= 128; i++) {
        sprintf(message.msg_text, "mensagem %d", i);

        // Envia a mensagem para o processo 2
        msgsnd(msgid, &message, sizeof(message), 0);

        // Aguarda a resposta do processo 2
        struct msg_buffer response;
        msgrcv(msgid, &response, sizeof(response), 2, 0);

        printf("Processo 1 enviou: %s\n", message.msg_text);
        printf("Processo 1 recebeu resposta: %s\n", response.msg_text);
    }

    // Remove a fila de mensagens
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}
