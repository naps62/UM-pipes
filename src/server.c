#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 32
#define PIPE_NAME "/tmp/patrcia"

int main(int argc, char **argv) {
  mkfifo(PIPE_NAME, 0666);
  char buf[SIZE];

  int fd = open(PIPE_NAME, O_RDONLY);

  while (1) {
    read(fd, buf, SIZE);
    printf("Received: %s\n", buf);
  }
}
