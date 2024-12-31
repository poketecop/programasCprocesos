// shared_memory.h
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

#define SHM_KEY1 1234
#define SHM_KEY2 5678
#define SHM_KEY3 91011

#define SEM_KEY 0x1234

typedef struct {
    int number;
    int response;
} shared_data;

int get_shared_memory(key_t key) {
    int shmid = shmget(key, sizeof(shared_data), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    return shmid;
}

shared_data* attach_shared_memory(int shmid) {
    shared_data *data = (shared_data*) shmat(shmid, NULL, 0);
    if (data == (void*) -1) {
        perror("shmat");
        exit(1);
    }
    return data;
}

void detach_shared_memory(shared_data *data) {
    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
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

#endif