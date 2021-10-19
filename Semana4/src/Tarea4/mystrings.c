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

    if (aux < 1 || aux > 255)
    {
        fprintf(stderr, "La longitud mínima de cadena tiene que ser mayor que 0 y menor que 256.\n");
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
    int contador=0;
    int flag_global=0;
    
    
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
        if(num_read!=0){
            int num_read_aux=num_read;
        }
        if (num_read == 0)
        {
            //FIN LECTURA
            //flag=1;
            //break;
        }
        int a_Escribir = puntero;
        int escritos = 0;
        if(puntero==buf_size){
            while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, a_Escribir)) == -1))
            {
                a_Escribir = a_Escribir - num_written;
                escritos = escritos + num_written;
            }
        }else{
            if((puntero<buf_size) && (num_read==0) && (flag==1)){
                if(puntero<min_length){
                    //break;
                }else{
                    // if((contador%11)==min_length){
                    //     while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, a_Escribir)) == -1))
                    //     {
                    //         a_Escribir = a_Escribir - num_written;
                    //         escritos = escritos + num_written;

                    //     }

                    // }
                }

            }
        }

        //contador=0;

        bufAux=bufLectura;
        int i=0;
        int contador_aux;
        while(contador!=num_read){
            contador_aux=0;
            for(int i=0;i<min_length;i++){
                if(isprint(bufAux[puntero])){//} || bufAux[puntero]=='\n' || bufAux[puntero]=='\b'){
                    bufEscritura[i]=bufAux[puntero];
                    contador++;
                    contador_aux++;
                }
                puntero++;
            }
            a_Escribir = puntero;

            if(puntero>contador){
                flag=1;
                
            }
            if(contador_aux==min_length){
                while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, a_Escribir)) == -1))
                {
                    a_Escribir = a_Escribir - num_written;
                    escritos = escritos + num_written;

                }
                break;

            }
            if(contador_aux<min_length){
                break;
            }
            if(puntero>=num_read && contador_aux>=0){
                while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, contador_aux)) == -1))
                {
                    a_Escribir = a_Escribir - num_written;
                    escritos = escritos + num_written;

                }
                flag=1;
                break;
            }
            
        }

        // for (int v = 0; v < min_length; v++){

        //     if (puntero == buf_size)
        //     {
        //         while ((a_Escribir > 0 && (num_written = write(fdout, bufEscritura + escritos, a_Escribir)) == -1))
        //         {
        //             a_Escribir = a_Escribir - num_written;
        //             escritos = escritos + num_written;
        //         }
        //         puntero = 0;
        //     } 

            
        //     bufAux=bufLectura;
        //     if(isprint(bufAux[puntero])){
        //         bufEscritura[v]=bufAux[puntero];
        //         puntero++;
        //         contador++;
        //     }else{
        //         if(bufAux[puntero]!='\n' && bufAux[puntero]!='\b'){
        //             bufEscritura[puntero]=bufAux[puntero];
        //             puntero++;
        //             contador++;
        //         }
        //         puntero++;
        //         //contador++;
        //     }
        // }


    }



}