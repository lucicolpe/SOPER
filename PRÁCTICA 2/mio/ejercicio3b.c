/**
 * @brief calculará el tiempo que tarda en crear 100
 *        hilos y cada hilo realizará el cálculo de los N primeros primos.
 * @author Lucia Colmenarejo Perez y Jesus D. Franco Lopez
 *        lucia.colmenarejo@estudiante.uam.es  y jesus.franco@estudiante.uam.es
 * Grupo 2201 Pareja 5
 * @version 1.0
 * @date 15-03-2017
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

/**
@brief Definicion de la macro NUM_PROC
*/

#define NUM_HILOS 100

/**
* @brief funcion que lista los n primeros primos
* @param arg numero de primos a calcular
* @return int valor de exito o fracaso
*/

void* primo(void* arg){
      int* array;
      int i, nprimos = 0, m = 2, n = *((int*)arg);

      array = (int*) malloc (sizeof(int)*n);
      if(!array){
          return NULL;
      }
      while(nprimos <= n){
          if(m == 2){
              array[nprimos] = m;
              m++;
              nprimos++;
              continue;
          }
          if(m%2 == 0){
              m++;
              continue;
          }
          for(i=3;i*i<=m;i+=2){ /*Lo que hacemos es no mirar los que son pares*/
              if(m%i != 0)
                  continue;
              break;
          }
          if(i*i>m){
              array[nprimos] = m;
              nprimos++;
          }
          m++;
      }
    pthread_exit(0);
}

/**
* @brief funcion de hilos, en la que cada hilo debe
*         calcular los N primeros números primos.
* @param argc contiene el número de parámetros totales pasados
* @param argv[] contiene los parámetros pasados por el usuario
* @@return int EXIT_SUCCESS en caso de exito y EXIT_FAILURE en caso de error
*/
int main (int argc, char *argv[]){
    pthread_t hilo;
    int i, numero, ret;
    struct timeval ti, tf;/*Siendo timeval un registro con 2 campos: int tv_sec, int tv_usec, que indican los segundos y microsegundos*/
    double tiempo;

    if(argc != 2){
        printf("Error en la introducción de los parámtros de entrada\n");
        return EXIT_FAILURE;
    }

    numero = atoi(argv[1]);

    if(numero <= 0){
        printf("No valido");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&ti, NULL);/*Instante inicial*/

    /*!<creacion de hilos*/
    for(i = 0; i < NUM_HILOS; i++){
        ret = pthread_create(&hilo, NULL, primo, (void*) &numero);
        if(ret){
            printf("Error al crear el hilo %d\n", i+1);
            exit(EXIT_FAILURE);
        }
        pthread_join(hilo,NULL);
        pthread_cancel(hilo);
    }

    gettimeofday(&tf, NULL);/*Instante final*/
    /*!<realizamos el calculo del tiempo, primero los segundos y despues los microsegundos*/
    tiempo = (tf.tv_sec - ti.tv_sec) + (tf.tv_usec - ti.tv_usec)/1000000.0;
    printf("Has tardado: %g segundos\n", tiempo);

    exit(EXIT_SUCCESS);
}
