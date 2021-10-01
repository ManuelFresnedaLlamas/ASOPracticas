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

    if((memReservar==NULL || ficheroSalida==NULL) && argc<=3){
        fprintf(stderr,"Error: No hay ficheros de entrada\n");
        imprimirUso();
        imprimirInstrucciones();
        exit(EXIT_FAILURE);
    }
}


void leerEscribir(int fdin, int fdout, char *buf, unsigned buf_size)
{
    ssize_t num_read, num_written;

    while ((num_read = read(fdin, buf, buf_size)) > 0)
    {
        if (num_read==buf_size){
        
            num_written = write(fdout, buf, num_read);
            // while((por_escribir>0 &&(num_written=write(fdout,buf+escritos,por_escribir))==-1)){ //nueva
            //     por_escribir -= num_written; //nueva
            //     escritos +=num_written; //nueva
            // }
            break;
            if (num_written == -1)
            {
                perror("write(fdin)");
                exit(EXIT_FAILURE);
            }
            /* Escrituras parciales no tratadas */
            // assert(num_written == num_read);
        }
    }

    if (num_read == -1)
    {
        perror("read(fdin)");
        exit(EXIT_FAILURE);
    }
}


/*necesitare un mientras todos los ficheros no sean EOF, haz:
*   read fichero, tantos bytes como buff_size -> este fichero no se escribira completo
*   escribir el buffer, vaciarlo y leer el siguiente
*   si un fichero = EOF, no hacer nada
*
*/
void reservarMemoriaYEscritura(char * buf, int buf_size, char * ficheroSalida, int argc, char **argv, int fdout){

    /* Reserva memoria dinámica para buffer de lectura */
    if ((buf = (char *)malloc(buf_size * sizeof(char))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }

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

    /* Abre cada fichero de entrada y lo escribe en 'fileout' */
    if (optind < argc)
        for (int i = optind; i < argc; i++)
        {
            int fdin = open(argv[i], O_RDONLY);
            if (fdin == -1)
            {
                perror("open(filein)");
                continue;
            }
            leerEscribir(fdin, fdout, buf, buf_size);
            if (close(fdin) == -1)
            {
                perror("close(fdin)");
                exit(EXIT_FAILURE);
            }
        }
    else
    {
        leerEscribir(STDIN_FILENO, fdout, buf, buf_size);
    }

    if (close(fdout) == -1)
    {
        perror("close(fdout)");
        exit(EXIT_FAILURE);
    }
}





int main(int argc, char **argv)
{
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


    comprobarNumFicheros(argc,memReservar,ficheroSalida);
        /*
        *Una vez comprobado el tamaño del buffer y el número de ficheros, podemos proceder a reservar el buffer de lectura y escritura
        */
    reservarMemoriaYEscritura(buf, buf_size, ficheroSalida, argc, argv, fdout);

    

    /* Libera memoria dinámica de buffer de lectura */
    free(buf);

    exit(EXIT_SUCCESS);


}