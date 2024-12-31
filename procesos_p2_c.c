#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    printf("CREACION: PROGRAMA 2: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    int vecesSincronizacion = atoi(argv[1]);
    
    int semid = semget(SEM_KEY, 6, 0666); // Obtener el semáforo existente

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("CREACION: PROGRAMA 2: Error al r el 1er proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        printf("CREACION: PROGRAMA 2: 1er proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

        int shmid = get_shared_memory(SHM_KEY2);
        shared_data *data = attach_shared_memory(shmid);

        srand(time(NULL) ^ (getpid()<<16));
        int i; 
        for (i = 0; i < vecesSincronizacion; i++) {
            data->number = rand() % 10 + 1;
            printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 1er proceso hijo escribe: %d\n", i, data->number);
            // Se usan los semáforos 2 y 3 para que no se pisen
            // Poner el semáforo 2 en verde
            signal_semaphore(semid, 2);
            // Esperar a que el semáforo 3 esté en verde
            wait_semaphore(semid, 3);
            printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 1er proceso hijo lee respuesta: %d\n", i, data->response);
        }
        detach_shared_memory(data);
        exit(0);
    } else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("CREACION: PROGRAMA 2: Error al r el 2do proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            printf("CREACION: PROGRAMA 2: 2do proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

            int shmid = get_shared_memory(SHM_KEY1);
            shared_data *data = attach_shared_memory(shmid);

            int i;
            for (i = 0; i < vecesSincronizacion; i++) {
                // Esperar a que el semáforo 0 esté en verde
                wait_semaphore(semid, 0);
                printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 2do proceso hijo lee: %d\n", i, data->number);
                data->response = data->number * 2;
                printf("COMUNICACION: ITERACIÓN %d: PROGRAMA 2: 2do proceso hijo escribe respuesta: %d\n", i, data->response);
                // Poner el semáforo 1 en verde
                signal_semaphore(semid, 1);
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