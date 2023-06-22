#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

/*figlio fa cat di file.txt e il padre fa la grep che prende da stdin collegare i due da pipe */

int main() {

    int status;

    pid_t pid; 

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    // a questo punto ho la pipe 

    pid = fork();

    if (pid == -1) {
        perror("Errore fork()\n");
        exit(-1);
    }
    else if (pid == 0) {
        /* codice del figlio */

        if(close(pipefd[0]) == -1) {
            perror("Errore nella chiusura in lettura della pipe\n");
            exit(-1);
        }

        dup2(pipefd[1],1);  // sposto lo stdout sulla scrittura della pipe

        if (close(pipefd[1]) == -1) {
            perror("Errore nella chiusura della pipe\n");
            exit(-1);
        }

        // a questo punto eseguo il comando del figlio 
        execl("/bin/cat", "cat", "file.txt", NULL);

        perror("EXEC FAILED\n");
        exit(-1);
    }
    else {
        /* codice del padre */

        // aspetto la terminazione del figlio 

        int wait_pid = wait(&status);

        if (wait_pid == -1) {
            perror("Errore nella terminazione del figlio\n");
            exit(-1);
        }

        printf("Figlio con pid %i terminato\n", wait_pid);

        // a questo punto il padre deve leggere il contenuto della pipe e metterlo come stdin per fare grep 0

        if(close(pipefd[1]) == -1){
            perror("Errore nella chiusura del lato scrittura\n");
            exit(-1);
        }

        dup2(pipefd[0],0);

        if(close(pipefd[0]) == -1) {
            perror("Errore nella chiusura in lettura della pipe\n");
            exit(-1);
        }

        // eseguo il comando grep

        execl("/bin/grep", "grep", "0", NULL);  // stampa in ordine tutte le file che contengono alemno uno 0/quel carattere nel file

        perror("EXEC FAILED\n");
        exit(-1);

    }





    return 0;
}

