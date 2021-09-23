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


//Si no se especifica -v VISOR -> EXIT FAILURE mostrando USO:... //hecho
//
//Si no se encuentra VISOR -> ERROR por stderr //hecho
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

    if (argc<=2)
    {
        fprintf(stderr,"Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }
    
    if(strcmp(argv[1],"-v")!=0){
        fprintf(stderr, "Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }
    
    if (argc>=2){
        fprintf(stderr, "Error: No hay imágenes que visualizar\n");
        exit(EXIT_FAILURE);
    }
    sprintf(argComando,"%s",argv[2]);

    if(execlp(argComando,argComando,NULL)==-1){ //funciona correcta,emte
        fprintf(stderr,"Error: '%s' no encontrado\n",argComando);
        exit(EXIT_FAILURE);
    }


    //tendre que guardar los pid de los fork y comprobar que todos se mueren en el
    //padre
    // for(int i=3; i<argc+1; i++){
    //     switch (pid=fork){
    //         case -1: //fork habrá fallado
    //             perror("fork()");
    //             exit(EXIT_FAILURE);
    //             break;
    //         case 0: //ha funcionado y soy el hijo, aquí se pone la implementación
    //             //TODO

    //         default: //soy el padre al crearse el hijo y esperare a todos los pid
    //             if(wait(NULL)==-1){
    //                 perror("wait()");
    //                 exit(EXIT_FAILURE);
    //             }
    //     }
    // }



}