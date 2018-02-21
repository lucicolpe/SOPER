
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>/*Libreria señales*/

/**
* @brief funcion de paso de señal y dormir
* @return int: valor de exito (0) o fracaso (-1)
*/
int main(){
    int fpid;
    fpid = fork();
    if(fpid < 0){
        printf("Error al emplear el fork.");
        return -1;
    }
    if(!fpid){
        while(1){
            printf("Soy el proceso hijo con PID: %d\n", getpid());
            fflush(stdout);
            sleep(5);/*Se manda dormir al hijo*/
        }
    }
    else{
        sleep(30);/*Se manda dormir al padre*/
        kill(fpid, SIGHUP);/*Se manda una señal de finalizacón cualquiera al hijo*/
        return 0;
    }
    return 0;
}
