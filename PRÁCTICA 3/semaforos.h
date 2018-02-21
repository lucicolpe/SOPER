/**
 * @brief Implementa el ejercicio 4 de la biblioteca de semaforos
 * @file semaforos.c
 * @author Lucia Colmenarejo Perez lucia.colmenarejo@estudiante.uam.es
 * @author Jesus Daniel Franco Lopez jesusdaniel.francolopez@estudiante.uam.es
 * @note Grupo 2201
 * @version 1.0
 * @date 06/04/2017
 */

#ifndef SEMAFOROS_H
#include <stdio.h>
#include <stdlib.h>
#define OK       1
#define ERROR    -1


/***************************************************************
   Nombre:
   Inicializar_Semaforo.
   Descripcion:
   Inicializa los semaforos indicados.
   Entrada:
   int semid: Identificador del semaforo.
   unsigned short *array: Valores iniciales.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
 ************************************************************/
int Inicializar_Semaforo(int semid, unsigned short *array);
/***************************************************************
   Nombre: Borrar_Semaforo.
   Descripcion: Borra un semaforo.
   Entrada:
   int semid: Identificador del semaforo.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int Borrar_Semaforo(int semid);
/***************************************************************
   Nombre: Crear_Semaforo.Descripcion: Crea un semaforo con la clave y el tamaño
   especificado. Lo inicializa a 0.
   Entrada:
   key_t key: Clave precompartida del semaforo.
   int size: Tamaño del semaforo.
   Salida:
   int *semid: Identificador del semaforo.
   int: ERROR en caso de error,
   0 si ha creado el semaforo,
   1 si ya estaba creado.
 **************************************************************/
int Crear_Semaforo(key_t key, int size, int *semid);
/**************************************************************
   Nombre:Down_Semaforo
   Descripcion: Baja el semaforo indicado
   Entrada:
   int semid: Identificador del semaforo.
   int num_sem: Semaforo dentro del array.
   int undo: Flag de modo persistente pese a finalización
   abrupta.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Down_Semaforo(int semid, int num_sem, int undo);
/***************************************************************
   Nombre: DownMultiple_Semaforo
   Descripcion: Baja todos los semaforos del array indicado
   por active.
   Entrada:
   int semid: Identificador del semaforo.
   int size: Numero de semaforos del array.
   int undo: Flag de modo persistente pese a finalización
   abrupta.
   int *active: Semaforos involucrados.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int DownMultiple_Semaforo(int semid, int size, int undo, int *active);
/**************************************************************
   Nombre:Up_Semaforo
   Descripcion: Sube el semaforo indicado
   Entrada:
   int semid: Identificador del semaforo.
   int num_sem: Semaforo dentro del array.
   int undo: Flag de modo persistente pese a finalizacion
   abupta.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
 ***************************************************************/
int Up_Semaforo(int semid, int num_sem, int undo);
/***************************************************************
   Nombre: UpMultiple_Semaforo
   Descripcion: Sube todos los semaforos del array indicado
   por active.
   Entrada:
   int semid: Identificador del semaforo.
   int size: Numero de semaforos del array.
   int undo: Flag de modo persistente pese a finalización
   abrupta.
   int *active: Semaforos involucrados.
   Salida:
   int: OK si todo fue correcto, ERROR en caso de error.
***************************************************************/
int UpMultiple_Semaforo(int semid, int size, int undo, int *active);
#endif
