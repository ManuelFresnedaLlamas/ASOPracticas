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
    int pid;
    int checkAcceso=0;
    int status=0;
                    int a=0;
    int arrayPID[argc-2];

    char *argVisor=NULL;
    int optind=1;
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
                // if(execlp(argVisor,argVisor,NULL)==-1){
                //     fprintf(stderr,"Error: '%s' no encontrado\n",argVisor);
                //     exit(EXIT_FAILURE);
                // }
                
                break; //funciona
            
            case 'h':
    
            default:
                fprintf(stderr,"Uso: ./openimg -v123 VISOR [IMGs]\n");
                exit(EXIT_FAILURE);
        }    

    }
    //necesito un buffer donde guardar los PID y esperar a que todos los PID terminen
    
    for(int i=3;i<argc-1;i++){
       // arrayPID[i-3]=fork(); //creamos proceso hijo
        
        switch (pid=fork()){
            case -1: //error de fork
                perror("fork():"); //perror se usa cuando son errores de llamadas al sistema
                exit(EXIT_FAILURE);
                break;
            case 0: //ahora soy el hijo
                //tendremos que comprobar que existen las imagenes
                checkAcceso=access(argv[i],F_OK); //si existe
                if (checkAcceso==-1){
                    fprintf(stderr,"No deberia entrar aqui\n");
                    exit(EXIT_FAILURE);
                }
                if(execlp(argVisor,argVisor,argv[i],NULL)==-1){
                    fprintf(stderr,"Error: '%s' no encontrado\n",argVisor);
                    exit(EXIT_FAILURE);
                    break;
                }

            default: //lo que hace el padre
                arrayPID[i-3]=pid;
                //for(int a=0;a<3;a++){
                    //if (waitpid(arrayPID[a],&status,0)==-1 && waitpid(arrayPID[a+1],&status,0)==-1 && waitpid(arrayPID[a+2],&status,0)==-1){ /* Espera a que termine el proceso hijo */ 
                    if(waitpid(arrayPID[i-3],&status,WUNTRACED)){
                        fprintf(stderr,"No deberia entrar aqui aun\n");
                        perror("waitpid()");
                        //exit(EXIT_FAILURE);
                    }
        }
        
    }

}