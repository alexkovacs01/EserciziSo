#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

#define DIM 100

int main(int argc, char *argv[]) {

    if (argc != 2) {
        perror("Errore nel utilizzo del programma\n");
        exit(-1);
    }

    /* creazione della pipe */    
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    pid_t pid; 

    pid = fork();

    if (pid == -1) {
        perror("Errore fork()\n");
        exit(-1);
    }
    else if (pid == 0) {
        /* produttore */

        // chiudo la pipe in lettura in quanto ci devo scrivere
        if(close(pipefd[0]) == -1) {
            perror("<producer> Errore nella chiusura della pipe in lettura\n");
            exit(-1);
        }
        
        char buffer_per_file[DIM];

        // apro il file

        int fd = open(argv[1],O_RDONLY);

        if (fd == -1) {
            perror("Errore nella creazione del file descriptor\n");
            exit(-1);
        }

        ssize_t byte_letti,byte_scritti = -1; 

        do {

            byte_letti = read(fd,buffer_per_file,sizeof(buffer_per_file));


            if (byte_letti == -1) {
                perror("Errore nella lettura dei byte\n");
                exit(-1);
            }

            // scrivo sulla pipe

            if (byte_letti > 0) {

                byte_scritti = write(pipefd[1], buffer_per_file, byte_letti);

                if (byte_letti != byte_scritti) {
                    perror("Errore nella scrittura su pipe\n");
                    exit(-1);
                }
            }

        } while (byte_letti > 0);   // finhè c'è roba da leggere leggo 

        // a questo punto ho finito quindi chiudo tutto 

        if (close(pipefd[1]) == -1 || close(fd) == -1) {
            perror("<producer> Errore nella chiusura generale delle cose\n");
            exit(-1);
        }

    }
    else {
        /* consumer */

        // chiudo il canale in scrittura

        if (close(pipefd[1]) == -1) {
            perror("<consumer> Errore nella chusura della pipe in scrittura\n");
            exit(-1);
        }

        ssize_t byte_letti = -1; 

        char buffer_pipe[DIM+1];

        do {

            byte_letti = read(pipefd[0], buffer_pipe, sizeof(buffer_pipe));

            if (byte_letti == -1) {
                perror("Errore con la pipe\n");
                exit(-1);
            }
            else if(byte_letti == 0) {
                write(0,"Sembra che la pipe sia vuota\n", sizeof("Sembra che la pipe sia vuota\n"));
            }
            else {
                buffer_pipe[byte_letti] = '\0';     // ci aggiungo il carattere terminatore
                write (0,buffer_pipe,sizeof(buffer_pipe));  // scrivo su stdout il risultato 
            }
        } while (byte_letti > 0);   // leggo finchè c'è


        // prima di concludere ovviamente devo chiudere tutto 

        if (close(pipefd[0]) == -1) {
            perror("<consumer> Errore nella chiusura della lettura della pipe\n");
            exit(-1);
        }
    }

    write(0,"Arrivederci\n",sizeof("Arrivederci\n"));
    return 0; 
}





