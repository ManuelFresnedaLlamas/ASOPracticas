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


void funcionEscritura(char * bufSalida){

}


int main(int argc, char **argv)
{
    int numFicheros;
    int buf_size;
    int opt;
    int fdout=0;
    int aux=0; //variable que usaremos para recoger el char* de -t
    char * memReservar=NULL;
    char * ficheroSalida=NULL;
    char * buf=NULL;
    
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


    
    numFicheros=comprobarNumFicheros(argc,memReservar,ficheroSalida);

    fdout=asignarFicheroSalida(ficheroSalida);


    /*
    *
    *Guardaremos los descriptores de fichero en un array[numFicheros]
    *  
    */
    int arrayFD[numFicheros];
    for(int i=optind;i<argc;i++){
       int fdin = open(argv[i],O_RDONLY);
       arrayFD[i-argc+numFicheros]=fdin;
    }

    /*
    *
    * La idea es guardar en un buffer, los buffer de lectura de cada fichero, asi que
    * reservamos cada buffer de lectura y los añadimos al cjtoBuffer
    * 
    */
    char * cjtoBuffer[numFicheros];
    for(int i=0;i<numFicheros;i++){
        if((buf=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
        }
        cjtoBuffer[i]=buf;
    }

    /*
    *
    * Podemos comprobar que esto funciona con el siguiente bucle
    * 
    */


    char * bufSalidaV=NULL;
    if((bufSalidaV=(char*)malloc(buf_size*sizeof(char)))==NULL){
        perror("malloc()");
        exit(EXIT_FAILURE);
    }
    int num_read=0;
    int var_save=99999999;
    while(1){
        for(int i=0;i<numFicheros;i++){
            if(read(arrayFD[i],cjtoBuffer[i],buf_size)==EOF){
                fprintf(stderr,"Se acabo un fichero\n");
            }
        }

        /*
        *
        *Tenemos ahora cada buffer "relleno" de la cadena, por tanto,
        *Ahora crearemos un buffer de salida, donde escribiremos la baraja de bytes
        * 
        */
        char * bufSalida=NULL;
        if((bufSalida=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
        }

        char * bufAux=NULL;
        if((bufAux=(char*)malloc(buf_size*sizeof(char)))==NULL){
            perror("malloc()");
            exit(EXIT_FAILURE);
        }

        for(int i=0;i<buf_size;i++){
            for(int j=0;j<numFicheros;j++){
                if(var_save==j){
                    strcat(bufSalidaV,"");
                }else{
                    bufAux=cjtoBuffer[j];
                    memcpy(bufSalida,bufAux+i,1);
                }
                if(*bufSalida=='\n'){
                    strcat(bufSalidaV,bufSalida);
                    free(cjtoBuffer[j]);
                    var_save=j;
                }else{
                    strcat(bufSalidaV,bufSalida);
                    *bufSalida='\0';
                }
                // if(contador==buf_size){
                //     funcionEscritura(bufSalidaV);
                // }
            }
        }
    }


    /* Libera memoria dinámica de buffer de lectura */
    free(buf);

    exit(EXIT_SUCCESS);


}