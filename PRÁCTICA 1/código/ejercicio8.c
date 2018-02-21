/**
*@brief Programa que crea una serie de procesos hijo a partir del numero que reciba el padre como argumentos. Estos procesos
*  hijo pueden ser cualquier tipo de programa ejecutable. Para ello vamos a utilizar las funciones execl, execv, execlp y execvp.
*  Elegimos el metodo mediente el que queremos ejecutar nuestros programas pasandolo como ultimo argumento del programa.
*
*@param El programa recibe como argumentos los programas que quiere ejecutar y el metodo de ejecucion de estos mismos.
*
*@author Lucia Colmenarejo Perez y Jesus D. Franco Lopez 
*  lucia.colmenarejo@estudiante.uam.es jesus.franco@estudiante.uam.es
*  Grupo 2201 Pareja 5
*
*@version 1.0
*
*@date 28-02-2017
*
*/

#include <stdio.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char*argv[]){
  int pid;
  int numero_procesos = argc-2;    /*!<Se le pasan argc-2 porque hay que eliminar el que corresponde al nombre del programa y el de la llamada exec*/
  int i;
  for(i=1; i<=numero_procesos ; i++){    /*!<Iteración donde se crean tantos procesos hijos como número de parámetros reciba por consola*/
    if((pid = fork())<0){
      printf("Has cometido algún error haciendo el fork\n");
      exit(EXIT_FAILURE);
    }
    /*!<Mediante el uso de una condicion if comprobamos cual es el ultimo parametro de entrada, el metodo de ejecucion de los programas*/
    else if(pid ==0){
      /*!<Si los metodos son -l pasamos los argumentos a execl como char*. En este caso, el primer argumento es el char* path.*/
      if(strcmp(argv[argc-1], "-l") == 0){
        char aux[100];
        strcpy(aux, "/bin/");
        strcat(aux, argv[i]);
        printf("El hijo %d ejecuta el programa %s con la función execl\n", i, argv[i]);
        execl(aux, argv[i], (char*)NULL);
        perror("Error en execl\n");
        exit(EXIT_FAILURE);
    
      }
      /*!<Si los metodos son -v pasamos los argumentos a execv como una lista de char*. En este caso, el primer argumento es el char* path.*/
      if(strcmp(argv[argc-1], "-v") == 0){
        char *programas[]= {argv[i], NULL};
        char aux[100];
        strcpy(aux, "/bin/");
        strcat(aux, argv[i]);
        printf("El hijo %d ejecuta el programa %s con la función execv\n", i, argv[i]);
        execv(aux,programas);
        perror("Error en execv\n");
        exit(EXIT_FAILURE);
    
      }
      /*!<Si los metodos son -l pasamos los argumentos a execlp como char*. En este caso, el primer argumento es el char* file.*/
       if(strcmp(argv[argc-1], "-lp") == 0){
        printf("El hijo %d ejecuta el programa %s con la función execlp\n", i, argv[i]);
        execlp(argv[i], argv[i], (char*)NULL);
        perror("Error en execlp\n");
        exit(EXIT_FAILURE);
    
      }
      /*!<Si los metodos son -vp pasamos los argumentos a execvp como una lista de char*. En este caso, el primer argumento es el char* file.*/
      if(strcmp(argv[argc-1], "-vp") == 0){
        char *programas[]= {argv[i], NULL};
        printf("El hijo %d ejecuta el programa %s con la función execvp\n", i, argv[i]);
        execvp(argv[i],programas);
        perror("Error en execvp\n");
        exit(EXIT_FAILURE);
      }

    }
    else{     /*!<Estamos en el caso del padre y, por tanto, ponemos un wait para que espere a su hijo*/
      wait(NULL);
    }
  }
  wait(NULL);/*!<El proceso padre espera a que los proceson hijo finalicen*/
  exit(EXIT_SUCCESS);
}

