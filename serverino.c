#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

void handlerAllarm(int sig);
int flag = 0; 


int main(void) {


    /* creo la fifo */

    //int mkfifo(const char *pathname, mode_t mode);

    char FIFOpath[] = "/tmp/myfifo";

    if (mkfifo(FIFOpath, S_IRUSR | S_IWUSR) == -1) {
        perror("Errore nella creazione della fifo\n");
        exit(-1);
    }

    // adesso la mia fifo esiste 

    printf("FIFO %s created\n", FIFOpath);


    // apro la fifo solo in lettura, il client la aprira solo in scrittura

    printf("<Server> waiting...\n");

    int fd = open(FIFOpath, O_RDONLY);
    
    if (fd == -1) {
        perror("Errore nel file descriptor\n");
        exit(-1);
    }

    // leggo il contenuto della fifo 

    int v[2] = {0,0};

    ssize_t byte_letti; 

    //byte_letti = read(fd, &v[1],sizeof(v));
    //byte_letti = read(fd, &v[0], sizeof(v));

    if (signal(SIGALRM,handlerAllarm) == SIG_ERR) {
        perror("Errore nella reimpostazione del segnale\n");
        exit(-1);
    }

    alarm(5);

    do {

        alarm(5);

        byte_letti = read(fd, &v[0], sizeof(v));

        if (byte_letti == -1) {
            perror("Ci sono problemi con la fifo\n");
            exit(-1);
        }
        else if (byte_letti == 0 || byte_letti != sizeof(v)) {
            
            printf("Sembra che la pipe sia vuota\n");
            //perror("Sembra che la fifo sia vuota\n");
            //exit(-1);
        }
        else {

            // la fifo non è vuota e posso soddisfare la richiesta dell'es

            if (v[0] < v[1]) {
                printf("%i è più piccolo di %i\n", v[0],v[1]);
            }    
            else if (v[0] > v[1])
                printf("%i è maggiore di %i\n", v[0],v[1]);
            else {
                printf("%i è = a %i\n", v[0],v[1]);
                break;
            }
            
        }

        //alarm(0);
    } while(flag != 1);


    // a questo punto il server rimuove la fifo

    close(fd);

    printf("<Server> is deleting fifo\n");
    if (remove(FIFOpath) == -1) {
        perror("Errore nella rimozione della fifo\n");
        exit(-1);
    }
}


void handlerAllarm(int sig) {
    printf("<Server> tempo scaduto\n");
    flag = 1; 
}

