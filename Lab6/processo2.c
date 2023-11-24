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

    // Obtém o ID da fila de mensagens
    msgid = msgget(key, 0666 | IPC_CREAT);

    struct msg_buffer message;
    message.msg_type = 2;

    // Recebe e responde às 128 mensagens
    for (int i = 1; i <= 128; ++i) {
        // Recebe a mensagem do processo 1
        msgrcv(msgid, &message, sizeof(message), 1, 0);

        printf("Processo 2 recebeu: %s\n", message.msg_text);

        // Modifica o tipo de mensagem para enviar de volta ao processo 1
        message.msg_type = 2;

        // Envia uma resposta para o processo 1
        msgsnd(msgid, &message, sizeof(message), 0);

        // Limpa a estrutura de mensagem antes de reutilizá-la
        memset(&message, 0, sizeof(message));
        message.msg_type = 2;
    }

    return 0;
}
