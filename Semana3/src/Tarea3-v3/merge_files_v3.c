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
    fprintf(stderr, "Uso: merge_files [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ...FILEINn]\n");
}

void imprimirInstrucciones(void){
    
    fprintf(stderr, "NO admite lectura de la entrada estandar.\n");
    fprintf(stderr, "-t BUFSIZE Tamaño de buffer donde 1<= BUFSIZE <=128MB\n");
    fprintf(stderr, "-o FILEOUT Usa FILEOUT en lugar de la salida estandar\n");
        
}

int comprobarSizeBuff(char * memoria){ 
    //TODO
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=128MB=134217728
    */
    
    int aux=atoi(memoria); //guarda la memoria en entero correctamente

    if(aux<1 || aux>134217718){
        fprintf(stderr, "Error: Tamaño de buffer incorrecto.\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }

    return aux;
    

}

int comprobarNumFicheros(int argc, char * memReservar, char * ficheroSalida){
    //Comprobamos que no se superen 16 ficheros.
    int contadorAux=0;
    //si memRe!=NULL -> argc [1] y argc[2] llenos
    //si ficheroSalida tambien != NULL -> argc[3] y argc[4]
    if(memReservar!=NULL && ficheroSalida!=NULL){
        for(int i=5;i<argc;i++){
            contadorAux++;
            if(contadorAux>16){ //si tenemos más de 16 ficheros
                fprintf(stderr,"Se ha reservado ambos\n");
                fprintf(stderr,"Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return contadorAux;
    }
    //si solo uno de ambos tiene NULL -> arg [1] y [2] ocupados
    if((memReservar!=NULL || ficheroSalida!=NULL)&& argc>3){
        for(int i=3;i<argc;i++){
            contadorAux++;
            if(contadorAux>16){ //si tenemos más de 16 ficheros
                fprintf(stderr,"Se ha reservado mem o hay fichout\n");
                fprintf(stderr,"Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }   
        return contadorAux;
    }

    if(memReservar==NULL && ficheroSalida==NULL){
        for(int i=1;i<argc;i++){
            contadorAux++;
            if(contadorAux>16){
                fprintf(stderr,"Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return contadorAux;
    }

    if((memReservar==NULL || ficheroSalida==NULL) && argc<=3){
        fprintf(stderr,"Error: No hay ficheros de entrada\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }
    return 0;
}

int asignarFicheroSalida(char * ficheroSalida){
    int fdout=0;
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
    else{ /* Por defecto, la salida estándar */
        fdout = STDOUT_FILENO;
    }

    return fdout;

}

// void barajarBytes(char *cjtoBuffer, int buf_size, int numFicheros){
//     char *bufAux=NULL;
//     if((bufAux=(char*)malloc(buf_size*sizeof(char)))==NULL){
//         perror("malloc()");
//         exit(EXIT_FAILURE);
//     }

//     char * bufSalida=NULL;
//     if((bufSalida=(char*)malloc(buf_size*sizeof(char)))==NULL){
//         perror("malloc()");
//         exit(EXIT_FAILURE);
//     }

//     for(int i=0;i<numFicheros;i++){
//         bufAux=cjtoBuffer+i;
//         //memcpy(bufSalida,bufAux+i,1);
//     }

// }
void escribirBuffer(int buf_size, char *bufferAE){
    
    char *bufSalida=NULL;
    if((bufSalida=(char*)malloc(buf_size*sizeof(char)))==NULL){
        perror("malloc()");
        exit(EXIT_FAILURE);
    }



}


int main(int argc, char **argv)
{
    int opt;
    int buf_size;
    char *memReservar=NULL;
    char *ficheroSalida=NULL;
    char *buf=NULL;

    if (argc == 1)
    {
        fprintf(stderr, "Error: No hay ficheros de entrada.\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "t:o:")) != -1) //: al final indica que esperan argumento
    {
        //fprintf(stdout,"%d",*optarg);
        switch (opt)
        {
        case 't': //isdigit -> devuelve 0 si no es un caracter numérico
            memReservar=optarg;
            break;
        case 'o':
            ficheroSalida=optarg;
            break;

        default: //Cuidado
            fprintf(stderr,"Error: No hay ficheros de entrada\n");
            imprimirUso();
            imprimirInstrucciones();
            break;            
        }
    }
    if(memReservar!=NULL){
        buf_size=comprobarSizeBuff(memReservar);
    } else{
        buf_size=1024;
    }

    /* Obtenemos el número de ficheros pasados como argumento */
    int numFicheros=comprobarNumFicheros(argc,memReservar,ficheroSalida);

    /* Comprobamos cual es el fichero de salida */

    int fdout=asignarFicheroSalida(ficheroSalida);

    /* Guardamos los FD's en un array del tamaño de numFicheros */
    int arrayFD[numFicheros];
    for(int i=optind;i<argc;i++){
       int fdin = open(argv[i],O_RDONLY);
       arrayFD[i-argc+numFicheros]=fdin;
    }

    /* Guardo en un buffer, los buffer de lectura de cada fichero */
    char * cjtoBuffer[numFicheros];
    for(int i=0;i<numFicheros;i++){
        if((buf=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
        cjtoBuffer[i]=buf;
    }

    int flag=0;
    int porEscribir[numFicheros];
    int i=0;
    ssize_t num_read;
    /* Reservamos buffer de salida y algunos auxiliares */
    char * bufSalida=NULL;
    if((bufSalida=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
    }

    char * bufSalidaAux=NULL;
    if((bufSalidaAux=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
    }

    char * bufAux=NULL;
    if((bufAux=(char*)malloc(buf_size*sizeof(char)))==NULL){
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

    while(flag!=numFicheros){

        for(i=0; i<numFicheros;i++){
            if((num_read=read(arrayFD[i],cjtoBuffer[i],buf_size))==0){
                flag++;
                if(close(arrayFD[i])==-1){
                    perror("close(fdin)");
                    exit(EXIT_FAILURE);
                }
            }
            porEscribir[i]=num_read;
        }

        if(i==numFicheros){ /* Entraremos aquí cuando se hayan leidos todos los ficheros */
            /* Barajamos los bytes */
            //CUIDADO -> NO TRATA ESCRITURAS PARCIALES ESTA CONDICIONAL
            
            //int v=0;
           //
            for(int v=0;v<buf_size;v++){  
                int alertaFin[numFicheros];  
                for(int j=0;j<numFicheros;j++){
                    bufAux=cjtoBuffer[j];
                    memcpy(bufSalidaAux,bufAux+v,1);
                    if(*bufSalidaAux=='\n' && porEscribir[j]!=buf_size){
                        alertaFin[j]++;
                        strcat(bufSalida,bufSalidaAux);
                        //bufAux='\0';
                        cjtoBuffer[j]='\0';
                    }else{
                        strcat(bufSalida,bufSalidaAux);
                    }
                }
                //v++;
                if(strlen(bufSalida)==buf_size){
                    write(fdout,bufSalida,buf_size);
                    *bufSalida='\0';
                    
                }
            //}
            }
        }
            //     /*Si los bytes leidos son 0-> final de fichero*/
            //     /*Tendremos que cerrar fd y vaciar arrayFD[i] y limpiar su buffer*/

            // }else{
            //     /*Dos opciones, lectura parcial o lectura total*/
            //     if(num_read!=buf_size){ /*Si es parcial*/
            //         escribirBuffer(buf_size,cjtoBuffer[i]);
            //     }else{ /* Si es completa */
            //         escribirBuffer(buf_size,cjtoBuffer[i]);
            //     }

    }

}
