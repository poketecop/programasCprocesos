#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#define SEM_KEY 0x1234

void init_semaphores(int semid) {
    // Inicializar los semáforos a 0
    for (int i = 0; i < 3; i++) {
        semctl(semid, i, SETVAL, 0);
    }
}

void wait_semaphore(int semid, int semnum) {
    struct sembuf sb = {semnum, -1, 0};
    semop(semid, &sb, 1);
}

void signal_semaphore(int semid, int semnum) {
    struct sembuf sb = {semnum, 1, 0};
    semop(semid, &sb, 1);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <N>\n", argv[0]);
        exit(1);
    }

    int N = atoi(argv[1]);
    int semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("Error al crear los semáforos");
        exit(1);
    }

    init_semaphores(semid);

    for (int i = 0; i < N; i++) {
        // Sincronización entre procesos
        signal_semaphore(semid, 0); // Desbloquear primer hijo del programa 1
        wait_semaphore(semid, 1);   // Esperar a segundo hijo del programa 2

        signal_semaphore(semid, 1); // Desbloquear primer hijo del programa 2
        wait_semaphore(semid, 2);   // Esperar a segundo hijo del programa 3

        signal_semaphore(semid, 2); // Desbloquear primer hijo del programa 3
        wait_semaphore(semid, 0);   // Esperar a segundo hijo del programa 1
    }

    // Eliminar los semáforos
    semctl(semid, 0, IPC_RMID, 0);

    return 0;
}