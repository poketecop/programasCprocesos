#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include "shared_memory.h"

int main(int argc, char *argv[]) {
    printf("PROGRAMA 1: Proceso padre creado con PID: %d, instante de creación: %ld\n", getpid(), time(NULL));

    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    int vecesSincronizacion = atoi(argv[1]);
    
    int semid = semget(SEM_KEY, 6, 0666); // Obtener el semáforo existente

    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("PROGRAMA 1: Error al crear el primer proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        printf("PROGRAMA 1: Primer proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

        int shmid = get_shared_memory(SHM_KEY1);
        shared_data *data = attach_shared_memory(shmid);

        srand(time(NULL) ^ (getpid()<<16));
        for (int i = 0; i < vecesSincronizacion; i++) {
            data->number = rand() % 10 + 1;
            printf("PROGRAMA 1: Primer proceso hijo escribe: %d\n", data->number);
            // Se usan los semáforos 0 y 1 para que no se pisen
            // Poner el semáforo 0 en verde
            signal_semaphore(semid, 0);
            // Esperar a que el semáforo 1 esté en verde
            wait_semaphore(semid, 1);
            printf("PROGRAMA 1: Primer proceso hijo lee respuesta: %d\n", data->response);
        }
        detach_shared_memory(data);
        exit(0);
    } else {
        pid_t pid2 = fork();
        if (pid2 < 0) {
            perror("PROGRAMA 1: Error al crear el segundo proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            printf("PROGRAMA 1: Segundo proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());

            int shmid = get_shared_memory(SHM_KEY3);
            shared_data *data = attach_shared_memory(shmid);

            for (int i = 0; i < vecesSincronizacion; i++) {
                // Esperar a que el semáforo 4 esté en verde
                wait_semaphore(semid, 4);
                printf("PROGRAMA 1: Segundo proceso hijo lee: %d\n", data->number);
                data->response = data->number * 2;
                printf("PROGRAMA 1: Segundo proceso hijo escribe respuesta: %d\n", data->response);
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