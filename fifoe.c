#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

int main() {

    int status;

    char FIFOpath[] = "/tmp/myfifo";

    mkfifo(FIFOpath, 0777);

    pid_t pid; 

    pid = fork();

    if (pid == -1) {
        perror("Errore nella fork()\n");
        exit(-1);
    }
    else if (pid == 0) {
        /* codice figlio */

        int fd;
        
        fd = open(FIFOpath, O_WRONLY);

        if (fd == -1) {
            perror("Errore nell'apertura della fifo!\n");
            exit(-1);
        }

        dup2(fd,1);

        execl("/bin/ls", "ls", "-l","-a", NULL);

        perror("EXEC FAILED\n");
        exit(-1);

    }
    else {
        /* codice padre */

        // non devo attendere la terminazione del figlio!

        char buffer[1024];

        // leggo dalla fifo e stampo a video il risultato prodotto

        ssize_t byte_letti; 

        int fd = open(FIFOpath,O_RDONLY);

        if (fd == -1) {
            perror("Errore nell'apertura della fifo\n");
            exit(-1);
        }

        byte_letti = read(fd,buffer,sizeof(buffer));

        /*
        buffer[byte_letti] = '\0';

        printf("%s\n", buffer);
        */

        while (byte_letti > 0) {
            write(1,buffer,byte_letti);
            byte_letti = read(fd,buffer,sizeof(buffer));
        }
    }

    remove(FIFOpath);

    return 0;
}














