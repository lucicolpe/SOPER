#include <stdio.h>
#include <stdlib.h>
#include "semaforos.h"

#define IPC_WAIT 0

int inicializar_semaforo(int semid, unsigned short *array) {
    union semun aux;
    aux.array = (ushort_t *) array;
    if(semctl(semid, 0, SETALL, aux) == -1) {
        return ERROR;
    }
    return OK;
}

int borrar_semaforo(int semid) {
    if(semctl(semid, 0, IPC_RMID) == -1) {
        return ERROR;
    }
    return OK;
}

int crear_semaforo(key_t key, int size, int *semid) {
    int i;
    int id;
    union semun init;
    init.array = (ushort_t *) malloc(size*sizeof(ushort_t));
    if(init.array == NULL) {
        fprintf(stderr, "Error with allocating memory");
        return ERROR;
    }
    id = semget(key, size, IPC_CREAT | IPC_EXCL| SHM_R | SHM_W); 
    if(id == -1) {
        if(errno == EEXIST) {
            return 1;
        } else {
            return ERROR;
        }
    }
    for(i = 0; i < size; i++) {
        init.array[i] = 0;
    }
    semctl(id, 0, SETALL, init);
    *semid = id;
    free(init.array);
    return 0;
}

int down_semaforo(int id, int num_sem, int undo) {
    struct sembuf sops;
    sops.sem_num = num_sem;
    sops.sem_op = -1;
    if(undo) {
        sops.sem_flg = SEM_UNDO;
    } else {
        sops.sem_flg = IPC_WAIT;
    }
    if(semop(id, &sops, 1) == -1) {
        return ERROR;
    }
    return OK;
}

int down_multiple_semaforo(int id,int size,int undo, int *active) {
    struct sembuf *sops;
    int i;
    sops = (struct sembuf *) malloc(size*sizeof(struct sembuf));
    if(sops == NULL) {
        fprintf(stderr, "Error al reservar memoria");
        return ERROR;
    }
    for(i = 0; i < size; i++) {
        sops[i].sem_num = active[i];
        sops[i].sem_op = -1;
        if(undo) {
            sops[i].sem_flg = SEM_UNDO;
        } else {
            sops[i].sem_flg = IPC_WAIT;
        }
    }
    if(semop(id, sops, size) == -1) {
        return ERROR;
    }
    free(sops);
    return OK;
}

int up_semaforo(int id, int num_sem, int undo) {
    struct sembuf sops;
    sops.sem_num = num_sem;
    sops.sem_op = 1;
    if(undo) {
        sops.sem_flg = SEM_UNDO;
    } else {
        sops.sem_flg = IPC_WAIT;
    }
    if(semop(id, &sops, 1) == -1) {
        return ERROR;
    }
    return OK;
}

int up_multiple_semaforo(int id,int size, int undo, int *active) {
    struct sembuf *sops;
    int i;
    sops = (struct sembuf *) malloc(size*sizeof(struct sembuf));
    if(sops == NULL) {
        fprintf(stderr, "Error al reservar memoria");
        return ERROR;
    }
    for(i = 0; i < size; i++) {
        sops[i].sem_num = active[i];
        sops[i].sem_op = 1;
        if(undo) {
            sops[i].sem_flg = SEM_UNDO;
        } else {
            sops[i].sem_flg = IPC_WAIT;
        }
    }
    if(semop(id, sops, size) == -1) {
        return ERROR;
    }
    free(sops);
    return OK;
}
