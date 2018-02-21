/**
*@brief Programa en el cual podemos ver que el proceso padre puede acceder a la memoria
*       del proceso hijo y viceversa mediante el empleo de tuberias. Hemos utilizado 8 tuberías
*       para poder hacerlo bidireccional, es decir, 4 tuberías para cada operación de de padre a hijo
*       y otras 4 para pasar de hijo a padre. Así de esta forma realizaremos las cuatro operaciones en el hijo
*       y que serán leídas e impresas en el padre.
*@author Lucia Colmenarejo Perez y Jesus D. Franco Lopez
*        lucia.colmenarejo@estudiante.uam.es  y jesus.franco@estudiante.uam.es
*	 Grupo 2201 Pareja 5
*@version 1.0
*@date 28-02-2017
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h> 

/**
@brief Definicion de la macro MAX para las cadenas de caracteres donde vamos a guardar los mensajes que se van
*      a intercambiar entre el padre y el hijo
*/

#define MAX 2000

/**
@brief Definicion de la macro NUM_PROC para la creación de 4 hijos 
*/

#define NUM_PROC 4


/**
*@brief Función principal del fichero
* 
*@return int EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error
*/

int main ( ){
    int tube_PadreaHjio[4][2] , tube_HijoaPadre[4][2] ; /*!<Declaración de las ocho tuberías*/
    int pid ;
    char message [MAX] ;
    int op1, op2, result, i;
    char readbuffer[MAX];
    char escribir[MAX];
    
    /*!<Petición de los operadores al usuario por consola*/

    printf("Por favor introduzca un entero: ");
    scanf("%d", &op1);
    printf("\nPor favor introduzca un segundo entero: ");
    scanf("%d", &op2);

   
    for (i=0; i < NUM_PROC; i++){
        
        if(pipe(tube_PadreaHjio[i]) == -1|| pipe(tube_HijoaPadre[i]) == -1){ /*!<Control de errores en la creacion de las tuberias*/
            perror ( "Error en la creacion del pipe " ) ;
            exit (EXIT_FAILURE) ;
        }

        pid = fork(); 
        if (pid == -1){
            perror ("Error en el fork");
            exit(EXIT_FAILURE);

        }else if (pid == 0) {
            /*!<Cerramos el descriptor de escritura de la tuberia de padre a hijo*/
            /*!<Cerramos el descriptor de lectura de la tuberia de hijo a padre*/
            /*!<Hacemos ambos cierres en este bucle y así hacemos la comprobación de errores también de cada uno de ellos*/

            if(close (tube_PadreaHjio[i][1]) == -1 || close (tube_HijoaPadre[i][0]) == -1){
                exit(EXIT_FAILURE);
            }
           
            read(tube_PadreaHjio[i][0] , readbuffer , MAX-1); /*!<Leemos en el hijo los datos pasado por el padre mediante la tuberia*/
            sscanf(readbuffer, "%d,%d", &op1, &op2);
            
               
            switch(i){ /*!<Realizacion de cad una de las operaciones en funcion del hijo en el que estemos*/
                    case 0:
                        result = op1 + op2;
                        sprintf(message, "Operando 1: %d Operando 2: %d Suma: %d Datos enviados por el proceso PID= %d \n", op1, op2, result, getpid());
                        break;
                       case 1: 
                           result = op1 - op2;
                           sprintf(message, "Operando 1: %d Operando 2: %d Resta: %d Datos enviados por el proceso PID= %d \n", op1, op2, result, getpid());
                        break;
                    case 2:
                        result = op1 * op2;
                           sprintf(message, "Operando 1: %d Operando 2: %d Producto: %d Datos enviados por el proceso PID= %d \n", op1, op2, result, getpid());
                        break;
                       case 3: 
                           result = op1 / op2;
                        sprintf(message, "Operando 1: %d Operando 2: %d Division: %d Dtos enviados por el proceso PID= %d \n", op1, op2, result, getpid());
                        break;
                    default:
                        printf("Has cometido algún error, no deberías de haber llegado a este error");
             }
                
             write(tube_HijoaPadre[i][1] ,message ,strlen(message)+1); /*!<Escribimos en el padre el resultado obtenido de cada operacion en el switch*/
             exit(EXIT_SUCCESS) ;

        } else {
            /*!<Cerramos el descriptor de lectura de la tuberia de padre a hijo*/
            /*!<Cerramos el descriptor de escritura de la tuberia de hijo a padre*/
            /*!<Hacemos ambos cierres en este bucle y así hacemos la comprobación de errores también de cada uno de ellos*/

            if(close (tube_PadreaHjio[i][0]) == -1 || close (tube_HijoaPadre[i][1]) == -1){
                exit(EXIT_FAILURE);
            }

            sprintf(escribir, "%d,%d", op1, op2);
            write(tube_PadreaHjio[i][1], escribir, strlen(escribir)+1); /*!<Escribimos en el hijo los parametros obtenidos por consola mediante la tuberia de padre a hijo que está abierto en modo escritura*/
            wait(NULL);
            read(tube_HijoaPadre[i][0] , message , MAX-1) ; /*!<Leemos el resultado que obtuvimos previamente del switch en el proceso hijo*/
            printf("%s", message);
            
            
        } 
    }
    wait(NULL); /*!<Escribimos un wait para que el padre espere a todos sus hijos*/
    exit(EXIT_SUCCESS);
}

