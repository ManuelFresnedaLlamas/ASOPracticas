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
    fprintf(stderr,"Uso: ./mystrings [-t BUFSIZE] [-n MINLENGTH]\n");
    fprintf(stderr,"Lee de la entrada estándar el flujo de bytes recibido y escribe en la salida estándar las cadenas compuestas por caracteres imprimibles incluyendo espacios, tabuladores y saltos de línea, que tengan una longitud mayor o igual a un tamaño dado,\n");
    fprintf(stderr,"-t BUFSIZE\tTamaño de buffer donde MINLENGTH <= BUFSIZE <= 1MB (por defecto 1024).\n");
    fprintf(stderr,"-n MINLENGTH\tLongitud mínima de la cadena. Mayor que 0 y menor que 256(por defecto 4).\n");
}

int comprobarSizeBuff(char *memoria, int min_length)
{
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=1MB = 1024 -> por defecto 1024
    */
    int aux = atoi(memoria); 

    if (aux<min_length || aux < 1 || aux > 1048576)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return aux;
}

int comprobarSizeCadena(char *cadena)
{
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=128MB=134217728
    */
    int aux = atoi(cadena); 

    if (aux < 0 || aux > 255)
    {
        fprintf(stderr, "La longitud mínima de cnadena tiene que ser mayor que 0 y menor que 256.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return aux;
}

int main(int argc, char *argv[]){
    
    int opt=0;
    int buf_size=0;
    char * bufsize=NULL;
    int min_length=0;
    char* minlength=0;
    
    while ((opt = getopt(argc, argv, "t:n:h")) != -1)
    {
        switch (opt)
        {
        case 't': 
            bufsize = optarg;
            break;
        case 'n':
            minlength = optarg;
            break;
        case 'h':
            imprimirUso();
            break;

        default: 
            imprimirUso();
            break;
        }
    }

    if (minlength != NULL)
    {
        min_length = comprobarSizeCadena(minlength);
    }
    else
    {
        min_length = 4; //Tamaño por defecto
    }

    if (bufsize != NULL)
    {
        buf_size = comprobarSizeBuff(bufsize,min_length);
    }
    else
    {
        buf_size = 1024; //Tamaño por defecto
    }


    /* Abrimos el fd para la entrada estándar */
    //int fdin=STDIN_FILENO;
    int fdin=open("f1",O_RDONLY);

    if(fdin==-1){
        perror("open(fdin)");
        exit(EXIT_FAILURE);
    }

    /* Abrimos el fd para la salida estándar */

    int fdout=STDOUT_FILENO;

    if(fdout==-1){
        perror("open(fdout)");
        exit(EXIT_FAILURE);
    }

    char *bufLectura = NULL;
    if ((bufLectura = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    char *bufEscritura = NULL;
    if ((bufEscritura = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    char *bufAux = NULL;
    if ((bufAux = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }



    /* 
    *
    * Tratamos lecturas parciales. 
    * Por otro lado, leeremos bytes hasta que no reciba mas bytes.
    * 
    */
    int flag=0;
    int num_written=0;
    int num_read=0;
    int puntero=0;
    int leidos = 0;
    int a_Leer = buf_size;
    
    
    while(flag==0){
        
        while ((a_Leer > 0 && (num_read = read(fdin, bufLectura + leidos, a_Leer)) == -1))
        {
            // if (num_read == 0)
            // {
            //     //FIN LECTURA
            //     flag=1;
            //     break;
            // }
            a_Leer = a_Leer - num_read;
            leidos = leidos + num_read;
        }
        if (num_read == 0)
        {
            //FIN LECTURA
            flag=1;
            //break;
        }

        for (int v = 0; v < num_read; v++){
            int a_Escribir = puntero;
            int escritos = 0;
            if (puntero == num_read)
            {
                while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, a_Escribir)) == -1))
                {
                    a_Escribir = a_Escribir - num_written;
                    escritos = escritos + num_written;
                }
                puntero = 0;
            } 


            bufAux=bufLectura;
            bufEscritura[puntero]=bufAux[v];
            puntero++;
        }


    }



}