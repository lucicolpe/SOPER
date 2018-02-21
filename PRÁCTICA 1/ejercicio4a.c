/**
*@brief Programa en el cual creamos una serie de procesos hijos,
*       pero el padre no espera a ninguno de sus hijos, por lo que 
*       la saida será simepre aleatoria.
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

/**
@brief Definicion de la macro NUM_PROC 
*/
#define NUM_PROC 3

/**
*@brief Función principal del fichero
* 
*@return int EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error
*/

int main (void){
    int pid;
    int i;
    for (i=0; i < NUM_PROC; i++){
        if ((pid=fork()) <0 ){
            printf("Error al emplear fork\n");
            exit(EXIT_FAILURE);
        }else if (pid ==0){
            printf("HIJO %d y su Padre es %d\n", getpid(), getppid()); /*!<Imprimimos el hijo y su padre con sus correspondientes pid's*/
        }else{
            printf ("PADRE %d \n", getpid());
            system("pstree getppid(i)");
        }
    }
    
    exit(EXIT_SUCCESS);/*!<El padre no espera a la ejecución de ninguno de sus hijos*/
}

