#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  int fd = open("ciao", O_WRONLY);
  dup2(fd, STDOUT_FILENO);

  execlp("ls", "ls", "-l", "-a", NULL);
  perror("Exec");

  return 0;
}