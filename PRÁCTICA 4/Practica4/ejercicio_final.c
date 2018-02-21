/**
 * @brief Código del ejercicio final
 * 
 * Este fichero contiene el código del ejercicio final.
 * @file ejercicio_final.c
 * @author Miguel García Moya miguel.garciamoya@estudiante.uam.es Grupo 2201
 * @date 10-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include "semaforos.h"

#define FILEKEY "/bin/cat"    /*!< FileKey para ftok*/
#define KEY 1300              /*!< Key para ftok*/
#define MAX_NOMBRE 20         /*!< Tamaño máximo de nombre */
#define MAX_CABALLOS 10       /*!< Máximo de caballos */
#define MAX_APUESTAS 200      /*!< Máximo de apuestas que se pueden gestionar */
#define NUM_CABALLOS_PODIO 3  /*!< Número de caballos mostrados como ganadores */
#define MAX_BUFFER 12         /*!< Tamaño máximo de buffer */
#define MUTEX 0               /*!< Índice del semáforo mutex */
#define MONITOR 1             /*!< Índice del semáforo del monitor */
#define NO_MAS_APUESTAS 2     /*!< Índice del semáforo que bloquea el gestor de apuestas */
#define NUM_SEMAFOROS 3       /*!< Número de semáforos */

/**
 * @brief Información de apuesta
 *
 * Esta estructura guarda la información de las apuestas hechas.
 */
typedef struct {    
    char nombre[MAX_NOMBRE]; /*!<Nombre del apostador*/
    int num_caballo;         /*!<Caballo apostado*/
    double dinero;           /*!<Cantidad a pagar en caso de victoria*/
} Apuesta; 

/**
 * @brief Datos de la memoria compartida
 *
 * Esta estructura guarda la información de la carrera.
 */
typedef struct {
    int segundos;                       /*!<Segundos hasta que comienze*/
    int posiciones[MAX_CABALLOS];       /*!<Posiciones de los caballos*/
    int num_caballos;                   /*!<Nº de caballos*/
    double cotizaciones[MAX_CABALLOS];  /*!<Cotizaciones de los caballos*/
    double dinero[MAX_CABALLOS];        /*!<Dinero apostado a cada caballo*/
    double dinero_total;                /*!<Dinero apostado a todos los caballos*/
    int carrera_comenzada;              /*!<1 si la carrera ha comenzado*/
    int carrera_terminada;              /*!<1 si la carrera ha terminado*/
    Apuesta apuestas[MAX_APUESTAS];     /*!<Apuestas hechas*/
    int num_apuestas;                   /*!<Nº de apuestas*/
    int ganadores[MAX_CABALLOS];        /*!<Caballos en podio*/
    int semaforos;                      /*!<Identificador de los semáforos*/
    int id_zone;                        /*!<Id de la memoria compartida*/
} Info_carrera;

/**
 * @brief Información de mensaje
 *
 * Esta estructura guarda la información de los mensajes entre apostador y gestión de apuestas.
 */
typedef struct {  
    long id;         /*!<Identificador del mensaje*/    
    char nombre[MAX_NOMBRE]; /*!<Nombre del apostador*/
    int num_caballo; /*!<Caballo apostado*/
    double apuesta;  /*!<Cantidad apostada*/
} Mensaje; 

void comprobacion_argumentos(int argc, char* argv[], int* num_caballos, int* longitud, int* num_apostadores, int* num_ventanillas);
Info_carrera* reservar_memoria_compartida(int num_caballos);
void liberar_memoria_compartida();
void monitor();
void imprimir_carrera_no_comenzada();
void imprimir_carrera_comenzada();
void imprimir_carrera_terminada();
void gestor_apuestas(int num_ventanillas);
void* gestion_apuestas(void* arg);
void apostador(int num_apostadores);
void caballo(int* pipe);
void calcular_ganadores();
int get_posicion(int num_caballo);

Info_carrera* info; /*!<Información de la carrera*/  
int msqid;         /*!<Id de la cola de mensajes*/

/**
 * @brief Ejecuta el código del programa principal de ejercicio final.
 * 
 * Gestiona la carrera, comprobando los datos, gestionando los
 * recursos y transmitiendo información a los procesos hijos.
 * 
 * @param argc Número de argumentos del programa
 * @param argv Argumentos del programa: nº de caballos, longitud de
 * la carrera, nº de apostadores y nº de ventanillas.
 */
int main(int argc, char* argv[]) {
    void manejador_SIGINT();
    void manejador_SIGALRM();
    void manejador_SIGUSR1_principal();
    int num_caballos, num_apostadores, num_ventanillas, longitud;
    int fd[2];
    int monitorpid, gestorpid, apostadorpid;
    int caballopid[MAX_CABALLOS];
    int min, max;
    int i;
    int key, id_zone;
    int id;
    int avance;
    char tirada[MAX_BUFFER] = "NORMAL";
    comprobacion_argumentos(argc, argv, &num_caballos, &longitud, &num_apostadores, &num_ventanillas);
    info = reservar_memoria_compartida(num_caballos);	
    if(info == NULL) {
        exit(EXIT_FAILURE);
    }
    if(signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        perror("signal");
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    }
    if((monitorpid = fork()) == -1) {
        perror("fork");
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    } else if(monitorpid == 0) {
        monitor();
    }
    if((gestorpid = fork()) == -1) {
        perror("fork");
        kill(0, SIGINT);
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    } else if(gestorpid == 0) {
        gestor_apuestas(num_ventanillas);
    }
    if((apostadorpid = fork()) == -1) {
        perror("fork");
        kill(0, SIGINT);
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    } else if(apostadorpid == 0) {
        apostador(num_apostadores);
    }
    if(pipe(fd) == -1) {
        perror("pipe");
        kill(0, SIGINT);
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < num_caballos; i++) {
        if((caballopid[i] = fork()) == -1) {
            perror("fork");
            kill(0, SIGINT);
            liberar_memoria_compartida();
            exit(EXIT_FAILURE);
        } else if(caballopid[i] == 0) {
            caballo(fd);
        }
    }
    if(signal(SIGALRM, manejador_SIGALRM) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    up_semaforo(info->semaforos, MONITOR, 1);
    down_semaforo(info->semaforos, MONITOR, 1);
    while(info->segundos > 0) {
    	alarm(1);
        pause();
        up_semaforo(info->semaforos, MONITOR, 1);
        down_semaforo(info->semaforos, MONITOR, 1);
    }
    up_semaforo(info->semaforos, NO_MAS_APUESTAS, 1);
    if(signal(SIGUSR1, manejador_SIGUSR1_principal) == SIG_ERR)  {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    while(!info->carrera_terminada) {
        info->carrera_comenzada = 1;
        min = get_posicion(info->ganadores[info->num_caballos-1]);
        max = get_posicion(info->ganadores[0]);
        for(i = 0; i < info->num_caballos; i++) {
            if(min == max) {
                strcpy(tirada, "NORMAL");
            } else if(info->posiciones[i] == min) {
                strcpy(tirada, "REMONTADORA");
            } else if(info->posiciones[i] == max) {
                strcpy(tirada, "GANADORA");
            } else {
                strcpy(tirada, "NORMAL");
            }
            write(fd[1], tirada, sizeof(tirada));
            kill(caballopid[i], SIGUSR1);
            pause();
            read(fd[0], &avance, sizeof(avance));
            info->posiciones[i] += avance; 
        }
        calcular_ganadores();
        if(get_posicion(info->ganadores[0]) >= longitud) {
            info->carrera_terminada = 1;
        }
        up_semaforo(info->semaforos, MONITOR, 1);
        down_semaforo(info->semaforos, MONITOR, 1);
        sleep(1);
    }
    for(i = 0; i < info->num_caballos; i++) {
        kill(caballopid[i], SIGINT);
    }
    sleep(15);
    liberar_memoria_compartida(info);
    
    exit(EXIT_SUCCESS);
}

/**
 * @brief Comprueba los argumentos de entrada
 * 
 * Comprueba que los argumentos de entrada son correctos
 * y asigna el valor a las variables correspondientes
 *
 * @param argc Nº de argumentos del programa
 * @param argv Argumentos de entrada del programa
 * @param num_caballos Variable que guardará el nº de caballos
 * @param longitud Variable que guardará la longitud de la carrera
 * @param num_apostadores Variable que guardará el nº de apostadores
 * @param num_ventanillas Variable que guardará el nº de ventanillas
 */
void comprobacion_argumentos(int argc, char* argv[], int* num_caballos, int* longitud, int* num_apostadores, int* num_ventanillas) {
    if(argc < 5) {
        fprintf(stderr, "Entrada incorrecta, formato esperado: ejercicio final <num_caballos> <longitud_carrera> <num_apostadores> <num_ventanillas>");
        exit(EXIT_FAILURE);
    }
    *num_caballos = atoi(argv[1]);
    if(*num_caballos < 2 || *num_caballos > 10) {
        fprintf(stderr, "Número de caballos no válido: introduzca un número entre 2 y 10.");
        exit(EXIT_FAILURE);
    }
    *longitud = atoi(argv[2]);
    if(*longitud <= 0) {
        fprintf(stderr, "Longitud de carrera no válida: introduzca un número positivo.");
        exit(EXIT_FAILURE);
    }
    *num_apostadores = atoi(argv[3]);
    if(*num_apostadores < 1 || *num_apostadores > 10) {
        fprintf(stderr, "Número de apostadores no válido: introduzca un número entre 1 y 10.");
        exit(EXIT_FAILURE);
    }
    *num_ventanillas = atoi(argv[4]);
    if(*num_ventanillas <= 0) {
        fprintf(stderr, "Número de ventanillas no válido: introduzca un número positivo.");
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief Reserva e inicializa la memoria compartida
 * 
 * Pide al sistema la memoria compartida e inicializa
 * los campos de la información de la carrera, incluidos
 * los semáforos necesarios
 *
 * @param num_caballos Número de caballos de la carrera
 * @return Puntero a la memoria compartida
 */
Info_carrera* reservar_memoria_compartida(int num_caballos) {
    int key, id_zone;
    int i;
    unsigned short semaforos_ini[NUM_SEMAFOROS];
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error with key \n");
        return NULL; 
    }    
    id_zone = shmget(key, sizeof(Info_carrera), IPC_CREAT | IPC_EXCL | SHM_R | SHM_W);
    if (id_zone == -1) {
        fprintf (stderr, "Error with id_zone \n");
        return NULL; 
    }
    info = shmat (id_zone, NULL, 0);
    if (info == NULL) {
        fprintf (stderr, "Error reserve shared memory \n");
        return NULL; 
    }  
    if(crear_semaforo(key, NUM_SEMAFOROS, &info->semaforos) == ERROR) {
        fprintf(stderr, "Error with semaphore");
        shmdt ((char *)info);
        shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
        return NULL;
    }
    semaforos_ini[MUTEX] = 1;
    semaforos_ini[MONITOR] = 0;
    semaforos_ini[NO_MAS_APUESTAS] = 0;
    if(inicializar_semaforo(info->semaforos, semaforos_ini) == ERROR) {
        fprintf(stderr, "Error with initializing semaphore");
        borrar_semaforo(info->semaforos);
        shmdt ((char *)info);
        shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
        return NULL;
    }
    info->id_zone = id_zone;
    info->num_caballos = num_caballos;
    info->segundos = 15;
    info->carrera_comenzada = 0;
    info->carrera_terminada = 0;
    info->dinero_total = 1.0;
    info->num_apuestas = 0;
    for(i = 0; i < num_caballos; i++) {
        info->posiciones[i] = 0;
        info->dinero[i] = 1.0;
        info->cotizaciones[i] = 1.0;
    }
    for(i = 0; i < MAX_CABALLOS; i++) {
        info->ganadores[i] = i+1;
    }
    return info;
}

/**
 * @brief Libera la memoria compartida
 * 
 * Libera la memoria compartida y los semáforos
 *
 */
void liberar_memoria_compartida() {
    int id_zone = info->id_zone;
    borrar_semaforo(info->semaforos);
    shmdt ((char *)info);
    shmctl (id_zone, IPC_RMID, (struct shmid_ds *)NULL);
}

/**
 * @brief Imprime la información de la carrera
 * 
 * Comprueba el estado de la carrera e imprime
 * la información relevante.
 *
 */
void monitor() {
    while(1) {
        down_semaforo(info->semaforos, MONITOR, 1);
        if(!info->carrera_comenzada) {
            imprimir_carrera_no_comenzada();
        } else if (!info->carrera_terminada) {
            imprimir_carrera_comenzada();
        } else {
            imprimir_carrera_terminada();
            exit(EXIT_SUCCESS);
        }
        up_semaforo(info->semaforos, MONITOR, 1); /*No es necesario añadir en el else gracias a la flag de undo*/
    }
    
}

/**
 * @brief Imprime la información de una carrera no comenzada
 * 
 * Imprime que la carrera no ha comenzado, el tiempo que falta
 * para que empiece y la cotización de los caballos.
 *
 */
void imprimir_carrera_no_comenzada() {
    int i = 0;
    down_semaforo(info->semaforos, MUTEX, 1);
    printf("Carrera: NO COMENZADA\n");
    printf("Quedan %d segundos para que empiece.\n", info->segundos);
    printf("Cotización de los caballos:\n");
    for(i = 0; i < info->num_caballos; i++) {
        printf("Caballo %d: %f\n", i+1, info->cotizaciones[i]);
    }
    up_semaforo(info->semaforos, MUTEX, 1);
    printf("\n");
}

/**
 * @brief Imprime la información de una carrera comenzada
 * 
 * Imprime que la carrera ha comenzado, la posición de 
 * cada caballo y su cotización.
 *
 */
void imprimir_carrera_comenzada() {
    int i;
    down_semaforo(info->semaforos, MUTEX, 1);
    printf("Carrera: COMENZADA\n");
    printf("Posiciones de caballos:\n");
    for(i = 0; i < info->num_caballos; i++) {
        printf("%dª posición: Caballo %d\n", i+1, info->ganadores[i]);
    }
    printf("Cotización de los caballos:\n");
    for(i = 0; i < info->num_caballos; i++) {
        printf("Caballo %d: %f\n", i+1, info->cotizaciones[i]);
    }
    up_semaforo(info->semaforos, MUTEX, 1);
    printf("\n");
    kill(getppid(), SIGUSR1);
}

/**
 * @brief Imprime la información de una carrera terminada
 * 
 * Imprime que la carrera ha terminado, los caballos
 * ganadores y el nombre y beneficio de los apostadores
 * que han ganado.
 *
 */
void imprimir_carrera_terminada() {
    int i;
    down_semaforo(info->semaforos, MUTEX, 1);
    printf("Carrera: TERMINADA\n");
    printf("Ganadores:\n");
    for(i = 0; i < info->num_caballos; i++) {
        printf("Puesto %d: Caballo %d \n", i+1, info->ganadores[i]);
    }
    printf("Ganadores de las apuestas:\n");
    for(i = 0; i < info->num_apuestas; i++)  {
        if(info->apuestas[i].num_caballo == info->ganadores[0]) {
            printf("%s: %f$\n", info->apuestas[i].nombre, info->apuestas[i].dinero);
        }
    }
    up_semaforo(info->semaforos, MUTEX, 1);
    kill(getppid(), SIGUSR1);
}

/**
 * @brief Gestiona las apuestas
 * 
 * Reserva la cola de mensajes, crea las ventanillas y
 * espera a que empiece la carrera.
 * 
 * @param num_ventanillas Número de ventanillas a crear
 */
void gestor_apuestas(int num_ventanillas) {
    int i;
    int key;
    pthread_t* ventanillas;
    info->num_apuestas = 0;
    info->dinero_total = 1.0*info->num_caballos;  
    for(i = 0; i < info->num_caballos; i++) {
        info->dinero[i] = 1.0;
        info->cotizaciones[i] = info->dinero_total / info->dinero[i];
    }
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error with key \n");
        exit(EXIT_FAILURE); 
    }  
    msqid = msgget (key, 0600 | IPC_CREAT);  
    if (msqid == -1) {  
        perror("Error al obtener identificador para cola mensajes");  
        exit(EXIT_FAILURE);
    } 
    ventanillas = (pthread_t *) malloc(num_ventanillas*sizeof(pthread_t));
    if(ventanillas == NULL) {
        perror("malloc");
        msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
        exit(EXIT_FAILURE);
    }
    for(i = 0; i < num_ventanillas; i++) {
        if(pthread_create(ventanillas + i, NULL, gestion_apuestas, &msqid) != 0) {
            perror("thread");
            free(ventanillas);
            exit(EXIT_FAILURE);
        }
    }
    down_semaforo(info->semaforos, NO_MAS_APUESTAS, 1);
    for(i = 0; i < num_ventanillas; i++) {
        pthread_join(ventanillas[i], NULL);
    }
    free(ventanillas);
    msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Asume las apuestas
 * 
 * Lee las apuestas, calcula el beneficio en caso de victoria,
 * almacena la apuesta y actualiza la cotización de los caballos
 * 
 * @param arg Id de la cola de mensajes
 */
void* gestion_apuestas(void* arg) {
    Mensaje msg;
    Apuesta apuesta;
    int i;
    msqid = *((int *) arg);
    while(!info->carrera_comenzada) {
        if(msgrcv (msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), 1, IPC_NOWAIT) != -1) {
            strcpy(apuesta.nombre, msg.nombre);
            apuesta.num_caballo = msg.num_caballo;
            info->dinero_total += msg.apuesta;
            info->dinero[apuesta.num_caballo - 1] += msg.apuesta;
            down_semaforo(info->semaforos, MUTEX, 1);
            apuesta.dinero = msg.apuesta * info->cotizaciones[apuesta.num_caballo - 1];
            for(i = 0; i < info->num_caballos; i++) {
                info->cotizaciones[i] = info->dinero_total / info->dinero[i];
            }
            if(info->num_apuestas < MAX_APUESTAS) {
                info->apuestas[info->num_apuestas] = apuesta;
                info->num_apuestas++;
            } else {
                fprintf(stderr, "Error: no caben más apuestas");
            }
            up_semaforo(info->semaforos, MUTEX, 1);
        }
    }
    pthread_exit(NULL);
}

/**
 * @brief Crea apuestas aleatorias
 * 
 * Reserva la cola de mensajes y manda apuestas aleatorias
 * cada 0.1s hasta que empiece la carrera.
 *
 * @param num_apostadores Número de apostadores
 */
void apostador(int num_apostadores) {
    void manejador_SIGINT_apostador();
    int key;
    Mensaje msg;  
    if(signal(SIGINT, manejador_SIGINT_apostador) == SIG_ERR) {
        perror("signal");
        liberar_memoria_compartida();
        exit(EXIT_FAILURE);
    }
    key = ftok(FILEKEY, KEY);
    if (key == -1) {
        fprintf (stderr, "Error with key \n");
        exit(EXIT_FAILURE);
    }  
    msqid = msgget (key, 0600 | IPC_CREAT);  
    if (msqid == -1) {  
        perror("Error al obtener identificador para cola mensajes");  
        msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
        exit(EXIT_FAILURE);
    } 
    srand(getpid());
    while(!info->carrera_comenzada) {
        usleep(100000);
        msg.id = 1;
        sprintf(msg.nombre, "Apostador-%d", rand()%num_apostadores + 1);
        msg.num_caballo = rand() % info->num_caballos + 1;
        msg.apuesta = rand() % 100 / 50 + 1;
        msgsnd(msqid, (struct msgbuf *) &msg,sizeof(Mensaje) - sizeof(long), IPC_NOWAIT);
    }
    msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
    exit(EXIT_SUCCESS);
}

/**
 * @brief Calcula la tirada de un caballo
 * 
 * Recibe la información de tirada del proceso principal,
 * calcula la tirada correspondiente y envía la información
 * al proceso principal.
 * 
 * @param pipe Identificador de la pipe principal-caballo
 */
void caballo(int* pipe) {
    void manejador_SIGUSR1_caballo();
    char readbuffer[MAX_BUFFER];
    int tirada;
    if(signal(SIGUSR1, manejador_SIGUSR1_caballo) == SIG_ERR)  {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    srand(getpid());
    while(!info->carrera_terminada) {
        pause();
        if(read(pipe[0], readbuffer, sizeof(readbuffer)) < 0) {
            perror("Error al leer de la pipe");
            exit(EXIT_FAILURE);
        }
        if(strcmp(readbuffer, "GANADORA") == 0) {
            tirada = tirada_ganadora();
        } else if(strcmp(readbuffer, "REMONTADORA") == 0) {
            tirada = tirada_remontadora();
        } else {
            tirada = tirada_normal();
        }
        write(pipe[1], &tirada, sizeof(tirada));
        kill(getppid(), SIGUSR1);
    }
    exit(EXIT_SUCCESS);
}

/**
 * @brief Calcula una tirada normal
 * 
 * Calcula un número aleatorio entre 1 y 6.
 *
 */
int tirada_normal() {
    return (rand() % 6) + 1;
}

/**
 * @brief Calcula una tirada ganadora
 * 
 * Calcula un número aleatorio entre 1 y 7.
 *
 */
int tirada_ganadora() {
    return (rand() % 7) + 1;
}

/**
 * @brief Calcula una tirada remontadora
 * 
 * Realiza dos tiradas normales y devuelve la suma de
 * los valores.
 *
 */
int tirada_remontadora() {
    return tirada_normal() + tirada_normal();
}

/**
 * @brief Calcula los caballos ganadores
 * 
 * Calcula la posición de los caballos en la carrera.
 *
 */
void calcular_ganadores() {
    int i,j;
    int aux;
    for(i = 1; i < info->num_caballos; i++) {
        aux = info->ganadores[i];
        for(j = i-1; j >= 0 && get_posicion(info->ganadores[j]) < get_posicion(aux); j--) {
            info->ganadores[j+1] = info->ganadores[j];
        }
        info->ganadores[j+1] = aux;
    }
}

/**
 * @brief Calcula la posición de un caballo
 * 
 * Devuelve la posición del caballo que se pide.
 *
 * @param num_caballo Caballo del que calcular posición
 * @return Posición del caballo
 */
int get_posicion(int num_caballo) {
    return info->posiciones[num_caballo - 1];
}

/**
 * @brief Ejecuta el manejador de la señal SIGINT.
 * 
 * Libera la memoria compartida y termina todos los procesos.
 */
void manejador_SIGINT() {
    kill(0, SIGINT);
    liberar_memoria_compartida();
    exit(EXIT_FAILURE);
}

/**
 * @brief Ejecuta el manejador de la señal SIGINT para el apostador.
 * 
 * Libera la cola de mensajes y termina el proceso.
 */
void manejador_SIGINT_apostador() {
    msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
    exit(EXIT_FAILURE);
}

/**
 * @brief Ejecuta el manejador de la señal SIGALRM.
 * 
 * Reduce los segundos restantes en 1.
 */
void manejador_SIGALRM() {
    if(info->segundos > 0) {
        info->segundos--;
    }
}

/**
 * @brief Ejecuta el manejador de la señal SIGUSR1.
 * 
 * Da la orden de continuar la decisión de posiciones.
 */
void manejador_SIGUSR1_principal() {
}

/**
 * @brief Ejecuta el manejador de la señal SIGUSR1.
 * 
 * Desbloquea la determinación de tirada.
 */
void manejador_SIGUSR1_caballo() {
}


