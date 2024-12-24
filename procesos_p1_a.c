#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

int main() {

    pid_t pid1, pid2;
    time_t t;

    // Crear el primer proceso hijo
    pid1 = fork();

    if (pid1 < 0) {
        perror("PROGRAMA 1: Error al crear el primer proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Este es el primer proceso hijo
        printf("PROGRAMA 1: Primer proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
        exit(0);
    } else {
        // Crear el segundo proceso hijo
        pid2 = fork();

        if (pid2 < 0) {
            perror("PROGRAMA 1: Error al crear el segundo proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            // Este es el segundo proceso hijo
            printf("PROGRAMA 1: Segundo proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
            srand(time(NULL) ^ (getpid()<<16));
            int wait_time = (rand() % 4) + 2;
            printf("PROGRAMA 1: Segundo proceso hijo con PID: %d va a esperar: %d segundos\n", getpid(), wait_time);
            sleep(wait_time);
            exit(0);
        } else {
            // Proceso padre
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}