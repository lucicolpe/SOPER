/**
 * @brief Implementa el ejercicio 2 de memoria compartida
 * @file ejercicio2.c
 * @author Lucia Colmenarejo Perez lucia.colmenarejo@estudiante.uam.es
 * @author Jesus Daniel Franco Lopez jesusdaniel.francolopez@estudiante.uam.es
 * @note Grupo 2201 
 * @version 1.0
 * @date 06/04/2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <signal.h>
#include <unistd.h>

/**
* @brief Definicion de la clave de fichero
*/

#define FILEKEY "/bin/cat"  

/**
* @brief Definicion de la clave
*/

#define KEY 1300

/**
 * @brief Estructura info que contiene los parametros pedidos para la
 * reserva de un bloque de memoria
 */

typedef struct info{
  char nombre[80];
  int id;
}Info;

/**
* Inicializacion de la estructura a NULL
*/

Info* datos = NULL;

/**
* @brief funcion de captura de señales
* @param signal señal pasada
*/

void manejador(int signal){
  printf("Nombre del usuario %s\n", (*datos).nombre);
  printf("Id del usuario %d\n", (*datos).id);
}

/**
* @brief funcion principal que genera nhijos y que implementa
*   todo lo que debe realizar tanto el padre como sus hijos
* @param argc: contiene el número de parámetros totales pasados
* @param argv: contiene los parámetros pasados por el usuario
* @return int: valor de exito (OK) o fracaso (ERROR)
*/

int main(int argc, char const *argv[]) {
  int i;
  int n;
  int pid;
  int id_zone;
  int key;

  if(argc != 2){
      printf("Pasamos un número entero positivo como argumento para generar los procesos hijos\n");
      exit(EXIT_FAILURE);
  }

  if((n = atoi(argv[1])) <= 0){
      printf("El argumento de entrada debe ser un entero positivo\n");
      exit(EXIT_FAILURE);
  }

  /*Para poder usar memoria compartida en Linux, es necesario in Identificador de IPC, para convertir la ruta del sistema en un Identificador IPC.
  Este identificador será necesario para la creación de la zon de memoria virtual(lo hacemos mediante la llamada a ftok).*/

  key = ftok(FILEKEY, KEY);

  /*Comprobamos que nos devuelve un número válido*/
  if(key == -1){
     printf("Error al obtener key\n");
     exit(EXIT_FAILURE);
  }
  /*Creamos un segmento de memoria compartida, le pasamos IPC_EXCL para que salvaguarde la escritura, y le pasamos también SHM_W y SHM_R para que
  dé permiso a la memoria compatida de escritura y lectura, respectivamente.*/
  id_zone = shmget(key, sizeof(Info), IPC_CREAT | IPC_EXCL | SHM_W |SHM_R);

  /*Comprobación de que se ha creado correctamente la zona de memoria compartida*/
  if(id_zone == -1){
    id_zone = shmget(key, sizeof(Info), IPC_CREAT | SHM_R | SHM_W);
    if(id_zone == -1){
        fprintf(stderr, "Error en la creación de la zona de memoria compartida\n");
        free(datos); /*Liberamos por si las moscas*/
        exit (EXIT_FAILURE);
    }
  }
  /*El núcleo verifica que el proceso tiene los permisos necesarios para acceder a la región, y devuelve la dirección donde nos conectamos*/
  datos = shmat(id_zone, (char*)0, SHM_R | SHM_W); /*(char*)0 es la dirección virtual donde el usuario quiere unir la memoria compartida*/

  /*Comprobación de errores de que no se ha unido correctamente la memoria*/
  if(datos == NULL){
    fprintf(stderr, "Error a la hora de unir la memoria\n");
    free(datos);
    exit(EXIT_FAILURE);
  }

  if(signal(SIGUSR1, manejador) == SIG_ERR){
    printf("Error al capturar la señal SIGUSR1");
    free(datos);
    exit(EXIT_FAILURE);
  }
  for (i=0; i <n; i++){
      /*Caso en el que si hacemos un fork se hace incorrectamente*/
      if((pid = fork()) == -1){
          printf("Error a la hora de hacer el fork\n");
          free(datos);
          exit(EXIT_FAILURE);
	    }
      /*Cuando se encuentre en el caso del padre*/
      if(pid > 0){
          pause();
      }
    /*Caso en el que estamos en el proceso hijo*/
    	if(pid == 0){
      	  usleep(getpid()*1000); /*Dormimos un tiempo aleatorio*/
          /*Creamos un segmento de memoria compartida, le pasamos IPC_EXCL para que salvaguarde la escritura, y le pasamos también SHM_W y SHM_R para que
          dé permiso a la memoria compatida de escritura y lectura, respectivamente.*/
          id_zone = shmget(key, sizeof(Info), 0);

          if(id_zone == -1){
            fprintf(stderr, "Error en la creación de la zona de memoria compartida\n");
            free(datos); /*Liberamos por si las moscas*/
            exit (EXIT_FAILURE);
          }

          /*Comprobamos de nuevo que se une a la memoria correctamente*/
          datos = shmat(id_zone, (char*)0, 0);
          if(datos == NULL){
             printf("Error al unir la memoria\n");
             exit(EXIT_FAILURE);
          }

          /*Pedimos por consola que se introduzca el nombre del ususario*/
          printf("Introduzca el nombre de usuario:\n");
          scanf("%s", (*datos).nombre);


        	(*datos).id++; /*Si no lo incrementará en una unidad*/

          /*Para separar el espacio de memoria de su direccionamiento. Le pasamos el valor que devuelve la llamada a shmat*/
          shmdt((char*) datos);

          kill(getppid(), SIGUSR1);
          exit(EXIT_SUCCESS);

        }

  }
  /*Permite a un proceso buscar el estado y establecer los parámetros de la región de memoria compartida.
  IPC_RMID lo que hacía era que eliminaba del sistema el identificador de memoria compartida*/
  shmctl(id_zone, IPC_RMID, (struct shmid_ds*) NULL);
  exit(EXIT_SUCCESS);
}
