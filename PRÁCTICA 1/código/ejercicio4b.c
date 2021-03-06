/**
*@brief Programa que crea una serie de procesos hijo. En este ejercicio, a diferencia de en el apartado a,
*    el padre espera a que finalice la ejecución de cada uno de los procesos hijo. La salida, por tanto
*    la salida no es aleatoria, sino que salen los procesos de manera ordenada.
*
*@param
*
*@author Lucia Colmenarejo Perez y Jesus D. Franco Lopez 
*    lucia.colmenarejo@estudiante.uam.es jesus.franco@estudiante.uam.es
*    Grupo 2201 Pareja 5
*
*@version 1.0
*
*@date 28-02-2017
*
*/

#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

/**
*@brief Definicion de la macro NUM_PROC
*/

#define NUM_PROC 3

/**
*@brief Funcion principal del fichero
*
*@return int EXIT_SECCES en caso de exito y EXIT_FAILURE en caso contrario
*/

int main (void){
    int pid;
    int i;
    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error haciendo fork\n");
            exit(EXIT_FAILURE);
        }else if (pid ==0){
            printf("HIJO %d y su PADRE es %d\n", getpid(), getppid());/*!<Imprimimos los procesos padre e hijo con sus correspondientes PID's*/
            /*system("pstree -ph");*/
        }else{
            printf ("PADRE %d\n", getppid());
        }
    }
    wait(NULL);/*!<El proceso padre espera a que los proceson hijo finalicen*/
    exit(EXIT_SUCCESS);
    
}

