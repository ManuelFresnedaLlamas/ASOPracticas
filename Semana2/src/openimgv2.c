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


//Si no se especifica -v VISOR -> EXIT FAILURE mostrando USO:... -> HECHO
//
//Si no se encuentra VISOR -> ERROR por stderr -> HECHO
//
//Se crea un proceso hijo para cada imagen
//
// Si no se indica imagen -> ERROR por stderr
//TODOS LOS PROCESOS HIJO DEBEN CREARSE AN>TES DE ESPERAR AL PRIMERO
//PADRE ESPERA QUE TERMINEN TODOS EN ORDEN DE CREAciON
//
//Cada vez que termine un hijo, padre muestra mensaje status
int main(int argc, char **argv){

    int opt;

    char *argVisor=NULL;
    int optind=1;
    //Comprobamos que se realiza la orden correctamente
    if(argc<=1 || strcmp(argv[1],"-v")!=0 || strcmp(argv[1],"-")!=0){
        fprintf(stderr,"Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }

    while((opt=getopt(argc, argv, "v:h")) != -1){
        switch (opt){
            case 'v':
                argVisor=optarg; //asi obtenemos el nombre del argumento que acompaña a "-v"
                if(execlp(argVisor,argVisor,NULL)==-1){
                    fprintf(stderr,"Error: '%s' no encontrado\n",argVisor);
                    exit(EXIT_FAILURE);
                }
                
                break; //funciona
            
            case 'h':
    
            default:
                fprintf(stderr,"Uso: ./openimg -v VISOR [IMGs]\n");
                exit(EXIT_FAILURE);
        }    

    }


}