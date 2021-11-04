/*
Autor: Manuel Fresneda Llamas
*/


//CONCATENAR merge_files -> mystrings -> split_files

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void imprimirUso(void){
    fprintf(stderr,"Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n");
    fprintf(stderr,"-t BUFSIZE  Tamaño de buffer donde 1<= BUFSIZE <= 1MB (por defecto 1024).\n");
    fprintf(stderr,"-n MINLENGTH    Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n");
    fprintf(stderr,"-i  Lista de ficheros de entrada separados por comas.\n");
}

int main(int argc, char **argv){

    int opt;
    char * minlength=NULL;
    char * bufsize=NULL;
    char * ficherosEntrada=NULL;


    while ((opt = getopt(argc, argv, "t:i:n:h")) != -1) //: al final indica que esperan argumento
    {
        switch (opt)
        {
        case 'h':
            imprimirUso();
            exit(EXIT_SUCCESS);
        case 't': 
            bufsize = optarg;
            break;
        case 'n':
            minlength = optarg;
            break;
        case 'i':
            ficherosEntrada = optarg;
            break;
        default: 
            fprintf(stderr,"Error: Deben proporcionarse ficheros de entrada con la opción -i\n");
            imprimirUso();
            break;
        }
    }

    if (ficherosEntrada==NULL){
        fprintf(stderr,"Error: Deben proporcionarse ficheros de entrada con la opción -i\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }
    char *arrayFicherosEntrada[16]; //Como máximo, entrarán 16 ficheros, así que reservamos una array de dicho tamaño
    char *ptrToken=NULL; 
    char *saveptr=NULL; 

    int contador=0; //Cuenta ficheros de entrada
 
    ptrToken = strtok_r(ficherosEntrada, ",", &saveptr); 
    while (ptrToken != NULL) 
    { 
        arrayFicherosEntrada[contador]=ptrToken;
        contador++;
        printf("%s\n", ptrToken); 
        ptrToken = strtok_r(NULL, ",", &saveptr); 
    } 
    for(int i=0;i<contador;i++){
        fprintf(stderr,"%s ",arrayFicherosEntrada[i]); //Devuelve f1 f2 f3
    }

    fprintf(stderr,"%s",ficherosEntrada);
}
   
