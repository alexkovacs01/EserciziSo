#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DIM 1024

int main() {
    // fare un programma che fa ls | sort 

    int pipefd[2];
    int status; 

    if(pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    pid_t pid; 

    pid = fork();

    if (pid == -1) {
        perror("Errore nella fork()\n");
        exit(-1);
    }
    else if (pid == 0) {

        /* figlio che comunica col padre tramite pipe */

        // il figlio fa questo comando e poi scrive il risutlato su pipe
        // il padre farà sort del risultato 

        // chiudo la lettura per la scrittura
        if(close(pipefd[0]) == -1) {
            perror("Errore nella chiusure della lettura\n");
            exit(-1);
        }

        // scrivo lo stdout su pipe 

        // la dup2 fa anche la close in automatico 
        dup2(pipefd[1],1);

        if (close(pipefd[1]) == -1) {
            perror("Errore nalla chiusura della scrittura\n");
            exit(-1);
        }

        execl("/bin/ls", "ls", "-al", NULL);

        perror("Errore execl\n");
        exit(-1);

    }
    else {

        // chiudo la scrittura 
        if(close(pipefd[1]) == -1) {
            perror("Errore nella chiusura della scirttura\n");
            exit(-1);
        }

        int wait_pid = wait(&status);

        if (wait_pid == -1) {
            perror("Erroe nella temrinazione di un figlio\n");
            exit(-1);
        }

        // su pipe adesso c'è lo stdout, ho bisogno di prenderlo in stdin qui e stamparlo a video

        // devo fare anche l'altro comando e poi stampare a video lo stdin

        dup2(pipefd[0],0); // reindirizzo il contenuto della pipe in stdin 

        if(close(pipefd[0]) == -1) {
            perror("Errore nella chiusura della lettura\n");
            exit(-1);
        }


        execl("/bin/sort", "sort", "-r", NULL); 

        perror("Errore exec padre\n");
        exit(-1);

        /*

        ssize_t byte_letti,byte_scritti_video = -1; 

        char buffer[DIM];

        byte_letti = read(pipefd[0],buffer,sizeof(buffer));

        if (byte_letti == -1) {
            perror("Errore nella lettura della pipe\n");
            exit(-1);
        }
        else if (byte_letti > 0) {

            buffer[byte_letti] = '\0';  // aggiungo il carattere terminatore per non sforare

            byte_scritti_video = write(1,buffer,byte_letti); // scrivo solo il numero di byte_letti

            if (byte_scritti_video != byte_letti) {
                perror("Errore nella scrittura dei caratteri\n"); 
                exit(-1);
            }

        }
        */

    }


    return 0; 

}

