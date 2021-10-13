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


void imprimirUso(){
    fprintf(stderr,"Uso: ./mystrings [-t BUFSIZE] [-n MINLENGTH]\n");
    fprintf(stderr,"Lee de la entrada estándar el flujo de bytes recibido y escribe en la salida estándar las cadenas compuestas por caracteres imprimibles incluyendo espacios, tabuladores y saltos de línea, que tengan una longitud mayor o igual a un tamaño dado,\n");
    fprintf(stderr,"-t BUFSIZE\tTamaño de buffer donde MINLENGTH <= BUFSIZE <= 1MB (por defecto 1024).\n");
    fprintf(stderr,"-n MINLENGTH\tLongitud mínima de la cadena. Mayor que 0 y menor que 256(por defecto 4).\n");
}

int comprobarSizeBuff(char *memoria)
{
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=1MB = 1024 -> por defecto 1024
    */
    int aux = atoi(memoria); 

    if (aux < 1 || aux > 1024)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return aux;
}

int main(int argc, char *argv[]){
    
    int opt;
    int buf_size;
    char * bufsize;
    char * min_length;
    
    while ((opt = getopt(argc, argv, "t:n:h")) != -1)
    {
        switch (opt)
        {
        case 't': 
            bufsize = optarg;
            break;
        case 'n':
            min_length = optarg;
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

}