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

int main(int argc, char *argv[]) {
    // El proceso padre mostrará un mensaje por pantalla en el que indique su PID e instante de creación
    printf("CREACION: PROGRAMA 2: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    int vecesSincronizacion = atoi(argv[1]);

    int semid = semget(SEM_KEY, 3, 0666); // Obtener el semáforo existente

    if (semid == -1) {
        perror("Error al obtener el semáforo");
        exit(1);
    }

    // Crear el 1er proceso hijo
    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("CREACION: PROGRAMA 2: Error al r el 1er proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Este es el 1er proceso hijo
        printf("CREACION: PROGRAMA 2: 1er proceso hijo do con PID: %d, PID del padre: %d\n", getpid(), getppid());

        time_t t;
        int i;
        for (i = 0; i < vecesSincronizacion; i++) {
           t = time(NULL);

            // Esperar a que el semáforo 1 esté en verde
            wait_semaphore(semid, 1);

            printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 1er proceso hijo con PID: %d ha esperado: %ld segundos\n", i, getpid(), time(NULL) - t);
        }
        
        exit(0);
    } else {
        // Crear el 2do proceso hijo
        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("CREACION: PROGRAMA 2: Error al r el 2do proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            // Este es el 2do proceso hijo
            printf("CREACION: PROGRAMA 2: 2do proceso hijo do con PID: %d, PID del padre: %d\n", getpid(), getppid());
            
            // Semilla para el generador de números aleatorios
            srand(time(NULL) ^ (getpid()<<16));

            int wait_time;
            int i;
            for (i = 0; i < vecesSincronizacion; i++) {
                wait_time = (rand() % 4) + 2;

                printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 2do proceso hijo con PID: %d va a esperar: %d segundos\n", i, getpid(), wait_time);

                sleep(wait_time);

                // Poner en verde el semáforo 0
                signal_semaphore(semid, 0);
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