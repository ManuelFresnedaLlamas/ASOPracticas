/*
Autor: Manuel Fresneda Llamas
*/

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

void imprimirUso(void)
{
    fprintf(stderr, "Uso: merge_files [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ...FILEINn]\n");
}

void imprimirInstrucciones(void)
{

    fprintf(stderr, "NO admite lectura de la entrada estandar.\n");
    fprintf(stderr, "-t BUFSIZE Tamaño de buffer donde 1<= BUFSIZE <=128MB\n");
    fprintf(stderr, "-o FILEOUT Usa FILEOUT en lugar de la salida estandar\n");
}

int comprobarSizeBuff(char *memoria)
{
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=128MB=134217728
    */
    int aux = atoi(memoria); 

    if (aux < 1 || aux > 134217718)
    {
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }

    return aux;
}

int comprobarNumFicheros(int argc, char *memReservar, char *ficheroSalida)
{
    //Comprobamos que no se superen 16 ficheros.
    int contadorAux = 0;
    //si memRe!=NULL -> argc [1] y argc[2] llenos
    //si ficheroSalida tambien != NULL -> argc[3] y argc[4]
    if (memReservar != NULL && ficheroSalida != NULL)
    {
        for (int i = 5; i < argc; i++)
        {
            contadorAux++;
            if (contadorAux > 16)
            { //si tenemos más de 16 ficheros
                fprintf(stderr, "Se ha reservado ambos\n");
                fprintf(stderr, "Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return contadorAux;
    }
    //si solo uno de ambos tiene NULL -> arg [1] y [2] ocupados
    if ((memReservar != NULL || ficheroSalida != NULL) && argc > 3)
    {
        for (int i = 3; i < argc; i++)
        {
            contadorAux++;
            if (contadorAux > 16)
            { //si tenemos más de 16 ficheros
                fprintf(stderr, "Se ha reservado mem o hay fichout\n");
                fprintf(stderr, "Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return contadorAux;
    }

    if (memReservar == NULL && ficheroSalida == NULL)
    {
        for (int i = 1; i < argc; i++)
        {
            contadorAux++;
            if (contadorAux > 16)
            {
                fprintf(stderr, "Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return contadorAux;
    }

    if ((memReservar == NULL || ficheroSalida == NULL) && argc <= 3)
    {
        fprintf(stderr, "Error: No hay ficheros de entrada\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int asignarFicheroSalida(char *ficheroSalida)
{
    int fdout = 0;
    //Si se da fichero salida, abrimos y guardamos su fd
    if (ficheroSalida != NULL)
    {
        fdout = open(ficheroSalida, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fdout == -1)
        {
            perror("open(fileout)");
            exit(EXIT_FAILURE);
        }
    }
    else
    { /* Por defecto, la salida estándar */
        fdout = STDOUT_FILENO;
    }

    return fdout;
}


int main(int argc, char **argv)
{
    int opt;
    int buf_size;
    char *memReservar = NULL;
    char *ficheroSalida = NULL;
    char *buf = NULL;

    if (argc == 1)
    {
        fprintf(stderr, "Error: No hay ficheros de entrada.\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "t:o:")) != -1) //: al final indica que esperan argumento
    {
        switch (opt)
        {
        case 't': 
            memReservar = optarg;
            break;
        case 'o':
            ficheroSalida = optarg;
            break;

        default: 
            fprintf(stderr, "Error: No hay ficheros de entrada\n");
            imprimirUso();
            imprimirInstrucciones();
            break;
        }
    }
    if (memReservar != NULL)
    {
        buf_size = comprobarSizeBuff(memReservar);
    }
    else
    {
        buf_size = 1024; //Tamaño por defecto
    }

    /* Obtenemos el número de ficheros pasados como argumento */
    int numFicheros = comprobarNumFicheros(argc, memReservar, ficheroSalida);

    /* Comprobamos cual es el fichero de salida */

    int fdout = asignarFicheroSalida(ficheroSalida);

    /* Guardamos los FD's en un array del tamaño de numFicheros */
    //int posAux=0;
    int posicionFicheroMalo=0;

    for (int i = optind; i < argc; i++)
    {
        int checkAccess=access(argv[i],F_OK); //Podríamos hacerlo más adelante al usar la función open()
        //posAux++;
        if(checkAccess==-1){
            char * aux=argv[i];
            fprintf(stderr,"Error:No se puede abrir %s: No such file or directory\n",aux);
            numFicheros--;
            posicionFicheroMalo=i;
        }
    }
    int activador=0;
    int arrayFD[numFicheros];

    for(int i=optind;i<argc;i++){
        if(activador!=0){
            int fdin = open(argv[i], O_RDONLY);
            arrayFD[i - optind - 1] = fdin;
        }
        if(posicionFicheroMalo!=i){
            int fdin = open(argv[i], O_RDONLY);
            arrayFD[i - optind] = fdin;
        }else{
            activador++;
        }
    }

    /* Guardo en un buffer, llamado cjtoBuffer(conjunto de buffers leidos) los buffer de lectura de cada fichero */
    char *cjtoBuffer[numFicheros];
    for (int i = 0; i < numFicheros; i++)
    {
        if ((buf = (char *)malloc(buf_size * sizeof(char))) == NULL)
        {
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
        cjtoBuffer[i] = buf;
    }

    int i = 0;
    /* La variable fin nos servirá de contador de ficheros finalizados */
    int fin = 0;
    /* La variable contador tiene una función similar, pero para romper el bucle, cuando no se lea más de ningún fichero */
    int contador = 0;

    int porEscribir[numFicheros];
    
    int ficherosTerminados[numFicheros];

    ssize_t num_read, num_written;

    /* Reservamos buffer de salida y uno auxiliar */
    char *bufSalida = NULL;
    if ((bufSalida = (char *)malloc(buf_size * sizeof(char))) == NULL)
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

    /* Trataremos las lecturas parciales */
    int punterowrite = 0;
    while (contador != numFicheros)
    {
        contador = 0;
        for (i = 0; i < numFicheros; i++)
        {
            int leidos = 0;
            int a_Leer = buf_size;
            while ((a_Leer > 0 && (num_read = read(arrayFD[i], cjtoBuffer[i] + leidos, a_Leer)) == -1))
            {
                if (num_read == 0)
                {
                    contador++;
                    ficherosTerminados[i] = 1;
                }
                a_Leer = a_Leer - num_read;
                leidos = leidos + num_read;
            }
            porEscribir[i] = num_read;
            if (porEscribir[i] == -1)
            {
                porEscribir[i] = 1;
            }
        }
        int a_Escribir=0;
        int escritos=0;
        /* Entraremos aquí cuando se hayan leidos todos los ficheros */
        /* Barajamos los bytes */
        /* Trataremos nuevamente las escrituras parciales */
        for (int v = 0; v < buf_size; v++)
        {
            for (int i = 0; i < numFicheros; i++)
            {
                a_Escribir = punterowrite;
                escritos = 0;
                if (punterowrite == buf_size)
                {
                    while ((a_Escribir > 0 && (num_written = write(fdout, bufSalida + escritos, a_Escribir)) == -1))
                    {
                        a_Escribir = a_Escribir - num_written;
                        escritos = escritos + num_written;
                    }
                    punterowrite = 0;
                } 
                if (porEscribir[i] == buf_size && ficherosTerminados[i] != 1)
                { //Si hemos leído todos los caracteres del buffer
                    bufAux = cjtoBuffer[i];
                    bufSalida[punterowrite] = bufAux[v];
                    punterowrite++;
                    if (porEscribir[i] == 0)
                    {
                        contador++;
                        ficherosTerminados[i] = 1;
                    }
                }
                else
                { //Aquí entramos cuando no se lee un buffer entero y entra algo de basura

                    if (porEscribir[i] <= buf_size && ficherosTerminados[i] != 1)
                    {

                        if (porEscribir[i] == v)
                        {
                            porEscribir[i] = -1;
                            ficherosTerminados[i] = 1;
                            contador++;
                            i--;
                        }
                        else
                        {
                            bufAux = cjtoBuffer[i];
                            bufSalida[punterowrite] = bufAux[v];
                            punterowrite++;
                        }
                    }
                }
            }
        }
        fin = 0;
        for (int i = 0; i < numFicheros; i++)
        {
            if (ficherosTerminados[i] == 1)
            {
                fin++;
            }
        }
        if (punterowrite >= 0 && fin == numFicheros)
        {
            while ((a_Escribir > 0 && (num_written = write(fdout, bufSalida + escritos, a_Escribir)) == -1))
            {
                a_Escribir = a_Escribir - num_written;
                escritos = escritos + num_written;
            }
            break;
        }
    }

    for (int i = 0; i < numFicheros; i++)
    {
        free(cjtoBuffer[i]);
        if (close(arrayFD[i]) == -1)
        {
            perror("close(fdin)");
            exit(EXIT_FAILURE);
        }
    }
}