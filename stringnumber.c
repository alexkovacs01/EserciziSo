#include <stdlib.h>
#include <stdio.h>

int main() {

    int number = 42; 

    char buffer[10];

    sprintf(buffer, "%i",number);

    printf("ecco il tuo numero: %s\n", buffer);

    write(1,buffer,2);
    
    return 0;
}