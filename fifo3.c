#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
  char buffer[1024];

  mkfifo("ciao", 0777);

  int fd = open("ciao", O_RDONLY);

  int bytes_read = read(fd, buffer, sizeof(buffer));

  while (bytes_read > 0) {
    write(STDOUT_FILENO, buffer, bytes_read);
    bytes_read = read(fd, buffer, sizeof(buffer));
  }

  remove("ciao");

  return 0;
}