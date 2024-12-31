#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>
#include "shared_memory.h"

int create_shared_memory(key_t key) {
    int shmid = shmget(key, sizeof(shared_data), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

int init_semaphore(int semid, int semnum) {
    semctl(semid, semnum, SETVAL, 0);
}

int remove_semaphore(int semid) {
    semctl(semid, 0, IPC_RMID);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <integer>\n", argv[0]);
        return 1;
    }

    // Usamos 6 semáforos ya que son 3 sincronizaciones independientes y además
    // vamos a usar 2 semáforos para cada sincronización para evitar que se pisen
    int semid = semget(SEM_KEY, 6, IPC_CREAT | 0666);

    if (semid == -1) {
        perror("Error al crear los semáforos");
        exit(1);
    }

    init_semaphore(semid, 0);
    init_semaphore(semid, 1);
    init_semaphore(semid, 2);
    init_semaphore(semid, 3);
    init_semaphore(semid, 4);
    init_semaphore(semid, 5);

    // Crear las 3 memorias compartidas. Una para cada comunicación
    create_shared_memory(SHM_KEY1);
    create_shared_memory(SHM_KEY2);
    create_shared_memory(SHM_KEY3);

    // Compilar los 3 programas de procesos
    system("gcc -o procesos_p1_c procesos_p1_c.c");
    system("gcc -o procesos_p2_c procesos_p2_c.c");
    system("gcc -o procesos_p3_c procesos_p3_c.c");

    // Ejecutar los 3 programas de procesos asíncronamente
    pid_t pid1 = fork();
    if (pid1 == 0) {
        execl("./procesos_p1_c", "procesos_p1_c", argv[1], NULL);
        perror("Error al ejecutar procesos_p1_c");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        execl("./procesos_p2_c", "procesos_p2_c", argv[1], NULL);
        perror("Error al ejecutar procesos_p2_c");
        exit(1);
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        execl("./procesos_p3_c", "procesos_p3_c", argv[1], NULL);
        perror("Error al ejecutar procesos_p3_c");
        exit(1);
    }

    // Esperar a que los procesos hijos terminen
    wait(NULL);
    wait(NULL);
    wait(NULL);

    // Eliminar los semáforos
    remove_semaphore(semid);

    return 0;
}