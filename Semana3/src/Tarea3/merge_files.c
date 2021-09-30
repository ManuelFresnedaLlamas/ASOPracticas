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




void imprimirUso(void){
    fprintf(stderr, "Uso: merge_files [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ...FILEINn]\n");
}

void imprimirInstrucciones(void){
    
    fprintf(stderr, "NO admite lectura de la entrada estandar.\n");
    fprintf(stderr, "-t BUFSIZE Tamaño de buffer donde 1<= BUFSIZE <=128MB\n");
    fprintf(stderr, "-o FILEOUT Usa FILEOUT en lugar de la salida estandar\n");
        
}

void comprobarSizeBuff(char * memoria){
    //TODO
    /*Comprobar que la memoria dada como argumento es un número entero entre
    *1<=BUFSIZE<=128MB=134217728
    */
                                                            //con * indico primer argumento, puedo usarlo como array
    fprintf(stdout,"La memoria pasada como argumento es: %d\n",(*memoria-'0')); //con '-0' convertimos el char en entero, nos devuelve el primero miembro del puntero
    
    
    int i=0;
    char a='0';
    char array[]={0};
    while(a!='\0'){
        array[i]=memoria[i];
        a=memoria[i];
        i++;
        fprintf(stdout,"Los valores del puntero son: %d",array[i-1]);
    }
    
  //  int aux=atoi(*memoria);

   // fprintf(stdout,"El entero es: %d\n",aux);

}

void comprobarNumFicheros(int argc, char * memReservar, char * ficheroSalida){
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
        return;
    }
    //si solo uno de ambos tiene NULL -> arg [1] y [2] ocupados
    if(memReservar!=NULL || ficheroSalida!=NULL){
        for(int i=3;i<argc;i++){
            contadorAux++;
            if(contadorAux>16){ //si tenemos más de 16 ficheros
                fprintf(stderr,"Se ha reservado mem o hay fichout\n");
                fprintf(stderr,"Error: Demasiados ficheros de entrada. Máximo 16 ficheros.\n");
                imprimirInstrucciones();
                exit(EXIT_FAILURE);
            }
        }
        return;
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
        return;
    }
}

int main(int argc, char **argv)
{
    int opt;
    int fdout;
    int aux=0; //variable que usaremos para recoger el char* de -t
    char * memReservar=NULL;
    char * ficheroSalida=NULL;
    
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

        //default:
            //Se tendra buffer de 1024 bytes y FILEOUT será la salida estándar            
        }
    }

    comprobarSizeBuff(memReservar);

    comprobarNumFicheros(argc,memReservar,ficheroSalida);
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
    else /* Por defecto, la salida estándar */
        fdout = STDOUT_FILENO;



}