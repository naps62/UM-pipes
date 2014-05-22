/* 
 * File:   taskManager.h
 * Author: naps62
 *
 * Created on May 26, 2010, 10:43 AM
 */

#include "linked_list.h"

#ifndef _TASKMANAGER_H
#define	_TASKMANAGER_H

struct process {
	char cmd[20]; /** falta separar por argumentos antes de executar */
	char **args;
	int pipeIn;
	int pipeOut;
	pid_t pid;
};

struct process_list {
	char input[30];
	char output[30];
	int input_id;
	int output_id;
	pid_t head_pid; /** -1 se nao estiver a executar */
	pid_t tail_pid; /** para comparar quando um filho terminar */
	List processes;
};

struct task_manager {
	List openFiles;
	List processList;
};

typedef struct process *Process;
typedef struct process_list *ProcessList;


typedef struct task_manager *TaskManager;

/** para testes */
void loadTestData(TaskManager m);

/** inicia a estrutura */
TaskManager tm_create();

/** elimina a estrutura */
void tm_deleteAll(TaskManager tm);

/** marca um ficheiro como ainda nao estando pronto a ler */
void tm_insertFile(TaskManager tm, char *fileName);

/** desmarca um ficheiro */
void tm_removeFile(TaskManager tm, char *fileName);

/** verifica se um ficheiro esta por terminar */
int tm_isFileQueued(TaskManager tm, char *fileName);

/** insere uma sequencia de comandos na estrutura */
void tm_insertSubmission(TaskManager tm, char *cmdStr);

/** remove uma lista de comandos, dado o seu indice na estrutura */
void tm_removeProcess(TaskManager tm, int index);

/** determina se ha algum processo que pode ser iniciado, e cria o respectivo processo */
void tm_updateExec(TaskManager tm);

/** dado um processo que terminou, actualiza a lista */
int finishedProcess(TaskManager tm, int pid);

/** divide a string (argv[1] do submeter) e guarda-a na estrutura */
ProcessList stringToProcessList(char* string);

#endif	/* _TASKMANAGER_H */

