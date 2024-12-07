#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid1, pid2;
    time_t t;

    // Crear el primer proceso hijo
    pid1 = fork();

    if (pid1 < 0) {
        // Error al crear el proceso
        perror("PROGRAMA 2: Error al crear el primer proceso hijo");
        exit(1);
    } else if (pid1 == 0) {
        // Este es el primer proceso hijo
        printf("PROGRAMA 2: Primer proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
    } else {
        // Crear el segundo proceso hijo
        pid2 = fork();

        if (pid2 < 0) {
            // Error al crear el proceso
            perror("PROGRAMA 2: Error al crear el segundo proceso hijo");
            exit(1);
        } else if (pid2 == 0) {
            // Este es el segundo proceso hijo
            printf("PROGRAMA 2: Segundo proceso hijo creado con PID: %d, PID del padre: %d\n", getpid(), getppid());
        } else {
            // Este es el proceso padre
            t = time(NULL);
            printf("PROGRAMA 2: Proceso padre con PID: %d creado en el instante: %s", getpid(), ctime(&t));
        }
    }

    return 0;
}