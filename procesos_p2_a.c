#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define SEM_KEY 0x1234

void wait_semaphore(int semid, int semnum) {
    struct sembuf sb = {semnum, -1, 0};
    semop(semid, &sb, 1);
}

void signal_semaphore(int semid, int semnum) {
    struct sembuf sb = {semnum, 1, 0};
    semop(semid, &sb, 1);
}

int main() {

    // El proceso padre mostrará un mensaje por pantalla en el que indique su PID e instante de creación
    printf("PROGRAMA 2: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    int semid = semget(SEM_KEY, 3, 0666); // Obtener el semáforo existente

    pid_t pid1, pid2;
    time_t t;

    // Crear el primer proceso hijo
    pid1 = fork();

    if (pid1 < 0) {
        perror("PROGRAMA 2: Error al crear el primer proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Este es el primer proceso hijo
        printf("PROGRAMA 2: Primer proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

        t = time(NULL);

        // Esperar a que el semáforo 2 esté en verde
        wait_semaphore(semid, 2);

        printf("PROGRAMA 2: Primer proceso hijo con PID: %d ha esperado: %ld segundos\n", getpid(), time(NULL) - t);

        exit(0);
    } else {
        // Crear el segundo proceso hijo
        pid2 = fork();

        if (pid2 < 0) {
            perror("PROGRAMA 2: Error al crear el segundo proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            // Este es el segundo proceso hijo
            printf("PROGRAMA 2: Segundo proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
            srand(time(NULL) ^ (getpid()<<16));
            int wait_time = (rand() % 4) + 2;
            printf("PROGRAMA 2: Segundo proceso hijo con PID: %d va a esperar: %d segundos\n", getpid(), wait_time);
            sleep(wait_time);

            // Poner en verde el semáforo 1
            signal_semaphore(semid, 1);

            exit(0);
        } else {
            // Proceso padre
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}