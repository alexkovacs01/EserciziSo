#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>


int main(void) {

    int v[2];

    printf("Inserisci 2 interi:\n");
    scanf("%i", &v[0]);
    scanf("%i", &v[1]);

    // li scrivo sulla fifo 
    ssize_t byte_scritti1,byte_scritti2; 

    // accedo alla fifo 
    int fd = open("/tmp/myfifo", O_WRONLY);

    if (fd == -1) {
        perror("Errore nel apertura della fifo\n");
        exit(-1);
    }

    if (write(fd,&v[0],sizeof(v)) != sizeof(v)) {
        perror("Errore nella scrittura\n");
        exit(-1);
    }

    if(close(fd) == -1) {
        perror("errore nella chiusura della fifo\n");
        exit(-1);
    }

    exit(0);

}


