/**
 * @brief Implementación del ejercicio 8 en el que se refuerza el procedimiento que se
 *        ha de seguir para enviar una señal de un proceso hijo a uno padre y viceversa.
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
#include <unistd.h>
#include <time.h>

/**
* @brief funcion del manejador muy simple debido a la decisión de no hacerlo
*        con variables globales.
* @param sig señal pasada
*/

void manejador(int sig){
    return;
}

/**
* @brief funcion en la que el proceso padre genere N procesos hijo en serie y tendremos
*        un paso de testigo circular que se va a repetir V veces. N (número de procesos) y V(número
*        de veces de paso del testigo) serán argumentos primero y segundo respectivamente del programa a ejecutar.
*        Cada proceso padre espera a recibir SIGUSR1. Una vez recibida la señal: imprime un mensaje con el pid y la hora,
*        espera 2 segundos y envía SIGUSR1 a su hijo. El último proceso hará lo mismo pero pasará la señal al proceso raíz.
*        Una vez el proceso raíz detecte que el testigo ha dado V vueltas, pasará la señal SIGTERM a su hijo.
*        El protocolo de terminación involucra que cada proceso realice las siguientes órdenes:cada proceso espera la recepción
*        de SIGTERM, espera un segundo y envía SIGTERM a su hijo, imprime un mensaje y termina llamando a exit.
* @param argc contiene el número de parámetros totales pasados
* @param argv[] contiene los parámetros pasados por el usuario
* @return int: valor de exito o fracaso
*/
int main (int argc, char *argv[]){
    int pid = 0;
    int i, j;
    int N;
    int V;
    int* cadena;
    time_t tiempo;
    struct tm *tl;
    char output[128];

    /*Comprobación de que se pasan el número correcto de argumentos*/
    if(argc != 3){
        printf("Se debe pasar dos parámetros\n");
        return EXIT_FAILURE;
    }
    /*Comprobación de la correcta reserva de memoria para el cadena*/
    cadena = (int*)malloc(sizeof(int)*(N+1));
    if(cadena == NULL){
        printf("Error reservando memoria.\n");
        exit(EXIT_FAILURE);
    }
    /*Comprobación de que el primer parámetro pasado por consola es un número entero positivo*/
    N = atoi(argv[1]);
    if(N <= 0){
        printf("El primer parámetro ha de ser un entero positivo.\n");
        exit(EXIT_FAILURE);
    }
    /*Comprobación de que el segundo número pasado por consola es un número entero positivo*/
    V = atoi(argv[2]);
    if(V <= 0){
        printf("El segundo parámetro ha de ser un entero positivo.\n");
        exit(EXIT_FAILURE);
    }

    /*Le asignamos a cada uno de los elementos del cadena el valor de 0, ya que luego en ellos guardaremos los id de los procesos*/
    for(i = 0; i < N+1; i++){
        cadena[i] = 0;
    }
    /*El padre crea N procesos hijos en serie*/
    cadena[0] = getpid();
    for(i = 0; i < N; i++){
        if(pid == 0){
            if ((pid = fork()) < 0){
                printf("Error al emplear fork\n");
                exit(EXIT_FAILURE);
            }
            /*Caso en el que nos encontramos en el padre*/
            if(pid > 0){
                cadena[i] = getpid();
                cadena[i+1] = pid;
                /*Armar la señal, la cual llama a nuestro manejador implementado fuera de la funcion*/
                if(signal(SIGUSR1, manejador) == SIG_ERR){
                    printf("Error en la señal SIGUSR1\n");
                    exit(EXIT_FAILURE);
                }
                pause();/* Bloquea al proceso hasta que llegue una señal*/

            }
            else{/*Estamos en el caso del hijo*/
                cadena[i+1] = getpid(); /*Guardamos en el cadena el pid del proceso*/
            }
        }
    }
    if(getpid() == cadena[N]){
        fflush(stdout); /*Nos fuerza la salida*/
        sleep (5);/*Hacemos que el último hijo espere 5 segundo y mande la señal al proceso padre*/
        kill(cadena[0], SIGUSR1); /*kill nos ayudaba a mandar señales entre procesos si estos tienen el mismo pid*/
        if(signal(SIGUSR1, manejador) == SIG_ERR){
            printf("Error en la señal SIGUSR1\n");
            exit(EXIT_FAILURE);
        }
        pause();
    }
    /*bucle para el número de veces de paso del testigo*/
    for(j = 0; j < V; j++){
        for (i = 0; i < N; i++){
            if(getpid() == cadena[i]){/*Hace que cada uno de los procesos padres imprima un mensaje, espere 2 segundo y mande la señal SIGUSR1 a su hijo*/
                tiempo = time(0);
                tl = localtime(&tiempo);
                strftime(output, 128, "%d/%m/%y %H:%M:%S", tl);
                printf("Hola PID=%d,time=%s\n", getpid(), output);
                fflush(stdout);
                sleep(2);/*Espera 2 segundos*/
                kill(cadena[i+1],SIGUSR1); /*Envía SIGUSR1 a el hijo siguiente*/
                break;
            }
        }
        /*El último de los hijos va a esperar 5 segundos y envía SIGUSR1 al proceso raíz*/
        if(getpid() == cadena[N]){
            tiempo = time(0);
            tl = localtime(&tiempo);
            strftime(output, 128, "%d/%m/%y %H:%M:%S", tl);
            printf("Hola PID=%d,time=%s\n", getpid(), output);
            fflush(stdout);
            sleep(2);
            kill(cadena[0],SIGUSR1);
        }
        /*Control de errores de la señal que cada proceso padre espera recibir*/
        if(signal(SIGUSR1, manejador) == SIG_ERR){
            printf("Error en la señal SIGUSR1\n");
            exit(EXIT_FAILURE);
        }
        /*Control de errores de la señal que cada proceso hijo espera recibir una vez que el proceso padre detecte que el testigo ha dado V vueltas*/
        if(signal(SIGTERM, manejador) == SIG_ERR){
            printf("Error en la señal SIGUSR1\n");
            exit(EXIT_FAILURE);
        }
        pause();/* Bloquea al proceso hasta que llegue una señal*/
    }

    /*Primero muere el primer proceso hijo*/
    if(getpid() == cadena[0]){
      sleep(1);
        kill(cadena[1],SIGTERM);
        if(signal(SIGTERM, manejador) == SIG_ERR){
            printf("Error en la señal SIGUSR1\n");
            exit(EXIT_FAILURE);
        }
        pause();
        printf("Muere %d\n", cadena[0]);
        free(cadena);
        exit(EXIT_SUCCESS);
    }
    /*Después mueren el resto de hijos*/
    for(i = 1; i < N; i++){
        if(getpid() == cadena[i]){
            sleep(1);
            kill(cadena[(i+1)],SIGTERM);
            if(signal(SIGTERM, manejador) == SIG_ERR){
                printf("Error en la señal SIGUSR1\n");
                exit(EXIT_FAILURE);
            }
            printf("Muere %d\n", cadena[i]);
            free(cadena);
            exit(EXIT_SUCCESS);
        }
    }
    /*Por último muere el proceso raíz*/
    if(getpid() == cadena[N]){
        sleep(1);
        kill(cadena[0],SIGTERM);
        if(signal(SIGTERM, manejador) == SIG_ERR){
            printf("Error en la señal SIGUSR1\n");
            exit(EXIT_FAILURE);
        }
        printf("Muere %d\n", cadena[N]);
        free(cadena);
        exit(EXIT_SUCCESS);
    }

    exit(EXIT_SUCCESS);
}
