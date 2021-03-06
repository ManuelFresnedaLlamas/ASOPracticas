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

void imprimirUso()
{
    fprintf(stderr, "Uso: ./split_files [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\n");
    fprintf(stderr, "Divide en ficheros el flujo de bytes que recibe por la entrada estandar\n");
    fprintf(stderr, "El numero maximo de ficheros de salida es de 16.\n");
    fprintf(stderr, "-t BUFSIZE Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n");
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}

int comprobarSizeBuff(char *bufsize)
{

    int buf_size = atoi(bufsize);

    if (buf_size < 1 || buf_size > 1048576)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return buf_size;
}

int comprobarNumFicheros(int argc, char *bufsize)
{

    int contadorFicheros = 0;

    if (bufsize == NULL)
    {
        for (int i = 1; i < argc; i++)
        {
            contadorFicheros++;
            if (contadorFicheros > 16)
            {
                fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
                imprimirUso();
                exit(EXIT_FAILURE);
            }
        }
        return contadorFicheros;
    }

    if (bufsize != NULL)
    {
        for (int i = 3; i < argc; i++)
        {
            contadorFicheros++;
            if (contadorFicheros > 16)
            {
                fprintf(stderr, "Error: Demasiados ficheros de salida.\n");
                imprimirUso();
                exit(EXIT_FAILURE);
            }
        }
        return contadorFicheros;
    }

    return 0;
}

int main(int argc, char *argv[])
{

    int opt = 0;
    int buf_size = 0;
    char *bufsize = NULL;
    char *bufEscritura = NULL;
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
    if (numFicheros == 0)
    {
        fprintf(stderr, "No hay ficheros de salida\n");
        imprimirUso();
    }

    /* Abrimos el fd para la entrada estándar */
    int fdin = STDIN_FILENO;
    //int fdin=open("f5",O_RDONLY);

    int arrayFD[numFicheros];

    for (int i = optind; i < argc; i++)
    {
        int fdout = open(argv[i], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR); //se trucaran y sino se crean
        if (fdout == -1)
        {
            fprintf(stderr,"Error: No se puede abrir/crear '%s': Permission denied\n",argv[i]);
            exit(EXIT_FAILURE);
        }
        arrayFD[i - optind] = fdout;
    }

    char* bufLectura;
    if ((bufLectura = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }


    /* Para optimizar las llamadas a read/write, utilizare un buffer de lectura de bytes de llegada
    *  y por otro lado, tantos buffer como numFicheros tengamos para escribir,escribiremos en los 
    *  ficheros cuando estos se llenen. Entonces, creamos un conjunto buffer donde añadiremos los buffer 
    *  descritos anteriormente dedicados a la escritura.
    */
    char *bufAux = NULL;
    if ((bufAux = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }


    char *bufAux_ = NULL;
    if ((bufAux_ = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    char *cjtoBuffer[numFicheros];
    for (int i = 0; i < numFicheros; i++)
    {
        if ((bufEscritura = (char *)malloc(buf_size * sizeof(char))) == NULL)
        {
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
        cjtoBuffer[i] = bufEscritura;
    }

    /* Necesitare controlar el tamaño de los buffer de escritura de dicho conjunto
    *  para saber que "capacidad de llenado" tiene en un momento determinado
    *  por tanto, creo un array de tamaño numFichero que mantendra la posición/cantidad 
    *  de llenado.
    */ 

    int llenadoFicheros[numFicheros];
    for(int i=0; i<numFicheros;i++){
        llenadoFicheros[i]=0;              //Reinicializo por lo que pudiera pasar
    }

    /* Definimos las variables necesarias para el algoritmo */

    int a_Leer=buf_size;
    int leidos=0;
    int a_Escribir=buf_size;
    int escritos=0;
    ssize_t num_written=0;
    ssize_t num_read=0;
    int punteroLectura=0;
    int punteroFicheroReferenciado=0;
    
    //Nuestra flag
    int finEntrada=0;

    while(finEntrada!=1){ //CONDICION A MODIFICAR

        while ((a_Leer > 0 && (num_read = read(fdin, bufLectura + leidos, a_Leer)) == -1))
        {
            a_Leer = a_Leer - num_read;
            leidos = leidos + num_read;
        } 

        bufAux=bufLectura;


        int punterowrite=0;
        
        for(int i=0;i<num_read;i++){
            if (punteroFicheroReferenciado==numFicheros){ //Asi aseguramos reiniciar la posicion del puntero a los ficheros
                punteroFicheroReferenciado=0;
                punterowrite++;
            }
            if(llenadoFicheros[punteroFicheroReferenciado]==buf_size){                                                                                //CUIDADO CON ESTE PARAMETRO
                while ((a_Escribir > 0 && (num_written = write(arrayFD[punteroFicheroReferenciado], cjtoBuffer[punteroFicheroReferenciado] + escritos, a_Escribir)) == -1))
                {
                    a_Escribir = a_Escribir - num_written;
                    escritos = escritos + num_written;
                }
                llenadoFicheros[punteroFicheroReferenciado]=0;
            }
            
            bufAux_=cjtoBuffer[punteroFicheroReferenciado];
            bufAux_[llenadoFicheros[punteroFicheroReferenciado]]=bufLectura[i];
            cjtoBuffer[punteroFicheroReferenciado]=bufAux_;
            llenadoFicheros[punteroFicheroReferenciado]++;
            punteroFicheroReferenciado++;


        }

        if(num_read==0){
            for(int i=0;i<numFicheros;i++){
                if(llenadoFicheros[i]!=0){
                    while ((a_Escribir > 0 && (num_written = write(arrayFD[i], cjtoBuffer[i] + escritos, llenadoFicheros[i])) == -1))
                    {
                        a_Escribir = a_Escribir - num_written;
                        escritos = escritos + num_written;
                    }
                }
            }
        }
        if(num_read==0){
            finEntrada=1;
        }

    }

  // Con liberar estos buffers bastará, puesto que han habido distintas igualdades más arriba
    free(bufLectura);
    for(int i=0;i<numFicheros;i++){
        free(cjtoBuffer[i]);
    }
}