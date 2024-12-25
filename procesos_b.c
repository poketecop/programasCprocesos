#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#define MSG_KEY 1234

struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    // Compilar los 3 programas de procesos
    system("gcc -o procesos_p1_b procesos_p1_b.c");
    system("gcc -o procesos_p2_b procesos_p2_b.c");
    system("gcc -o procesos_p3_b procesos_p3_b.c");

    // Crear la cola de mensajes
    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("Error al crear la cola de mensajes");
        return 1;
    }

    // Ejecutar los 3 programas de procesos asíncronamente
    pid_t pid1 = fork();
    if (pid1 == 0) {
        execl("./procesos_p1_b", "procesos_p1_b", argv[1], NULL);
        perror("Error al ejecutar procesos_p1_b");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        execl("./procesos_p2_b", "procesos_p2_b", argv[1], NULL);
        perror("Error al ejecutar procesos_p2_b");
        exit(1);
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        execl("./procesos_p3_b", "procesos_p3_b", argv[1], NULL);
        perror("Error al ejecutar procesos_p3_b");
        exit(1);
    }

    // Esperar a que los procesos hijos terminen
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Eliminar la cola de mensajes
    msgctl(msgid, IPC_RMID, NULL);

    return 0;
}