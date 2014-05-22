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

  Arvore *contadores;

  while (1) {
    // 1. ler o comando
    read(fd, buf, SIZE);

    // 2. separar a string

    if ("INCREMENTA") {
      incrementa(["Braga", "Amares"], 10);
      gravar_as_cenas_em_ficheiro();
    }
    else if ("AGREGAR") {
      if fork() {
        agrega(["Braga", "Amares"], 0, "/tmp/cenas");
      }
    }


    printf("Received: %s\n", buf);
  }
}
