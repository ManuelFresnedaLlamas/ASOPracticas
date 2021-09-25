#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

//Si no se especifica -v VISOR -> EXIT FAILURE mostrando USO:...
//
//Si no se encuentra VISOR -> ERROR por stderr
//
//Se crea un proceso hijo para cada imagen
//
// Si no se indica imagen -> ERROR por stderr
//TODOS LOS PROCESOS HIJO DEBEN CREARSE AN>TES DE ESPERAR AL PRIMERO
//PADRE ESPERA QUE TERMINEN TODOS EN ORDEN DE CREAciON
//
//Cada vez que termine un hijo, padre muestra mensaje status
int main(int argc, char **argv)
{

    int opt;
    int pid;
    int status;
    int arrayPID[argc - 3]; //array que contendra los PID de los hijos, el -3 de los 3 primeros args
    char *argVisor = NULL;

    //Comprobamos que se realiza la orden correctamente

    if (argc == 1)
    {
        fprintf(stderr, "Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-v") != 0 && argc <= 2)
    {
        fprintf(stderr, "Uso: ./openimg -v VISOR [IMGs]\n");
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "v:h")) != -1)
    {
        switch (opt)
        {
        case 'v':
            argVisor = optarg; //asi obtenemos el nombre del argumento que acompaña a "-v"
            break;             //funciona
        case 'h':

        default:
            fprintf(stderr, "Uso: ./openimg -v VISOR [IMGs]\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 2; i < argc - 1; i++)
    {
        pid = fork();
        //printf("PID DEVUELTO ES: %d\n", pid);
        arrayPID[i - 2] = pid;
        if (pid == 0)
        {
            execlp(argVisor, argVisor, argv[i + 1], NULL);
            fprintf(stderr, "Error: '%s' no encontrado\n", argVisor);
            exit(EXIT_SUCCESS);
            break; //creamos hijo y salimos de este bucle, por tenerlo mas claro
        }
        else if (pid == -1)
        {
            perror("fork()");
            exit(EXIT_FAILURE);
            break;
        }
    }

    //Ahora implementamos sólo la lógica del padre, puesto que la del hijo la implementamos anteriormente
    if (pid == 0)
    {
    }
    else
    {
        for (int i = 3; i < argc; i++)
        {   // esperamos a que todos los hijos terminen
            if ((waitpid(arrayPID[i - 3], &status, 0)) != 0)
            {
                /*con inkscape y nomacs siempre devuelven 0, puesto q abren una nueva en caso de fallo
                Usando el visor shotwell se ve que funciona esto correctamente*/
                fprintf(stdout, "./openimg: IMG:%s, STATUS: %d,\n", argv[i], WEXITSTATUS(status));
            }
        }
        if (argc == 3)
        {
            fprintf(stderr, "Error: No hay imágenes que visualizar\n");
            exit(EXIT_FAILURE);
        }
    }
}