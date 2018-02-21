/**
 * @brief calculará el tiempo que se invierte en la creación de 100 procesos hijos,
 *        la ejecución de cada proceso hijo y la finalización correcta del programa
 * @author Lucia Colmenarejo Perez y Jesus D. Franco Lopez
 *        lucia.colmenarejo@estudiante.uam.es  y jesus.franco@estudiante.uam.es
 * Grupo 2201 Pareja 5
 * @version 1.0
 * @date 15-03-2017
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

/**
@brief Definicion de la macro NUM_PROC
*/

#define NUM_PROC 100

/**
* @brief funcion que calcula los n primeros primos
* @param nprim numero de primos a calcular
* @return array que contiene los n primeros primos
*/
int* primo(int nprim){
    int* array;
    int i, nprimos = 0, n = 2;

    array = (int*) malloc (sizeof(int)*nprim);
    if(!array){
        return NULL;
    }
    while(nprimos <= nprim){
      if(n == 2){
          array[nprimos] = n;
          n++;
          nprimos++;
          continue;
      }

      if(n%2 == 0){
          n++;
          continue;
      }

      for(i=3;i*i<=n;i+=2){ /*Lo que hacemos es no mirar los que son pares*/
          if(n%i != 0)
              continue;
          break;
      }
      if(i*i>n){
          array[nprimos] = n;
          nprimos++;
      }
      n++;
    }
    return array;
}

/**
* @brief funcion de procesos con un solo padre.Cada proceso hijo
*       debe calcular los N primeros números primos.
* @param argc contiene el número de parámetros totales pasados
* @param argv[] contiene los parámetros pasados por el usuario
* @@return int EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error
*/
int main (int argc, char *argv[]){
    int fpid = 1;
    int i, n;
    struct timeval ti, tf; /*Siendo timeval un registro con 2 campos: int tv_sec, int tv_usec, que indican los segundos y microsegundos*/
    double tiempo;


    if(argc != 2){
        printf("Se debe pasar un solo parámetro\n");
        return EXIT_FAILURE;
    }

    /*Comprobamos error de no pasar un numero*/
    n = atoi(argv[1]);
    if(n <= 0){
        printf("No valido");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&ti, NULL);/*Instante inicial*/

    /*!<Creamos cada uno de los procesos*/
    for(i = 0; i < NUM_PROC; i++){
        if (fpid != 0){
            if ((fpid=fork()) == -1){
                printf("Error al emplear fork\n");
                exit(EXIT_FAILURE);
            }
            if(fpid == 0){
                int* array;
                array = primo(n);
                if(array == NULL){
                    printf("Error al reservar memoria\n");
                    exit(EXIT_FAILURE);
                }
                free(array);
                exit(EXIT_SUCCESS);
            }
        }
        wait(NULL);
    }

    gettimeofday(&tf, NULL);/*Instante final*/
    /*!<realizamos el calculo del tiempo, primero los segundos y despues los microsegundos*/
    tiempo = (tf.tv_sec - ti.tv_sec) + (tf.tv_usec - ti.tv_usec)/1000000.0;
    printf("Tiempo de ejecucion: %gs\n", tiempo);

    exit(EXIT_SUCCESS);
}
