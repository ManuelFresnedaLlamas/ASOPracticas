#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv){
    
    int pid;
    int checkAcceso=0;

    if (argc<2){
        fprintf(stderr, "USO: openpdf fichero.pdf\n");
        exit(EXIT_FAILURE);
    }


    pid=fork();
    switch (pid)
    {
        case -1:
            perror("Fork():"); //perror se usa cuando son errores de llamadas al sistema
            exit(EXIT_FAILURE);
            break;
        case 0:
            //soy el hijo
            checkAcceso=access(argv[1],F_OK);
            if(checkAcceso==-1){
                //fprintf(stderr, "No existe el fichero dado como argumento\n");
                perror("acces()");
                exit(EXIT_FAILURE);
                //sobra el break;
            }
            execlp("evince","evince",argv[1],NULL);
            fprintf(stderr, "execlp() failed\n"); /* Esta línea no se debería ejecutar si la anterior tuvo éxito */
            exit(EXIT_FAILURE);
            break;
        default:
            //soy el padre, por tanto esperaremos al hijo
            //sleep(3600);
            if(wait(NULL)==-1){
                perror("wait()");
                exit(EXIT_FAILURE);
            }
    }



}