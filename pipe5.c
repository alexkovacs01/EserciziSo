#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {

    // programma che concatena questi 3 comandi ls -al | cat -b27- | sort -n


    // ho tante pipe quanti sono i |

    int i; 

    int status; 

    /* creazione delle pie */
    int pipefd1[2];
    int pipefd2[2];

    if (pipe(pipefd1) == -1) {
        perror("Errore nella creazione della 1 pipe\n");
        exit(-1);
    }

    if (pipe(pipefd2) == -1) {
        perror("ERrore nella creazione della 2 pipe\n");
        exit(-1);
    }

    pid_t pid; 

    for(i = 0; i < 2; i++)  {

        pid = fork();

        if (pid == -1) {
            perror("Errore nella fork()\n");
            exit(-1);
        }
        else if (pid == 0) {

            if (i == 0) {
                /* primo figlio */
            
                // esegue la prima cosa

                // prima di fare una cosa del genre chiudo la scrittura 

                if (close(pipefd1[0]) == -1) {
                    perror("Errore nella chiusura della lettura 1\n");
                    exit(-1);
                }

                dup2(pipefd1[1],1);     // dup mi duplica il fd quindi dovrò chiudere un'altra volta la scrittura

                // succesivamente chiudo anche la scrittura

                if (close(pipefd1[1]) == -1) {
                    perror("Errore nella chiusura della scrittura 1\n");
                    exit(-1);
                }

                execl("/bin/ls", "ls", "-la", NULL);

                perror("EXEC FAILED 1\n");
                exit(-1);

            }
            else {
                /* secondo figlio */

                // leggo il risultato prodotto dall'azione precedente come input
                // chiudo la scrittura e ne approffitto 

                if(close(pipefd1[1]) == -1) {
                    perror("Errore nella chiusura della scrittura 2\n");
                    exit(-1);
                }

                // eseguo dup
                dup2(pipefd1[0],0);

                // una volta fatto ciò chiudo la lettura ricordandomi che è stata duplicata quindi dovro chiuderla 2 volte

                if (close(pipefd1[0]) == -1) {
                    perror("Errore nella chiusura della lettura 1.1\n");
                    exit(-1);
                }

                // a questo punto posso scrivere sulla seconda pipe

                //chiudo la lettura della seconda pipe 
                
                if (close(pipefd2[0]) == -1) {
                    perror("Errore nella chiusura della lettura 1 [2]\n");
                    exit(-1);
                }

                dup2(pipefd2[1],1);

                if(close(pipefd2[1]) == -1) {
                    perror("Errore nella chiusura della scrittura 1 [2]\n");
                    exit(-1);
                }


                // eseguo il comando che andrà sulla pipe 

                execl("/bin/cut", "cut", "-b27-", NULL);

                perror("EXEC FAILED 2\n");
                exit(-1);
            }

        }

    }

    // chiudo ciò che è rimasto aperto per la prima pipe

    if(close(pipefd1[0]) == -1) {
        perror("Errore nella chiusura della lettura 1.2\n");
        exit(-1);
    }

    if(close(pipefd1[1]) == -1) {
        perror("Errore nella chiusura della scrittura 2.2\n");
        exit(-1);
    }


    // a questo punto sono il padre, attendo la terminazione dei due figli ed eseguo il mio comando 
    
    int wait_pid; 

    for(i = 0; i < 2; i++) {
        wait_pid = wait(&status);

        if (wait_pid == -1) {
            perror("Errore nella terminazione dei figli \n");
            exit(-1);
        }

        // qui potrei vedere quali figli hanno terminato

        printf("%i pid temrinato\n", wait_pid);

    }

    // a questo punto i figli hanno sicuramente terminato la loro esecuzione permetendomi cosi di eseguire l'ultimo comando
    
    // chiudo la lettura 

    
    if (close(pipefd2[1]) == -1) {
        perror("Errore nella chiusura della lettura 2 [2]\n");
        exit(-1);
    }

    // prendo come output ciò che era rimasto 
    dup2(pipefd2[0],0);

    if(close(pipefd2[0]) == -1) {   // chiudo la scrittura 
        perror("Errore nella chiusura della scrittura 1 [2]\n");
        exit(-1);
    }

    execl("/bin/sort", "sort", "-n", NULL);

    perror("EXEC FAILED 3\n");
    exit(-1);

}








