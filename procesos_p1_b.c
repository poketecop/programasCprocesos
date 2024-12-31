#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define MSG_KEY 1234

struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char *argv[]) {
    // El proceso padre mostrará un mensaje por pantalla en el que indique su PID e instante de creación
    printf("CREACION: PROGRAMA 1: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    int vecesSincronizacion = atoi(argv[1]);

    // Obtener la cola de mensajes
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("Error al obtener la cola de mensajes");
        exit(1);
    }

    // Crear el 1er proceso hijo
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("CREACION: PROGRAMA 1: Error al r el 1er proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Este es el 1er proceso hijo
        printf("CREACION: PROGRAMA 1: 1er proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
        
        struct msgbuf msg;
        time_t t;
        int i;
        for (i = 0; i < vecesSincronizacion; i++) {
            
            t = time(NULL);

            // Esperar por el mensaje de que el 2do hijo del programa 2 ha terminado
            if (msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0) == -1) {
                perror("Error al recibir mensaje");
                exit(1);
            }
            
            printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 1: 1er proceso hijo con PID: %d ha esperado: %ld segundos\n", i, getpid(), time(NULL) - t);
        }
        
        exit(0);
    } else {
        // Crear el 2do proceso hijo
        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("CREACION: PROGRAMA 1: Error al r el 2do proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            // Este es el 2do proceso hijo
            printf("CREACION: PROGRAMA 1: 2do proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
            
            // Semilla para el generador de números aleatorios
            srand(time(NULL) ^ (getpid()<<16));
            
            struct msgbuf msg;
            msg.mtype = 1;
            int wait_time;
            int i;
            for (i = 0; i < vecesSincronizacion; i++) {
                wait_time = (rand() % 4) + 2;

                printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 1: 2do proceso hijo con PID: %d va a esperar: %d segundos\n", i, getpid(), wait_time);

                sleep(wait_time);

                // Mandar mensaje de que el 2do hijo del programa 1 ha terminado
                if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
                    perror("Error al enviar mensaje");
                    exit(1);
                }
            }

            exit(0);
        } else {
            // Proceso padre
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}