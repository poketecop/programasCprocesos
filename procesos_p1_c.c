#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    printf("CREACION: PROGRAMA 1: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    int vecesSincronizacion = atoi(argv[1]);
    
    int semid = semget(SEM_KEY, 6, 0666); // Obtener el semáforo existente

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("CREACION: PROGRAMA 1: Error al r el 1er proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        printf("CREACION: PROGRAMA 1: 1er proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

        // Obtener y conectar a la memoria compartida correspondiente
        int shmid = get_shared_memory(SHM_KEY1);
        shared_data *data = attach_shared_memory(shmid);

        // Semilla para el generador de números aleatorios
        srand(time(NULL) ^ (getpid()<<16));
        int i;
        for (i = 0; i < vecesSincronizacion; i++) {
            data->number = rand() % 10 + 1;
            printf("COMUNICACION: ITERACION %d: PROGRAMA 1: 1er proceso hijo escribe: %d\n", i, data->number);
            // Se usan los semáforos 0 y 1 para que no se pisen
            // Poner el semáforo 0 en verde
            signal_semaphore(semid, 0);
            // Esperar a que el semáforo 1 esté en verde
            wait_semaphore(semid, 1);
            printf("COMUNICACION: ITERACION %d: PROGRAMA 1: 1er proceso hijo lee respuesta: %d\n", i, data->response);
        }
        detach_shared_memory(data);
        exit(0);
    } else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("CREACION: PROGRAMA 1: Error al r el 2do proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            printf("CREACION: PROGRAMA 1: 2do proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

            // Obtener y conectar a la memoria compartida correspondiente
            int shmid = get_shared_memory(SHM_KEY3);
            shared_data *data = attach_shared_memory(shmid);

            int i;
            for (i = 0; i < vecesSincronizacion; i++) {
                // Esperar a que el semáforo 4 esté en verde
                wait_semaphore(semid, 4);
                printf("COMUNICACION: ITERACION %d: PROGRAMA 1: 2do proceso hijo lee: %d\n", i, data->number);
                data->response = data->number * 2;
                printf("COMUNICACION: ITERACION %d: PROGRAMA 1: 2do proceso hijo escribe respuesta: %d\n", i, data->response);
                // Poner el semáforo 5 en verde
                signal_semaphore(semid, 5);
            }
            detach_shared_memory(data);
            exit(0);
        } else {
            wait(NULL);
            wait(NULL);
        }
    }

    return 0;
}