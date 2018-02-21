/**
 * @brief Código del ejercicio 1
 * 
 * Este fichero contiene el código del ejercicio 1 de la práctica 4.
 * @file cadena_montaje.c
 * @author Miguel García Moya miguel.garciamoya@estudiante.uam.es Grupo 2201
 * @date 10-05-2017
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>  
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>  
#include <sys/msg.h>  

#define N 33     /*!< Clave para ftok */
#define A_A_B 1  /*!< Id de los mensajes entre A y B */
#define B_A_C 2  /*!< Id de los mensajes entre B y C */

typedef struct {  
    long id;              /*!< Tipo de mensaje*/   
    char trozo[1024*4];   /*!< Cadena a transmitir*/   
} Mensaje;  

void proceso_a(int msqid, char* input_path);
void proceso_b(int msqid);
void proceso_c(int msqid, char* output_path);

int sigint_flag = 0; /*!< Flag que se pone a 1 al recibir SIGINT*/   

/**
 * @brief Ejecuta el código del ejercicio 1.
 * 
 * Crea los tres procesos de la cadena de montaje y reserva la
 * cola de mensajes, además de cortar la cadena de montaje al
 * recibir SIGINT.
 * 
 * @param argc Número de argumentos del programa
 * @param argv Argumentos del programa: ficheros de entrada y salida
 */
int main(int argc, char* argv[]) {  
    key_t clave;  
    sigset_t set;
    void manejador_SIGINT();
    int msqid;  
    int pid_a, pid_b, pid_c;
    int i;
    if(argc < 3) {
        fprintf(stderr, "Argumentos incorrectos, formato esperado es: $ cadena_montaje <f1> <f2>");
        exit(EXIT_FAILURE);
    }
    if(sigfillset(&set) == -1) {
        fprintf (stderr, "Error with set \n");
        return -1; 
    }
    if(sigdelset(&set, SIGINT) == -1) {
        fprintf (stderr, "Error with set \n");
        return -1; 
    } 
    if(sigprocmask(SIG_SETMASK, &set,NULL) == -1) {
        fprintf (stderr, "Error with mask \n");
        return -1; 
    }    
    if(signal(SIGINT, manejador_SIGINT) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }
    clave = ftok ("/bin/ls", N);  
    if (clave == (key_t) -1)  {  
        perror("Error al obtener clave para cola mensajes\n");  
        exit(EXIT_FAILURE);  
    }  
    msqid = msgget (clave, 0600 | IPC_CREAT);  
    if (msqid == -1) {  
        perror("Error al obtener identificador para cola mensajes");  
        return(0);  
    }  
    if((pid_a = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid_a == 0) {
        proceso_a(msqid, argv[1]);
    }
    if((pid_b = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid_b == 0) {
        proceso_b(msqid);
    }
    if((pid_c = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid_c == 0) {
        proceso_c(msqid, argv[2]);
    }
    while(!sigint_flag);
    kill(pid_a, SIGINT);
    kill(pid_b, SIGINT);
    kill(pid_c, SIGINT);
    msgctl (msqid, IPC_RMID, (struct msqid_ds *)NULL);
    exit(EXIT_SUCCESS);   
}  

/**
 * @brief Ejecuta el código del proceso A.
 * 
 * Lee del fichero de entrada y manda la información a B.
 * 
 * @param msqid Id de la cola de mensajes
 * @param input_path Ruta del fichero de entrada
 */
void proceso_a(int msqid, char* input_path) {
    int input_file;
    Mensaje msg;  
    input_file = open(input_path, O_RDONLY);
    if(input_file == -1) {
        perror("open");
        kill(getppid(), SIGINT);
        exit(EXIT_FAILURE);
    }
    while(!sigint_flag) {
    	if(read(input_file, msg.trozo, 1024*4) == -1) {
            perror("read");
            close(input_file);
            kill(getppid(), SIGINT);
            exit(EXIT_FAILURE);
        }
        msg.id = A_A_B;   
        msgsnd (msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), IPC_NOWAIT);  
        sleep(5);
    }
    close(input_file);
    exit(EXIT_SUCCESS);  
}

/**
 * @brief Ejecuta el código del proceso B.
 * 
 * Lee el mensaje de A, lo pone en mayúsculas y lo
 * transmite a C.
 * 
 * @param msqid Id de la cola de mensajes
 */
void proceso_b(int msqid) {
    Mensaje msg;
    int i = 0;
    while(!sigint_flag) {
        if(msgrcv (msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), A_A_B, IPC_NOWAIT) != -1) {  
            while(msg.trozo[i]) {
                msg.trozo[i] = toupper(msg.trozo[i]);
                i++;
            }
            msg.id = B_A_C;
            msgsnd(msqid, (struct msgbuf *) &msg, sizeof(Mensaje) - sizeof(long), IPC_NOWAIT);
        } 
    } 
    exit(EXIT_SUCCESS);
}

/**
 * @brief Ejecuta el código del proceso C.
 * 
 * Lee el mensaje de B y lo imprime en el fichero de salida.
 * 
 * @param msqid Id de la cola de mensajes
 * @param output_path Ruta del fichero de salida
 */
void proceso_c(int msqid, char* output_path) {
    int output_file;
    Mensaje msg;
    output_file = open(output_path, O_CREAT | O_WRONLY);
        if(output_file == -1) {
        perror("open");
        kill(getppid(), SIGINT);
        exit(EXIT_FAILURE);
    }
    while(!sigint_flag) {
        if(msgrcv (msqid, (struct msgbuf *)&msg, sizeof(Mensaje) - sizeof(long), B_A_C, IPC_NOWAIT) != -1) {
            if(write(output_file, msg.trozo, strlen(msg.trozo)) == -1) {
                perror("write");
                close(output_file);
                kill(getppid(), SIGINT);
                exit(EXIT_FAILURE);
            }
        }
    }
    close(output_file);
    exit(EXIT_SUCCESS); 
}

/**
 * @brief Ejecuta el manejador de la señal SIGINT.
 * 
 * Pone la flag correspondiente a SIGINT a 1.
 */
void manejador_SIGINT() {
    if(sigint_flag == 0) {
        sigint_flag = 1;
    }
}


