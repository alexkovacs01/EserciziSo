#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(void) {

    pid_t pid;
    int stauts; 


    pid = fork(); 

    if (pid == -1) {
        perror("Errore nella fork\n");
        exit(-1);
    }
    else if(pid == 0) {
        /* sono il figlio e faccio exec del random number */

        execl("randomizzatore", "./randomizzatore", NULL);

        perror("EXEC FAILED\n");
        exit(-1);

    }
    else {

        int wait_pid = wait(&stauts);

        int ris = WEXITSTATUS(stauts);
        
        if (wait_pid == -1) {
            perror("Errore nella terminazione del figlio\n");
        }

        printf("IL figlio con pid %i è terminato\n", wait_pid);

        printf("Il numero generato random è: %i\n", ris);
        
    }


    // a questo punto faccio 10 figlio che fanno questa cosa
















    return 0; 
}