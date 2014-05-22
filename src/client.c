#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 32
#define PIPE_NAME "/tmp/patrcia"

int main(int argc, char **argv) {
  int fd = open(PIPE_NAME, O_WRONLY);
  char buf[SIZE];

  while (1) {
    printf("Message: ");
    gets(buf);
    write(fd, buf, SIZE);
  }
}
