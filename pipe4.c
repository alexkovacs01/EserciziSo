#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {

    int i;

    int pipefd[2];
    int pipefd2[2];

    /*creo la pipe*/

    if (pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    if (pipe(pipefd2) == -1) {
        perror("Errore nella creazione della seconda pipe\n");
        exit(-1);
    }

    // programma che concatena questi 3 comandi ls -al | cat -b27- | sort -n

    pid_t pid;

    int status; 

    for (i = 0; i < 2; i++) {

        pid = fork(); 

        if (pid == -1) {
            perror("Errore nella fork()\n");
            exit(-1);
        }
        else if (pid == 0) {

            /* codice del figlio */

            // chiudo la lettura

            if (i == 0) { // First soon: chiude e scrive sulla prima pipe

                if(close(pipefd[0]) == -1) {
                    perror("Errore nella chiusura della lettura -><-\n");
                    exit(-1);
                }

                dup2(pipefd[1],1);

                if(close(pipefd[1]) == -1) {
                    perror("Errore nella chiusura della scrittura !! \n");
                    exit(-1);
                }

                execl("/bin/ls", "ls", "-al", NULL);
            }
            else if (i == 1){ // Secondo soon 
               
                if(close(pipefd[1]) == -1) {
                    perror("Errore nella chiusura della scrittura --\n");
                    exit(-1);
                }

                dup2(pipefd[0],0); // prendo in input ciò che c'è sulla prima pipe 
               
                if(close(pipefd[0]) == -1) {
                    perror("Errore nella chiusura della lettura ->\n");
                    exit(-1);
                }
               
                if(close(pipefd2[0]) == -1) {
                    perror("Errore nella chiusura della lettura qui\n");
                    exit(-1);
                }                
            
                dup2(pipefd2[1],1);  // scirvo ciò che ho letto sulla seconda pipe
               
                if(close(pipefd2[1]) == -1) {
                    perror("Errore nella chiusura della scrittura anche qui\n");
                    exit(-1);
                }
                
                execlp("cut", "cut", "-b27-", NULL);    // risulato che verrà scritto sulla seconda fifo
            }

            perror("ERRORE EXEC\n");
            exit(-1);
        }
    }


    // chiudo la prima pipe

    if(close(pipefd[1]) == -1) {
        perror("Errore nella chiusura della scrittura --\n");
        exit(-1);
    }

    if(close(pipefd[0]) == -1) {
        perror("Errore nella chiusura della lettura ->\n");
        exit(-1);
    }

    /* sono il padre ed attendo la terminazione dei miei due figli per poi fare l'ultimo comando */
    int wait_pid;

    for(i = 0; i < 2; i++) {

        wait_pid = wait(&status); 
    
        if (wait_pid == -1) {
            perror("Errore nella terminazione dei figli\n");
            exit(-1);
        }

        printf("%i pid terminated\n", wait_pid);
    
    }

    // chiudo la scrittura per la lettura
    if (close(pipefd2[1]) == -1){
        perror("ERRORE NELLA CHIUSURA DELLA SCRITTURA\n");
        exit(-1);
    }
    
    // a questo punto chiudo lo stdin e scrivo contenuto della pipe 

    dup2(pipefd2[0],0);

    if(close(pipefd2[0]) == -1) {
        perror("Errore nella chiusura della lettura\n");
        exit(-1);
    }

    execl("/bin/sort", "sort", "-r", NULL);

    perror("ERRORE EXEC PADRE\n");
    exit(-1);

}