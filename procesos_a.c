#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <time.h>

#define SEM_KEY 0x1234

void init_semaphore(int semid, int semnum) {
    semctl(semid, semnum, SETVAL, 0);
}

int main(int argc, char *argv[]) {
    int semid = semget(SEM_KEY, 3, IPC_CREAT | 0666);

    if (semid == -1) {
        perror("Error al crear los semáforos");
        exit(1);
    }

    init_semaphore(semid, 1);
    init_semaphore(semid, 2);
    init_semaphore(semid, 3);

    // Compilar los 3 programas de procesos
    system("gcc -o procesos_p1_a procesos_p1_a.c");
    system("gcc -o procesos_p2_a procesos_p2_a.c");
    system("gcc -o procesos_p3_a procesos_p3_a.c");

    // Ejecutar los 3 programas de procesos asíncronamente
    pid_t pid1 = fork();
    if (pid1 == 0) {
        execl("./procesos_p1_a", "procesos_p1_a", (char *)NULL);
        perror("Error al ejecutar procesos_p1_a");
        exit(1);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        execl("./procesos_p2_a", "procesos_p2_a", (char *)NULL);
        perror("Error al ejecutar procesos_p2_a");
        exit(1);
    }

    pid_t pid3 = fork();
    if (pid3 == 0) {
        execl("./procesos_p3_a", "procesos_p3_a", (char *)NULL);
        perror("Error al ejecutar procesos_p3_a");
        exit(1);
    }

    // Esperar a que los procesos hijos terminen
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}