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

int main(){

    char *bufSalida = NULL;
    if ((bufSalida = (char *)malloc(1 * sizeof(int))) == NULL)
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }


    for(int i=0;i<3;i++){
        bufSalida[i]=i;
        printf("%d",*bufSalida[i]);
    }


    for(int i=0;i<5;i++){
        printf("%d",bufSalida[i]);
    }


}