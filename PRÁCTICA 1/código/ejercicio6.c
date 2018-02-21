/**
*@brief Programa en el cual podemos ver que el proceso padre al hacer un fork
*       no comparte memoria con el hijo, para poder acceder a la memoria del hijo o el 
*       padre, vamos a tener que uitlizar tuberías(veremos en el ejercicio9).
*       Lo comprobamos al ver que al que podemos guardar distintas cosas en una misma variable, 
*       ya que cada proceso tiene su memoria.
*@author Lucia Colmenarejo Perez y Jesus D. Franco Lopez
*        lucia.colmenarejo@estudiante.uam.es  y jesus.franco@estudiante.uam.es
*	 Grupo 2201 Pareja 5
*@version 1.0
*@date 28-02-2017
*/
#include <sys/types.h> 
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
*@brief Función principal del fichero
* 
*@return int EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error
*/


int main (){
    int pid;
    char* cadena;
    cadena = (char*)malloc(80 * sizeof (char));

    if ((pid=fork()) <0 ){
        printf("Error haciendo fork\n");
        free(cadena);
        exit(EXIT_FAILURE);
    }else if (pid ==0){
        printf("Introduce un nombre que se va a guardar en la variable cadena del hijo: \n");/*!<Pedimos por consola que el usuario meta una cadena para guardarla en el hijo*/
        scanf("%s",cadena);
        

    }else{
        wait(NULL);
        printf("Introduce un nombre que se va a guardar en la variable cadena del padre: \n");/*!<Pedimos por consola que el usuario meta una cadena para guardarla en el padre*/
        scanf("%s",cadena);
        exit(EXIT_SUCCESS);
    
    }
    wait(NULL);
    printf("cadena: %s\n",cadena); /*!<Imprimimos la cadena para ver que no comparten memoria*/
    free(cadena);/*!<Liberamos la memoria de la cadena*/
    exit(EXIT_SUCCESS);
}

