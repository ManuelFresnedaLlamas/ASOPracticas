#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>


//Si no se especifica -v VISOR -> EXIT FAILURE mostrando USO:...
//
//Si no se encuentra VISOR -> ERROR por stderr
//
//Se crea un proceso hijo para cada imagen
//
// Si no se indica imagen -> ERROR por stderr
//TODOS LOS PROCESOS HIJO DEBEN CREARSE AN>TES DE ESPERAR AL PRIMERO
//PADRE ESPERA QUE TERMINEN TODOS EN ORDEN DE CREAciON
//
//Cada vez que termine un hijo, padre muestra mensaje status
int main(int argc, char **argv){

    int pid;
    char argComando[255]={0}; //le doy un tamaño exageradamente grande para que pueda caber cualquier cosa
    sprintf(argComando,"%s",argv[2]);

    if(strcmp(argv[1],"-v")!=0){
        fprintf(stderr, "Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }
    if(execlp(argComando,argComando,NULL)==-1){ //funciona correcta,emte
        fprintf(stderr,"Error: '%s' no encontrado\n",argComando);
        exit(EXIT_FAILURE);
    }
    // for(int i=3; i<argc+1; i++){
    //     if(execlp("inkscape","inkscape",argv[i],NULL)==-1){
    //         fprintf(stderr,"Error: '%s' no encontrado",argv[2]);
    //     }
    // }
    //Se necesita bucle para realizar el execlp
    //execlp("inkscape","inkscape",argv[i]);

    switch (pid=fork()){
        case -1: /* fork() falló */
            perror("fork()");
            exit(EXIT_FAILURE);
            break;
        default:                  /* Ejecución del proceso padre tras fork() con éxito */
            if (wait(NULL) == -1) /* Espera a que termine el proceso hijo */
            {   //waitpid(-1,&status, 0)==-1
                perror("wait()");
                //perror("Waitpid()");
                exit(EXIT_FAILURE);
            }
        break;
        //fprintf(stderr, "El pid %d ha terminado\n", pid, status);

    }

}