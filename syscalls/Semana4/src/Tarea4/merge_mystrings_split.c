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

char* comprobarSizeBuff(char *bufsize)
{

    int buf_size = atoi(bufsize);

    if (buf_size < 1 || buf_size > 1048576)
    {
        fprintf(stderr, "Error: El tamaño de buffer debe ser mayor que 0 y menor que 1048576.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return bufsize;
}

char* comprobarSizeCadena(char *cadena)
{
    int aux = atoi(cadena); 

    if (aux < 1 || aux > 255)
    {
        fprintf(stderr, "Error: La longitud mínima de cadena tiene que ser mayor que 0 y menor que 256.\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    return cadena;
}

int main(int argc, char **argv){

    int opt;
    char* buf_size;
    char* min_length;
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
            //fprintf(stderr,"Error: Deben proporcionarse ficheros de entrada con la opción -i\n");
            //fprintf(stderr, "Error: La longitud mínima de cadena tiene que ser mayor que 0 y menor que 256.\n");
            fprintf(stderr,"Debe proporcionarse la lista de ficheros de salida\n");
            imprimirUso();
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (argc<=3){
        fprintf(stderr,"Error: Deben proporcionarse ficheros de entrada con la opción -i\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }


    if (bufsize != NULL)
    {
        buf_size = comprobarSizeBuff(bufsize);
    }
    else
    {
        buf_size = "1024"; //Tamaño por defecto
    }

    if (minlength != NULL)
    {
        min_length = comprobarSizeCadena(minlength);
    }
    else
    {
        min_length = "4"; //Tamaño por defecto
    }


    char *arrayFicherosEntradaAux[16]; //Como máximo, entrarán 16 ficheros, así que reservamos una array de dicho tamaño
    char *ptrToken=NULL; 
    char *saveptr=NULL; 

    int contadorFichEntrada=0; //Cuenta ficheros de entrada
 
    ptrToken = strtok_r(ficherosEntrada, ",", &saveptr); 
    while (ptrToken != NULL) 
    { 
        arrayFicherosEntradaAux[contadorFichEntrada]=ptrToken;
        contadorFichEntrada++;
        //printf("%s\n", ptrToken); 
        ptrToken = strtok_r(NULL, ",", &saveptr); 
    } 
    char *arrayFicherosEntrada[contadorFichEntrada];

    for(int i=0;i<contadorFichEntrada;i++){
        //fprintf(stderr,"%s ",arrayFicherosEntradaAux[i]); //Devuelve f1 f2 f3
        arrayFicherosEntrada[i]=arrayFicherosEntradaAux[i];    
    }


    int contadorFichSalida=0;
    char * ficherosSalida=NULL;
    char *arrayFicherosSalidaAux[16]; //16 ficheros de salida como maximo
    
    for (int i = optind; i < argc; i++){
        contadorFichSalida++;
        //fprintf(stderr,"%s",argv[i]); //funciona, nos imprime los ficheros de salida
        arrayFicherosSalidaAux[i-optind]=argv[i];
    }

    char *arrayFicherosSalida[contadorFichSalida];

    for(int i=0;i<contadorFichSalida;i++){
        arrayFicherosSalida[i]=arrayFicherosSalidaAux[i];
    }

    if(contadorFichSalida==0){
        fprintf(stderr,"Debe porporcionarse la lista de ficheros de salida\n");
        imprimirUso();
        exit(EXIT_FAILURE);
    }

    // char * arrayMergeFiles[3+contadorFichEntrada+1];
    // arrayMergeFiles[0]="./merge_files";
    // arrayMergeFiles[1]="-t ";
    // arrayMergeFiles[2]=bufsize;
    // for(int i=0;i<contadorFichEntrada;i++){
    //     arrayMergeFiles[3+i]=arrayFicherosEntrada[i];
    // }
    // arrayMergeFiles[3+contadorFichEntrada]=NULL;
    
    
    // char * arrayMergeFiles[2+contadorFichEntrada+1];
    // arrayMergeFiles[0]="-t";
    // arrayMergeFiles[1]=bufsize;
    // for(int i=0;i<contadorFichEntrada;i++){
    //     arrayMergeFiles[2+i]=arrayFicherosEntrada[i];
    // }
    // arrayMergeFiles[2+contadorFichEntrada]=NULL;
    
    
    char * arrayMergeFiles[3+contadorFichEntrada+1];
    arrayMergeFiles[0]="./merge_files";
    arrayMergeFiles[1]="-t";
    arrayMergeFiles[2]=buf_size;
    for(int i=0;i<contadorFichEntrada;i++){
        arrayMergeFiles[3+i]=arrayFicherosEntrada[i];
    }
    arrayMergeFiles[3+contadorFichEntrada]=NULL;

/*************************************************************************************/
    
    char * arrayMyStrings[6];
    arrayMyStrings[0]="./mystrings";
    arrayMyStrings[1]="-t";
    arrayMyStrings[2]=buf_size;
    arrayMyStrings[3]=" -n";
    arrayMyStrings[4]=min_length;
    arrayMyStrings[5]=NULL;

/*************************************************************************************/

    char * arraySplitFiles[3+contadorFichSalida+1];
    arraySplitFiles[0]="./split_files";
    arraySplitFiles[1]="-t";
    arraySplitFiles[2]=buf_size;
    for(int i=0;i<contadorFichSalida;i++){
        arraySplitFiles[3+i]=arrayFicherosSalida[i];
    }
    arraySplitFiles[3+contadorFichSalida]=NULL;





    int pipefdsI_C[2]; /* Descriptores de fichero de la tubería */
    int pipefdsC_D[2];
    
    if (pipe(pipefdsI_C) == -1) /* Paso 0: Creación de la tubería */
    {
        perror("pipe()");
        exit(EXIT_FAILURE);
    }

    // if (pipe(pipefdsC_D) == -1) /* Paso 0: Creación de la tubería */
    // {
    //     perror("pipe()");
    //     exit(EXIT_FAILURE);
    // }




//CONCATENAR merge_files -> mystrings -> split_files
    switch (fork())
    {
    case -1:
        perror("fork(1)");
        exit(EXIT_FAILURE);
        break;
    case 0: /* Hijo izquierdo de la tubería */
        /* Paso 2: El extremo de lectura no se usa */
        if (close(pipefdsI_C[0]) == -1)
        {
            perror("close(1)");
            exit(EXIT_FAILURE);
        }
        /* Paso 3: Redirige la salida estándar al extremo de escritura de la tubería */
        if (dup2(pipefdsI_C[1], STDOUT_FILENO) == -1)
        {
            perror("dup2(1)");
            exit(EXIT_FAILURE);
        }
        /* Paso 4: Cierra descriptor duplicado */
        if (close(pipefdsI_C[1]) == -1)
        {
            perror("close(2)");
            exit(EXIT_FAILURE);
        }
        /* Paso 5: Reemplaza el binario actual por el de `youtube-dl` */
        //execlp("./merge_files", "./merge_files", "-t", bufsize, *arrayFicherosEntrada, NULL);
        execvp("./merge_files",arrayMergeFiles);
        perror("execlp(izquierdo)");
        exit(EXIT_FAILURE);
        break;
    default: /* El proceso padre continúa... */
        if (close(pipefdsI_C[1]) == -1)
        {
            perror("close(2)");
            exit(EXIT_FAILURE);
        }

        if (pipe(pipefdsC_D) == -1) /* Paso 0: Creación de la tubería */
        {
            perror("pipe()");
            exit(EXIT_FAILURE);
        }


        switch (fork())
        {
        case -1:
            perror("fork(2)");
            exit(EXIT_FAILURE);
            break;
        case 0: /* Hijo central de la tubería  */
            /* Paso 7: El extremo de escritura se usa */
            if (close(pipefdsC_D[0]) == -1)
            {
                perror("close(3)");
                exit(EXIT_FAILURE);
            }
            /* Paso 8: Redirige la entrada estándar al extremo de lectura de la tubería */
            if (dup2(pipefdsI_C[0], STDIN_FILENO) == -1)
            {
                perror("dup2(2)");
                exit(EXIT_FAILURE);
            }
            if (close(pipefdsI_C[0]) == -1)
            {
                perror("close(4)");
                exit(EXIT_FAILURE);
            }

            if(dup2(pipefdsC_D[1],STDOUT_FILENO) == -1)
            {
                perror("dup2(2)");
                exit(EXIT_FAILURE);
            }
            /* Paso 9: Cierra descriptor duplicado */
            if (close(pipefdsC_D[1]) == -1)
            {
                perror("close(4)");
                exit(EXIT_FAILURE);
            }
            /* Paso 10: Reemplaza el binario actual por el de `ffmpeg` */
            //execlp("./mystrings", "./mystrings", "-t", *bufsize, "-n", *minlength, NULL);
            execvp("./mystrings",arrayMyStrings);
            perror("execvp(central)");
            exit(EXIT_FAILURE);
            break;
        default: /* El proceso padre continúa... */
            switch (fork())
            {
            case -1:
                perror("fork(2)");
                exit(EXIT_FAILURE);
                break;
            case 0: /* Hijo derecho de la tubería  */
                /* Paso 7: El extremo de escritura no se usa */
                if (close(pipefdsC_D[1]) == -1)
                {
                    perror("close(3)");
                    exit(EXIT_FAILURE);
                }
                /* Paso 8: Redirige la entrada estándar al extremo de lectura de la tubería */
                if (dup2(pipefdsC_D[0], STDIN_FILENO) == -1)
                {
                    perror("dup2(2)");
                    exit(EXIT_FAILURE);
                }
                /* Paso 9: Cierra descriptor duplicado */
                if (close(pipefdsC_D[0]) == -1)
                {
                    perror("close(4)");
                    exit(EXIT_FAILURE);
                }
                //execlp("./split_files", "./split_files", "-t", *bufsize, *ficherosSalida,NULL);
                execvp("./split_files", arraySplitFiles);
                perror("execlp(derecho)");
                exit(EXIT_FAILURE);
                break;
            default: /* El proceso padre continúa... */
                if (close(pipefdsI_C[0]) == -1)
                {
                    perror("close(pipefds[0])");
                    exit(EXIT_FAILURE);
                }
                if (close(pipefdsC_D[0]) == -1)
                {
                    perror("close(pipefds[0])");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }





    /* El proceso padre cierra los descriptores de fichero no usados */
    // if (close(pipefdsI_C[0]) == -1)
    // {
    //     perror("close(pipefds[10])");
    //     exit(EXIT_FAILURE);
    // }
    // if (close(pipefdsI_C[1]) == -1)
    // {
    //     perror("close(pipefds[11])");
    //     exit(EXIT_FAILURE);
    // }
    // if (close(pipefdsC_D[0]) == -1)
    // {
    //     perror("close(pipefds[012])");
    //     exit(EXIT_FAILURE);
    // }
    if (close(pipefdsC_D[1]) == -1)
    {
        perror("close(pipefds[13])");
        exit(EXIT_FAILURE);
    }
    // /* El proceso padre espera a que terminen sus procesos hijo */
    if (wait(NULL) == -1)
    {
        perror("wait(1)");
        exit(EXIT_FAILURE);
    }
    if (wait(NULL) == -1)
    {
        perror("wait(2)");
        exit(EXIT_FAILURE);
    }
    if(wait(NULL) == -1){
        perror("wait(3)");
        exit(EXIT_FAILURE);
    }

   
}
   
