#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "defs.h"

int main(int argc, char **argv) {
	int len;
	_DEBUG("A abrir ficheiro\n");
	int fd = open(PIPE_NAME, O_WRONLY);
	
	if (fd == -1) {
		perror("Erro ao abrir ligação com pipe. O gestor está a correr?\n");
		exit(0);
	}

	if (argc != 2) {
		fputs("Numero de parametros incorrecto\n", stdout);
	} else {
		len = strlen(argv[1]);
		if (write(fd, &len, sizeof(int)) > 0
			&& write(fd, argv[1], strlen(argv[1])) > 0) {
				
			fputs("Comando submetido\n", stdout);
		}
		else {
			perror("Erro a enviar dados ao gestor\n");
		}
	}
	
	return 0;
}
