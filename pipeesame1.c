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

    // padre e figlio connessi da una pipe, figlio fa ls -alR e padre scrive risultato su file

    int status;

    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("Errore nella creazione della pipe\n");
        exit(-1);
    }

    pid_t pid; 

    pid = fork();

    if (pid == -1) {
        perror("asdfas");
        exit(-1);
    }
    else if (pid == 0) {

        /*figlio esegue comando */

        if (close(pipefd[0]) == -1) {
            perror("askndmakl");
            exit(-1);
        }

        dup2(pipefd[1],1);

        if (close(pipefd[1]) == -1) {
            perror("snkdjasn");
            exit(-1);
        }

        execl("/bin/ls", "ls", "-alR", NULL);

        perror("Exec failed\n");
        exit(-1);

    }


    // aspetto la terminazione del figlio 

    int wait_pid = wait(&status);

    if (wait_pid == -1) {
        perror("asnbdkjasbnd");
        exit(-1);
    }



    /* padre prende il contenuto e lo scrive su file */

    if (close(pipefd[1]) == -1) {
        perror("askldnma");
        exit(-1);
    }

    ssize_t byte_letti, byte_scritti;

    char buffer[DIM];

    // creo il file
    int fd = open("testone.txt", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRWXU | S_IRWXG);

    if (fd == -1) {
        perror("errore file descriptor\n");
        exit(-1);
    }

    // leggo dalla pipe
    byte_letti = read(pipefd[0],buffer,sizeof(buffer));

    buffer[byte_letti] = '\0';

    byte_scritti = write(fd,buffer,byte_letti);

    if (close(pipefd[0]) == -1) {
        perror("asijhdajsnhd");
        exit(-1);
    }

    



}




