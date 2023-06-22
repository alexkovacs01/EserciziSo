#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc,char *argv[]) {

    if(argc != 2) {
        perror("ESECUZIONE SCORRETTA DEL GIOCO\n");
        exit(-1);
    }

    int N = atoi(argv[1]);

    int i; 

    /* creazione delle due pipe per comunicazione tra processi */

    int pipefd1[2]; // dal figlio al padre
    int pipefd2[2]; // dal padre al figlio 

    if (pipe(pipefd1) == -1) {
        perror("Errore nella creazione della prima pipe\n");
        exit(-1);
    }

    if (pipe(pipefd2) == -1) {
        perror("Errore nella creazione della seconda pipe\n");
        exit(-1);
    }

    pid_t pid; 

    pid = fork();

    if (pid == -1) {
        perror("Errore nella fork()\n");
        exit(-1);
    }
    else if (pid == 0) {

        /*figlio*/

        ssize_t byte_letti,byte_scritti; 

        char buffer[5];
        
        if (close(pipefd2[1]) == -1 || close(pipefd1[0])) {
            perror("Errore\n");
            exit(-1);
        }

        for(i = 0; i < N; i++) {

            // leggo ping dal processo padre

            byte_letti = read(pipefd2[0],buffer,sizeof(buffer));

            if (byte_letti == -1) {
                perror("Errore nella lettura della pipe\n");
                exit(-1);
            }

            buffer[byte_letti] = '\0';

            // stampo il risultato della lettura: 
            printf("%i-%s\n",i+1,buffer);


            // scrivo pong al padre
            byte_scritti = write(pipefd1[1],"pong",sizeof("pong"));

            if (byte_scritti == -1) {
                perror("Errore nella scrittura della pipe\n");
                exit(-1);
            }

        }

        // chiudo ciò che ho lasciato aperto e termino

        if (close(pipefd2[0]) == -1 || close(pipefd1[1])) {
            perror("Errore\n");
            exit(-1);
        }

        _exit(0);

    }
    else {
        /*padre*/

        ssize_t byte_letti,byte_scritti; 

        char buffer[5];
        
        if (close(pipefd1[1]) == -1 || close(pipefd2[0])) {
            perror("Errore\n");
            exit(-1);
        }

        for(i = 0; i < N; i++) {

            // scrivo ping al figlio
            byte_scritti = write(pipefd2[1],"ping",sizeof("ping"));

            if (byte_scritti == -1) {
                perror("Errore nella scrittura della pipe\n");
                exit(-1);
            }

            // leggo pong dal processo figlio

            byte_letti = read(pipefd1[0],buffer,sizeof(buffer));

            if (byte_letti == -1) {
                perror("Errore nella lettura della pipe\n");
                exit(-1);
            }

            buffer[byte_letti] = '\0';

            // stampo il risultato della lettura: 
            printf("%i-%s\n",i+1,buffer);


        }

        if (close(pipefd1[0]) == -1 || close(pipefd2[1])) {
            perror("Errore\n");
            exit(-1);
        }

        _exit(0);
       
        

    }






}

















