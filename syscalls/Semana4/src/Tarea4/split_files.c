/*
* Autor: Manuel Fresneda Llamas
*/


#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>


void imprimirUso(){
    fprintf(stderr,"Uso: ./split_files [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\n");
    fprintf(stderr,"Divide en ficheros el flujo de bytes que recibe por la entrada estandar\n");
    fprintf(stderr,"El numero maximo de ficheros de salida es de 16.\n");
    fprintf(stderr,"-t BUFSIZE Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n");
    fprintf(stderr,"\n");
}

int comprobarSizeBuff(char* bufsize){
    
    int buf_size = atoi(bufsize); 

    if (buf_size < 1 || buf_size > 1048576)
    {                               
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return buf_size;
}

int comprobarNumFicheros(int argc, char *bufsize){
    
    int contadorFicheros = 0;

    if (bufsize==NULL){
        for(int i=1;i<argc;i++){
            contadorFicheros++;
            if(contadorFicheros>16){
                fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
                imprimirUso();
                exit(EXIT_FAILURE);
            }
        }
        return contadorFicheros;
    }   

    if(bufsize!=NULL){
        for(int i=3;i<argc;i++){
            contadorFicheros++;
            if(contadorFicheros>16){
                fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
                imprimirUso();
                exit(EXIT_FAILURE);
            }
        }
        return contadorFicheros;
    }

    return 0; 
}

int main(int argc, char *argv[]){

    int opt=0;
    int buf_size=0;
    char* bufsize=NULL;
    while ((opt = getopt(argc, argv, "t:h")) != -1)
    {
        switch (opt)
        {
        case 't': 
            bufsize = optarg;
            break;
        case 'h':
            imprimirUso();
            break;

        default: 
            imprimirUso();
            break;
        }
    }

    if (bufsize != NULL)
    {
        buf_size = comprobarSizeBuff(bufsize);
    }
    else
    {
        buf_size = 1024; //Tamaño por defecto
    }


    /* Obtenemos el número de ficheros pasados como argumento */
    int numFicheros = comprobarNumFicheros(argc, bufsize);
    if(numFicheros==0){
        fprintf(stderr,"No hay ficheros de salida\n");
        imprimirUso();
    }


}