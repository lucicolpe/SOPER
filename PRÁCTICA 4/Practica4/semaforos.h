#ifndef SEMAFOROS_H
#define SEMAFOROS_H

#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define OK 0
#define ERROR -1

typedef unsigned short ushort_t;

union semun {
    int val;
    struct semid_ds *buf;
    ushort_t *array;
};

/**
 * @brief Inicializa los semáforos indicados.
 * 
 * Inicializa un array de semáforos al valor del array.
 * 
 * @param semid Identificador del array de semáforos
 * @param array Valores iniciales
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int inicializar_semaforo(int semid, unsigned short *array); 

/**
 * @brief Borra un semáforo.
 * 
 * Borra un array de semáforos.
 * 
 * @param semid Identificador del array de semáforos
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int borrar_semaforo(int semid); 

/**
 * @brief Crea un semáforo.
 * 
 * Crea un semáforo con la clave y el tamaño especificado
 * y lo inicializa a 0.
 * 
 * @param key Clave precompartida del semaforo
 * @param size Tamaño del semaforo
 * @param semid Puntero al entero que almacenará el identificador del semáforo
 * @return ERROR en caso de error, 0 si el semáforo ha sido creado y 1 si ya existía.
 */
int crear_semaforo(key_t key, int size, int *semid); 

/**
 * @brief Baja un semáforo.
 * 
 * Baja el semáforo del array indicado con el índice correspondiente.
 * 
 * @param semid Identificador del semáforo
 * @param num_sem Índice del semáforo a bajar
 * @param undo Flag de modo persistente pese a finalización abrupta
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int down_semaforo(int id, int num_sem, int undo); 

/**
 * @brief Baja todos los semáforos del array introducido.
 * 
 * Baja los semáforos del array indicado con índices en active.
 * 
 * @param semid Identificador del array de semáforos
 * @param num_sem Número de semáforos del array
 * @param undo Flag de modo persistente pese a finalización abrupta
 * @param active Semáforos involucrados
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int down_multiple_semaforo(int id,int size,int undo, int *active); 

/**
 * @brief Sube un semáforo.
 * 
 * Sube el semaforo del array indicado con el índice correspondiente.
 * 
 * @param semid Identificador del semáforo
 * @param num_sem Índice del semáforo a subir
 * @param undo Flag de modo persistente pese a finalización abrupta
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int up_semaforo(int id, int num_sem, int undo); 

/**
 * @brief Sube todos los semáforos del array introducido.
 * 
 * Sube los semáforos del array indicado con índices en active.
 * 
 * @param semid Identificador del array de semáforos
 * @param num_sem Número de semáforos del array
 * @param undo Flag de modo persistente pese a finalización abrupta
 * @param active Semáforos involucrados
 * @return OK si todo fue correcto, ERROR en caso de error.
 */
int up_multiple_semaforo(int id,int size, int undo, int *active); 

#endif
