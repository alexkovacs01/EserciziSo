#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

struct vettore1 {
    int v[10];
};

struct vettore2{
    int v[10];
};

typedef struct vettore1 vec1; 
typedef struct vettore2 vec2; 

int main(int argc, char *argv[]) {

    if (argc != 3) {
        perror("UTILIZZO SCORRETTO DELLA STRINGA DI RIFERIMENTO\n");
        exit(-1); 
    }

    key_t k1 = atoi(argv[1]); 
    key_t k2 = atoi(argv[2]); 

    int shm_id_1,shm_id_2; 

    int *shm_pointer_1,*shm_pointer_2;

    shm_id_1 = shmget(k1,0,0);

    shm_id_2 = shmget(k2,0,0);


    if (shm_id_1 == -1 || shm_id_2 == -1) {
        perror("Errore <client> nella creazione del id\n");
        exit(-1);
    }

    shm_pointer_1 = shmat(shm_id_1,NULL,0);

    shm_pointer_2 = shmat(shm_id_2,NULL,0);

    if(shm_pointer_1 == (void *)-1 || shm_pointer_2 == (void *)-1) {
        perror("Errore nell'attach della memoria condivisa\n");
        exit(-1);
    }

    // adesso devo semplicemente eseguire la somma e stamparla a video 

    int somma = 0; 


    for(int i = 0; i < 10; i++) {
        somma += *(shm_pointer_1+i) + *(shm_pointer_2+i);
    }

    printf("<Client> la somma TOTALE è: %i\n",somma);


    // mi stacco dalla memoria prima di terminare

    if(shmdt(shm_pointer_1) == -1) {
        perror("Errore nella prima detach\n");    
        exit(-1);
    }

    if(shmdt(shm_pointer_2) == -1) {
        perror("Errore nella seconda detach\n");    
        exit(-1);
    }

    exit(0);
}




