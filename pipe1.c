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

    if(argc != 2) {
        perror("Errore nel esecuzione del programma\n");
        exit(-1);
    }
    int buffer[DIM];

    int pipefd[2]; 

    if(pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    // ora che ho la pipe creo figlio e padre
    pid_t pid; 

    pid = fork();

    int status; 
    int wait_pid;

    if (pid == -1) {
        perror("Errore nella fork()\n");
        exit(-1);
    }
    else if (pid == 0) {
        /* codice del produttore */

        // legge il contenuto del file in max chunk di 100 caratteri 
        // ssize_t read(int fd, void *buf, size_t count);

        printf("file.txt : %s\n", argv[1]);

        // apro il file in sola lettura 
        // int open(const char *pathname, int flags);
        int fd = open(argv[1],O_RDONLY); 
        
        if (fd == -1) {
            perror("Errore nel apertura del file\n");
            exit(-1);
        }

        ssize_t byte_letti,byte_scritti = -1; 

        byte_letti = read(fd,buffer,sizeof(buffer));

        if (byte_letti == -1) {
            perror("Errore nella lettura del file \n");
            exit(-1);
        }

        // per scrivere chiudo il canale in lettura 
        if (close(pipefd[0]) == -1) {
            perror("Errore nella chiusura del canale in lettura\n");
            exit(-1);
        }       

        // adesso posso scrivere sulla pipe    

        if (byte_letti > 0) {
            
            // scrivo n byte_letti sulla pipe, se n != byte_letti la write è errata

            byte_scritti = write(pipefd[1], buffer, byte_letti); // non ci va sizeof in quanto magari byte_letti < 100

            if (byte_scritti != byte_letti) {
                perror("Errore nella scrittura su pipe\n");
                exit(-1);
            }

        }

        // bisogna anche chiudere tutto 

        if (close(pipefd[1]) == -1 || close(fd) == -1) {
            perror("Errore nella chiusura del produttore\n");
            exit(-1);
        }

    }
    else {
        /* codice del consumaotre */

        // devo leggere il risultato che c'è all'interno della pipe e stamparlo a video 
        // per leggere chiudo la pipe in scrittura 

        ssize_t byte_letti = -1; 

        char buffer2[DIM+1];

        if(close(pipefd[1]) == -1) {
            perror("Errore nella chiusura della pipe in scrittura\n");
            exit(-1);
        }

        do {
            // eseguo la lettura della fifo

            byte_letti = read(pipefd[0], buffer2, sizeof(buffer2)); 
        
            if (byte_letti == -1) {
                printf("Sembra che la pipe abbia un problema\n");
                exit(-1);
            }
            else if (byte_letti == 0){
                printf("Sembra che la pipe adesso sia vuota\n");
            }
            else {
                buffer2[byte_letti] = '\0';
                printf("<CONSUMATORE>%s\n", buffer2);
            }
    
        } while (byte_letti > 0);   // fallo finchè c'è qualcosa da leggere qui dentro 


        // bisnogna anche chiudere tutto ciò che abbiamo aperto
        if(close(pipefd[0]) == -1) {
            perror("Errore nella chiusura del consumatore\n");
            exit(-1);
        }

    }

    return 0; 

}