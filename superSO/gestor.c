/** 
 * File:   main.c
 * Author: naps62, Rocket
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include <bits/waitflags.h>
#include <errno.h>

#include "defs.h"
#include "taskManager.h"

void term_gestor(int sig);
char *readCommand(int pipeIn, int len);
int createPipe();
void deletePipe();
void childHandler(int sig);

TaskManager tm;

/*
 * 
 */
int main(int argc, char** argv) {
	int readCount = 1; /** number of bytes read from FIFO */
	int pid; /** finished child id */
	int cmdLen; /** length of command read from FIFO */
	char *cmdStr; /** full command read from FIFO */
	int pipeIn; /** file descriptor for FIFO read */
	int pipeOut;

	signal(SIGINT, term_gestor);
	signal(SIGCHLD, childHandler);

	/** create named pipe */
	pipeIn = createPipe();
	pipeOut = open(PIPE_NAME, O_WRONLY);

	/** create process struct */
	tm = tm_create();

	while(1) {
		tm_updateExec(tm);
		
		pid = 0;
		_DEBUG("Waiting for pipe read\n");
		readCount = read(pipeIn, &cmdLen, sizeof (int));
		_DEBUG("\nBytes read from FIFO: %d\n", readCount);
		
		if (readCount > 0) { /** new submission was inserted */
			_DEBUG("Submited command length: %d\n", cmdLen)
			if ((cmdStr = readCommand(pipeIn, cmdLen)) == NULL) {
				printf("Erro a ler comando submetido");
			} else {
				_DEBUG("Submited command: %s\n", cmdStr);
				tm_insertSubmission(tm, cmdStr);
				free(cmdStr);
				pid = 1;
			}
		}
		
		if (readCount == 0) { /** EOF, restarts pipe */
			_DEBUG("Restarting pipe\n");
			unlink(PIPE_NAME);
			pipeIn = createPipe();
			_DEBUG("pipe restarted\n");
		}
	}

	return 0;
}

void term_gestor(int sig) {
	deletePipe();
	tm_deleteAll(tm);
	_DEBUG("Terminating...\n");
	exit(0);
}

char* readCommand(int pipeIn, int len) {
	char *cmd = (char *) malloc(sizeof (char) * (len + 1));
	if (read(pipeIn, cmd, len) != len) {
		free(cmd);
		return NULL;
	}
	cmd[len] = '\0';
	return cmd;
}

int createPipe() {
	int pipe;
	if (mkfifo(PIPE_NAME, 0644) == -1) {
		perror("Erro na criação do pipe\n");
		exit(-1);
	}
	_DEBUG("Pipe created. opening link\n");
	/** open pipe */
	if ((pipe = open(PIPE_NAME, O_RDONLY|O_ASYNC)) == -1) {
		printf("Erro na abertura do pipe\n");
		exit(-1);
	}
	_DEBUG("Pipe opened\n");


	return pipe;
}

void deletePipe() {
	_DEBUG("Forking to delete pipe\n");
	if (fork() == 0) {
		execlp("rm", "rm", "-f", PIPE_NAME, NULL);
		_DEBUG("Error on forking to delete FIFO\n")
		exit(-1);
	}
	wait(NULL);
	_DEBUG("Pipe deleted\n");
}

void childHandler(int sig) {
	int status;
	int pid = waitpid(WAIT_ANY, &status, WNOHANG | WUNTRACED);
	if (pid > 0) {
		finishedProcess(tm, pid);
		tm_updateExec(tm);
	}
}
