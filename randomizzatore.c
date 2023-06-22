#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "errno.h"

int main(int argc, char *argv[]) {

    if (argc > 1) {
        perror("Esecuzione scorretta del programma\n");
        exit(-1);
    }
    
    srand(time(NULL));


    int rndnum = rand()%10+1;

    printf("<Figlio> ecco il numero random che ho generato: [%i]\n", rndnum);

    return rndnum;

}