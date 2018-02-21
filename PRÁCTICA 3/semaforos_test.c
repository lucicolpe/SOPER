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
#include <sys/sem.h>
#include <errno.h>
#include "semaforos.h"

#define NOT_PASSED -1
#define PASSED 1

/**
*
*/
int Crear_Semaforo_Test(){
	union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;

  	int clave = 12345;
  	int sem1, sem2;
  	int tamanio = 4;
  	int i;

  	arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);

    i = Crear_Semaforo(clave, tamanio, &sem1);
    if (i == ERROR) {
        //No ha ningún caso creado de antes
        perror("Crear_Semaforo_Test falla en la creación de un semáforo sin haber ninguno antes.\n");
        Borrar_Semaforo(sem1);
        return NOT_PASSED;
	}

	//Caso en el que nos debería de devolver error dado que ya se ha creado un semaforo con esa clave
	i = Crear_Semaforo(clave, tamanio, &sem2);
	if (i == 1 || i == ERROR){
		/*Crear_Semaforo_Test falla en la creación de un semáforo habiendo creado uno antes*/
        Borrar_Semaforo(sem1);
        free(arg.array);
        return PASSED;
	}
	if(sem1 != sem2){
		Borrar_Semaforo(sem1);
        return NOT_PASSED;
	}

	//Comprobamos que se le pasa el tamaño correcto
	if (Crear_Semaforo(clave, tamanio + 1, &sem2) == 1 || sem2 != ERROR) {
        perror("Crear_Semaforo_Test falla porque le pasamos un tamaño incorrecto.\n");
        Borrar_Semaforo(sem1);
        return NOT_PASSED;
	}


	Borrar_Semaforo(sem1);
	free(arg.array);
	return PASSED;

}


int Inicializar_Semaforo_Test(){
	int sem;
	int clave = 12345;
	int tamanio = 4;
	int i;
	unsigned short array[4] = { 0, 1, 2, 3};

	union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;

  	arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);


  	if(Crear_Semaforo(clave,tamanio,&sem) == ERROR){
  	 	return NOT_PASSED;
  	}

  	if (Inicializar_Semaforo(sem, array) == ERROR) {
        Borrar_Semaforo(sem);
        return NOT_PASSED;
	}
	
	semctl(sem, tamanio, GETALL, arg);
    for (i = 0; i < tamanio; i++) {
        if (arg.array[i] != i) {
            Borrar_Semaforo(sem);
            return NOT_PASSED;
        }
    }

    if (Inicializar_Semaforo(sem, NULL) != ERROR) {
        Borrar_Semaforo(sem);
        return NOT_PASSED;
    }

    Borrar_Semaforo(sem);
    free(arg.array);
	return PASSED;
}


int Down_Semaforo_Test(){
    int sem;
    int clave = 12345;
    int tamanio   = 4;
    int i, aux;
    unsigned short array[4] = { 1, 2, 3, 4 };
    union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);

    Crear_Semaforo(clave, tamanio, &sem);
    Inicializar_Semaforo(sem, array);
    for (i = 0; i < tamanio; i++) {
        Down_Semaforo(sem, i, SEM_UNDO);
        aux = i;
        if(semctl(sem, i, GETVAL, 0) != aux) {
            Borrar_Semaforo(sem);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(sem);
    free(arg.array);
    return PASSED;
}


int Up_Semaforo_Test(){
    int sem;
    int clave = 12345;
    int tamanio  = 4;
    int i, aux;
    unsigned short array[4] = { 1, 2, 3, 4 };
    union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;
    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);

    Crear_Semaforo(clave, tamanio, &sem);
    Inicializar_Semaforo(sem, array);
    for (i = 0; i < tamanio; i++) {
        Up_Semaforo(sem, i, SEM_UNDO);
        aux = i+2;
        if(semctl(sem, i, GETVAL, 0) != aux) {
            Borrar_Semaforo(sem);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(sem);
    free(arg.array);
    return PASSED;
}

int DownMultiple_Semaforo_Test(){
    int sem;
    int clave = 12345;
    int tamanio   = 4;
    int i, aux, aux2;
 	unsigned short array[4] = { 1, 2, 3, 4 };
	int active[3] = { 0, 2, 3 };
	
	union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;

    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);

    
    Crear_Semaforo(clave, tamanio, &sem);
    Inicializar_Semaforo(sem, array);

    DownMultiple_Semaforo(sem, 3, SEM_UNDO, active);
    aux = -1;
	

    for (i = 0; i < tamanio; i++) {
        aux2 = 0;
        if (i != 1) {
            aux2 = aux;
        }
        if (semctl(sem, i, GETVAL, 0) != array[i] + aux2) {
            Borrar_Semaforo(sem);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(sem);
    free(arg.array);
    return PASSED;
}



int UpMultiple_Semaforo_Test(){
    int sem;
    int clave = 12345;
    int tamanio   = 4;
    int i, aux, aux2;
 	unsigned short array[4] = { 1, 2, 3, 4 };
	int active[3] = { 0, 2, 3 };
	
	union semun {
    	int val;
    	struct semi_ds *buf;
    	unsigned short *array;
  	} arg;

    arg.array = (unsigned short *) malloc(sizeof(unsigned short) * tamanio);

    
    Crear_Semaforo(clave, tamanio, &sem);
    Inicializar_Semaforo(sem, array);

    UpMultiple_Semaforo(sem, 3, SEM_UNDO, active);
    aux = 1;
	

    for (i = 0; i < tamanio; i++) {
        aux2 = 0;
        if (i != 1) {
            aux2 = aux;
        }
        if (semctl(sem, i, GETVAL, 0) != array[i] + aux2) {
            Borrar_Semaforo(sem);
            return NOT_PASSED;
        }
    }
    Borrar_Semaforo(sem);
    free(arg.array);
    return PASSED;
}

void main(){

    if (Crear_Semaforo_Test() == NOT_PASSED){
        perror("Error en Crear_Semaforo_Test");
    	return;
    }
    printf("Crear_Semaforo_Test correcto.\n");

    if (Inicializar_Semaforo_Test() == NOT_PASSED){
        perror("Error en Inicializar_Semaforo_Test");
	    return;
    }
    printf("Inicializar_Semaforo correcto.\n");

    if (Down_Semaforo_Test() == NOT_PASSED){
        perror("Error en Down_Semaforo_Test");
        return;
    }
    printf("Down_Semaforo correcto.\n");

    if (Up_Semaforo_Test() == NOT_PASSED){
        perror("Error en Up_Semaforo_Test");
        return;
    }
    printf("Up_Semaforo correcto.\n");

    if (DownMultiple_Semaforo_Test() == NOT_PASSED){
        perror("Error en DownMultiple_Semaforo_Test");
        return;
    }
    printf("DownMultiple_Semaforo correcto.\n");

    if (UpMultiple_Semaforo_Test() == NOT_PASSED){
        perror("Error en UpMultiple_Semaforo_Test");
        return;
    }   
    printf("UpMultiple_Semaforo correcto.\n");
	return;
}