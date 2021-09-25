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


    int opt;
    int pid; 
    char *argVisor=NULL;

    //Comprobamos que se realiza la orden correctamente

    if (argc==1){
        fprintf(stderr,"Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[1],"-v")!=0  && argc<=2){
        fprintf(stderr,"Uso: ./openimg 321 -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }

    while((opt=getopt(argc, argv, "v:h")) != -1){
        switch (opt){
            case 'v':
                argVisor=optarg; //asi obtenemos el nombre del argumento que acompaña a "-v"
                break; //funciona
            case 'h':
    
            default:
                fprintf(stderr,"Uso: ./openimg -v123 VISOR [IMGs]\n");
                exit(EXIT_FAILURE);
        }    

    }


    for(int i=3; i<5; i++){
        pid=fork();

        if(pid==0){
            execlp("evince","evince",NULL);
            break; //creamos hijo y salimos de este bucle
        }
        else if(pid==-1){
            perror("fork()");
            exit(EXIT_FAILURE);
            break;
        }
    }

    //Ahora implementamos tanto la logica del padre e hijo
    if(pid==0) {
        fprintf(stdout,"Soy uno de los hijos\n");
        execlp("evince","evince",NULL);
        exit(EXIT_SUCCESS);
    }
    else{
        for (int i=3; i<argc; i++) {		// esperamos a que todos los hijos terminen (código mejorado)
    		if ((wait(NULL)) >= 0) {
    			fprintf(stdout,"Esperando q termine\n");
    		}

        }
        fprintf(stdout,"Soy el padre\n");
    }





}