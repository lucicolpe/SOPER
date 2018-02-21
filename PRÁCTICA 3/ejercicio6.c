#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaforos.h"


#define MUTEX 0
#define VACIO 1
#define LLENO 2
#define SIZE 6
#define KEY 1300
#define FILEKEY "/bin/cat"


int main(){
    
    int i; 
    int pid_productor, pid_consumidor; 
    int semid; 
    int key; 
    int id_zone; 
    char* buffer;
    char c;
    char* s;
    unsigned short array[] = {1,SIZE-1,0}; 

    /*
    * Obtenemos la clave para poder crear la zona compartida
    */
    key = ftok(FILEKEY, KEY);
    if(key == -1){
    printf("Error al obtener key\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
                
    /*
    * Creacion de la zona compartida
    */            
    id_zone = shmget(key, SIZE, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if(id_zone == -1){
        id_zone = shmget(key, SIZE, IPC_CREAT | SHM_R | SHM_W);
        if(id_zone == -1){
            printf("Error al crear la zona de memoria compartida\n");
            exit(EXIT_FAILURE);
        }
    }

    /*
    * Creacion de semaforos
    */
    if(Crear_Semaforo(KEY, 3, &semid) == -1){
        perror("Error al crear el semaforo\n");
        shmdt(buffer);
        shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
        exit(EXIT_FAILURE);
    }

    /*
    * Inicializacion de los semaforos
    */
    if(Inicializar_Semaforo(semid, array) == ERROR){
        perror("Error al inicializar semáforos");
        shmdt(buffer);
        shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
        Borrar_Semaforo(semid);
        exit(EXIT_FAILURE);
    }

    /*
    * Creamos el primer proceso hijo que sera el productor
    */
    if((pid_productor = fork()) == -1){
        perror("Error al realizar el primer fork\n");
        shmdt(buffer);
        shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
        Borrar_Semaforo(semid);
        exit(EXIT_FAILURE);
    }
    
    /*
    * Trabajo que realiza el productor con semaforos 
    */
    if(pid_productor == 0){
        buffer = shmat(id_zone, (char*)0, SHM_R | SHM_W);
        if(buffer == NULL){
            printf("Error al unir la memoria\n");
            exit(EXIT_FAILURE);
        }

        s = buffer;        
        for(i = 1, c = 'a'; c <= 'z'; i++, c++){
            if(Down_Semaforo(semid, VACIO, 0) == ERROR){
                perror("Error al decrementar el semáforo vacio");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
            if(Down_Semaforo(semid, MUTEX, 0) == ERROR){
                perror("Error al decrementar el semáforo MUTEX");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }

            *s = c;
            s = buffer+i%SIZE;

            printf("El productor produce %c\n", c);
            if(Up_Semaforo(semid, MUTEX, 0) == ERROR){
                perror("Error al aumentar el semáforo MUTEX");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
            if(Up_Semaforo(semid, LLENO, 0) == ERROR){
                perror("Error al aumentar el semáforo lleno");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
        }
        exit(EXIT_SUCCESS);
    }
    
    /*
    * Creamos el segundo proceso hijo que sera el consumidor
    */
    if((pid_consumidor = fork()) == -1){
        perror("Error al realizar el segundo fork\n");
        kill(pid_productor, SIGKILL);
        wait(NULL);
        Borrar_Semaforo(semid);        
        shmdt(buffer);
        shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
        exit(EXIT_FAILURE);
    }
    
    /* 
    * Trabajo que realiza el consumidor con semaforos 
    */
    if(pid_consumidor == 0){
        buffer = shmat(id_zone, (char*)0, SHM_R | SHM_W);
        if(buffer == NULL){
            printf("Error al unir la memoria\n");
            exit(EXIT_FAILURE);
        }

        for(s = buffer, i = 1; *s <= 'z'; i++){
            if(Down_Semaforo(semid, LLENO, 0) == ERROR){
                perror("Error al decrementar el semáforo lleno");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
            if(Down_Semaforo(semid, MUTEX, 0) == ERROR){
                perror("Error al decrementar el semáforo MUTEX");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
            
            printf("%c ha sido consumido por el consumidor\n", *s);
            fflush(stdout);


            if(Up_Semaforo(semid, MUTEX, 0) == ERROR){
                perror("Error al aumentar el semáforo MUTEX");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }

            if(Up_Semaforo(semid, VACIO, 0) == ERROR){
                perror("Error al aumentar el semáforo vacio");
                shmdt(buffer);
                exit(EXIT_FAILURE);
            }
            if(*s == 'z'){
                break;
            }
            s = buffer+i%SIZE;
        }
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    wait(NULL);
    shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
    Borrar_Semaforo(semid);
    exit(EXIT_SUCCESS);
}