#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

// server che crea 2 vettori di 10 int che li alloca in memoria condivisa 
// 2 memoria condivisa con array da 10 numeri 
// il server crea un figlio che fa exec di somma che ha dei parametri
// le due chiavi delle memorie condivise e posizione del elemnto da sommare nel array
// chiavi posizione, attacco alla memoria condivisa e faccio la somma nelle posizioni e printare a schermo 
// seconda parte: fare questa somma per ogni posizione dell'array  

struct vettore1 {
    int v[10];
};

struct vettore2{
    int v[10];
};


typedef struct vettore1 vec1; 
typedef struct vettore2 vec2; 

int main(void) {

// inizializzo i vettori 

int i; 

vec1 vet1; 
vec2 vet2; 


srand(time(NULL));

for(i = 0; i <10 ; i++) {

    // genero valori casuali fino a 10
    vet1.v[i] = rand() %10+1; 
    vet2.v[i] = rand() %10+1; 

}

// verifico che l'inserimento sia andato a buon fine
for(i = 0; i < 10; i++) {
    printf("[%i]-",vet1.v[i]);
    printf("[%i]",vet2.v[i]); 
    printf("\n");
}

// creo il test case per la somma totale
int sum_ris = 0; 
for(i = 0; i < 10; i++) {
    sum_ris += vet1.v[i] + vet2.v[i];
}

// stampo fuori la somma
printf("La somma totale dovrà essere: %i\n", sum_ris);

/*creo le memoria condivisa*/

key_t shm_key_1 = 109; 
key_t shm_key_2 = 209;

int shm_id_1,shm_id_2; 

shm_id_1 = shmget(109, sizeof(vec1), IPC_CREAT | 0666);

shm_id_2 = shmget(209, sizeof(vec2), IPC_CREAT | 0666);

if (shm_id_1 == -1) {
    perror("Errore nella creazione della 1 shm\n");
    exit(-1);
}

if (shm_id_2 == -1) {
    perror("Errore nella creazione della 2 shm\n");
    exit(-1);
}

int *shm_pointer_1, *shm_pointer_2; 

shm_pointer_1 = shmat(shm_id_1,NULL,0);

if (shm_pointer_1 == (void *)-1) {
    perror("Errore nell'attacch della 1 shm\n");
    exit(-1);
}

shm_pointer_2 = shmat(shm_id_2,NULL,0);

if (shm_pointer_2 == (void *)-1) {
    perror("Errore nell'attacch della 1 shm\n");
    exit(-1);
}

// adesso che ho entrambi i puntatori alle memorie con i relativi vettori 


// popolo la memoria condivisa con i dati
// per evitare di fare questa cosa avrei dovuto riempire direttamente qui i valori 
for(i = 0; i < 10; i++) {
    *(shm_pointer_1+i) = vet1.v[i];
    *(shm_pointer_2+i) = vet2.v[i]; 
}

printf("->%i\n",*(shm_pointer_1+3));


pid_t pid; 

for(i = 0; i < 11; i++) {

    pid = fork(); // creo 10 figli che eseguiranno il comando somma 

    if (pid == -1) {
        perror("Errore nella fork\n");
        exit(-1);
    }
    else if (pid == 0) {
        /* codice del figlio */

    
        if (i != 10) {
            // devo dirgli le chiavi della memoria condivisa e la posizione i-esima
            execl("sommatore", "./sommatore", "109", "209", "3", NULL);

            printf("Se sono qui significa che la exec è fallita\n");
            exit(-7);
        }
        else {
            // osno l'ultimo figlio eseguo il client che mi somma tutto 
            execl("sommatoretotale", "./sommatoretotale", "109", "209", NULL);

            printf("Se sono qui significa che la exec è fallita\n");
            exit(-7);
        }
    }
}


// arrivati a questo punto il padre termina chiudendo le memorie aperte 
// aspetto che i figli terminino la loro esecuzione

int status; 
int wait_pid; 

for(i = 0; i < 10; i++) {

    wait_pid = wait(&status);

    if (wait_pid == -1) {
        perror("Problemi con l'attesa di un figlio\n");
        exit(-1);
    }

    //printf("Il processo con pid %i ha terminato\n", wait_pid);
}

// qui hanno finito tutti qundi posso togliere la memoria condivisa 
// forse mi serviranno dei semafori per arrivare qui
// eseguo la detach della memoria condivisa 

if(shmdt(shm_pointer_1) == -1) {
    perror("Errore nel detach della 1 memoria condivisa\n");
    exit(-1);
}

if(shmdt(shm_pointer_2) == -1) {
    perror("Errore nel detach della 2 memoria condivisa\n");
    exit(-1);
}

// elimino la memoria condivisa
if(shmctl(shm_id_1,IPC_RMID,NULL) == -1) {
    perror("Errore nel eliminazione della 1 memoria condivisa\n");
    exit(-1);
}

if(shmctl(shm_id_2,IPC_RMID,NULL) == -1) {
    perror("Errore nel eliminazione della 2 memoria condivisa\n");
    exit(-1);
}



}