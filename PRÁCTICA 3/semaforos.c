/**
 * @brief Implementa el ejercicio 4 de la biblioteca de semaforos
 * @file semaforos.c
 * @author Lucia Colmenarejo Perez lucia.colmenarejo@estudiante.uam.es
 * @author Jesus Daniel Franco Lopez jesusdaniel.francolopez@estudiante.uam.es
 * @note Grupo 2201
 * @version 1.0
 * @date 06/04/2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include "semaforos.h"
#define OK       1
#define ERROR    -1

/**
* @brief Inicializa los semaforos indicados.
* @param semid: Identificador del semaforo.
* @param array: Valores iniciales.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int Inicializar_Semaforo(int semid, unsigned short *array){

  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } arg;

  if (semid < 0 || !array){
    return ERROR;
  } 
  arg.array = array;

  if(semctl(semid, 0, SETALL, arg) == -1){
    return ERROR;
  }
  return OK;
}

/**
* @brief Borra un semaforo.
* @param semid: Identificador del semaforo.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int Borrar_Semaforo(int semid){
  if(semid<0){
    return ERROR;
  }
  if (semctl(semid, 0, IPC_RMID, 0) == -1) {
          return ERROR;
  }
  return OK;
}


/**
* @brief Crea un semaforo con la clave y el tamaño 
*   especificado. Lo inicializa a 0.
* @param key: Clave precompartida del semaforo.
* @param size: Tamaño del semaforo.
* @param semid: Identificador del semaforo.
* @return int: ERROR en caso de error, 0 si ha creado el semaforo, 
*   1 si ya estaba creado.
*/


int Crear_Semaforo(key_t key, int size, int *semid){
  int i;

  union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
  } arg;

  *semid = semget(key, size, IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
  if ((*semid == ERROR) &&(errno == EEXIST)){
      *semid = semget(key, size, SHM_R | SHM_W);
      if (*semid == ERROR) {
        return ERROR;
      }
      return 1;
  }
  
  if (*semid == ERROR) {
    return ERROR;
  }

  arg.array = (unsigned short *) malloc(sizeof(unsigned short) * size);
  if (!arg.array){
    return ERROR;
  }

  for(i=0; i<size; i++){
      arg.array[i] = 0;
  }

  if(semctl(*semid, 0, SETALL, arg) == -1){
    return ERROR;
  }
  free(arg.array);
  return 0;

}



/**
* @brief Baja todos los semaforos del array indicado por active.
* @param semid: Identificador del semaforo.
* @param size: Numero de semaforos del array.
* @param undo: Flag de modo persistente pese a finalización
*   abrupta.
* @param active: Semaforos involucrados.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int Down_Semaforo(int semid, int num_sem, int undo){
  struct sembuf sem_oper;

  sem_oper.sem_num = num_sem;
  sem_oper.sem_op = -1;
  sem_oper.sem_flg = undo;

  if (semop(semid, &sem_oper, 1) == -1)
    return ERROR;

  return OK;
}

/**
* @brief Baja el semaforo indicado
* @param semid: Identificador del semaforo.
* @param num_sem: Semaforo dentro del array.
* @param undo: Flag de modo persistente pese a finalización
*   abrupta.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int DownMultiple_Semaforo(int semid, int size, int undo, int *active){
  int i, a;
  for(i=0; i<size; i++){
    a = Down_Semaforo(semid, active[i], undo);
    if (a == ERROR){
      return ERROR;
    }
  }
  return OK;
}

/**
* @brief Sube el semaforo indicado
* @param semid: Identificador del semaforo.
* @param num_sem: Semaforo dentro del array.
* @param undo: Flag de modo persistente pese a finalizacion
*   abupta.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int Up_Semaforo(int semid, int num_sem, int undo){
  struct sembuf sem_oper;

  sem_oper.sem_num = num_sem;
  sem_oper.sem_op = 1;
  sem_oper.sem_flg = undo;

  if (semop(semid, &sem_oper, 1) == -1)
    return ERROR;

  return OK;
}

/**
* @brief Sube todos los semaforos del array indicado por active.
* @param semid: Identificador del semaforo.
* @param size: Numero de semaforos del array.
* @param undo: Flag de modo persistente pese a finalización
*   abrupta.
* @param active: Semaforos involucrados.
* @return int: OK si todo fue correcto, ERROR en caso de error.
*/

int UpMultiple_Semaforo(int semid, int size, int undo, int *active){
  int i, a;
  for(i=0; i<size; i++){
    a = Up_Semaforo(semid, active[i], undo);
    if (a == ERROR){
      return ERROR;
    }
  }
  return OK;
}


