#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "defs.h"
#include "linked_list.h"
#include "taskManager.h"

/**
 * list getKey, compare and destructors
 */
void *openFiles_getKey(void *data) {
	return data;
}

int openFiles_compareKeys(void *k1, void *k2) {
	return strcmp(k1, k2);
}

void *processList_getKey(void *data) {
	return &(((ProcessList) data)->tail_pid);
}

int processList_compareKeys(void *k1, void *k2) {
	pid_t p1 = *(pid_t *) k1;
	pid_t p2 = *(pid_t *) k2;
	if (p1 == p2) return 0;
	else if (p1 < p2) return -1;
	else return 1;
}

void* processList_destructor(void *data, void *params) {
	list_deleteAll(((ProcessList) data)->processes, NULL);
	free(data);

	return NULL;
}

void loadTestData(TaskManager tm) {
	_DEBUG("Creating test data\n");
	ProcessList new = malloc(sizeof (struct process_list));
	new->head_pid = -1;
	new->tail_pid = -1;
	strcpy(new->input, "");
	strcpy(new->output, "teste.txt");
	new->processes = list_create(NULL, NULL, LIST_SIMPLE_FREE);

	Process p1 = (Process) malloc(sizeof (struct process));
	strcpy(p1->cmd, "ls");
	p1->args = (char **) malloc(sizeof(char *)*2);
	p1->args[0] = (char *) malloc(sizeof(char) * 20);
	strcpy(p1->args[0], "ls");
	p1->args[1] = NULL;

	Process p2 = (Process) malloc(sizeof(struct process));
	strcpy(p2->cmd, "head");
	p2->args = (char **) malloc(sizeof(char *) * 2);
	p2->args[0] = (char *) malloc(sizeof(char) * 20);
	strcpy(p2->args[0], "head");
	p2->args[2] = NULL;
	list_insertTail(new->processes, p1);
	list_insertTail(new->processes, p2);
	list_insertTail(tm->processList, new);
}

TaskManager tm_create() {
	_DEBUG("Creating task manager struct\n");
	TaskManager tm = (TaskManager) malloc(sizeof (struct task_manager));
	tm->openFiles = list_create(&openFiles_getKey, &openFiles_compareKeys, LIST_SIMPLE_FREE);
	tm->processList = list_create(&processList_getKey, &processList_compareKeys, &processList_destructor);

	return tm;
}

void tm_deleteAll(TaskManager tm) {
	_DEBUG("Disposing manager data\n");
	list_deleteAll(tm->processList, NULL);
	list_deleteAll(tm->openFiles, NULL);
	free(tm);
	_DEBUG("Done\n");
}

/** marca um ficheiro como ainda nao estando pronto a ler */
void tm_insertFile(TaskManager tm, char *fileName) {
	char *s = (char *) malloc(sizeof (char) * (strlen(fileName) + 5));
	strcpy(s, fileName);
	list_insertOrd(tm->openFiles, s);
}

/** desmarca um ficheiro */
void tm_removeFile(TaskManager tm, char *fileName) {
	_DEBUG("Un-queuing file %s\n", fileName);
	list_remove(tm->openFiles, fileName);
}

/** verifica se um ficheiro esta por terminar */
int tm_isFileQueued(TaskManager tm, char *fileName) {
	return list_search(tm->openFiles, fileName) != NULL;
}

void tm_insertSubmission(TaskManager tm, char *cmdStr) {
	ProcessList newProcessList = stringToProcessList(cmdStr);
	list_insertHead(tm->processList, newProcessList);
}

void tm_removeProcess(TaskManager tm, int pid) {
	_DEBUG("Removing process with pid %d\n", pid);
	ProcessList removedProcess = (ProcessList) list_remove(tm->processList, &pid);
	processList_destructor(removedProcess, NULL);
}

int processToArray(void *data, void *params) {
	Process *cmds = (Process *) ((void **) params)[0];
	int *i = (int *) ((void **) params)[1];

	cmds[(*i)++] = data;

	return 1;
}

void execProcess(TaskManager tm, ProcessList pList) {
	_DEBUG("Starting new execution\n");
	int retVal;
	/** marca o ficheiro de saida como aberto */
	_DEBUG("Marking file as queued: %s\n", pList->output);
	tm_insertFile(tm, pList->output);

	/** cria um array com tamanho suficiente para guardar a lista */
	int size = list_size(pList->processes);
	Process cmds[size];

	/** passa a lista para a forma de um array, para trabalhar mais facilmente */
	int i = 0;
	void *params[2];
	params[0] = cmds;
	params[1] = &i;
	list_map(pList->processes, &processToArray, params);



	/** cria os processos, redireccionando o respectivo I/O */
	int tempPipe[2];
	int lastInPipe = -1;
	int currentIn, currentOut;

	for (i = 0; i < size; i++) {
		tempPipe[0] = -1;
		tempPipe[1] = -1;
		currentOut = -1;
		currentIn = -1;
		//_DEBUG("Index %d\n", i);
		if (i != size - 1) {
			retVal = pipe(tempPipe);
			//_DEBUG("Index %d: pipe(%d, %d)\n", i, tempPipe[0], tempPipe[1]);
		}

		if (i == 0) {
			if (strcmp(pList->input, "") == 0) {
				currentIn = -1;
			} else {
				//_DEBUG("Index %d: Opening input file: %s\n", i, pList->input);
				currentIn = open(pList->input, O_RDONLY);
			}
		} else {
			if (tempPipe[0])
				close(tempPipe[0]);

			currentIn = lastInPipe;
		}

		if (i == size - 1) {
			if (strcmp(pList->output, "") == 0) {
				currentOut = -1;
			} else {
				//_DEBUG("Opening output file at index %d: %s\n", i, pList->output);
				currentOut = open(pList->output, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			}
		} else {
			currentOut = tempPipe[1];
			lastInPipe = tempPipe[0];
		}
		
		_DEBUG("Forking new process: %s\n", cmds[i]->cmd);
		//_DEBUG("Input: %d, Output: %d\n", currentIn, currentOut);
		if ((cmds[i]->pid = fork()) == 0) {
			if (currentIn != -1) {
				//_DEBUG("Copying input %d to stdin %d\n", currentIn, STDIN);
				dup2(currentIn, STDIN);
				close(currentIn);
			}
			if (currentOut != -1) {
				//_DEBUG("Copying output %d to stdout %d\n", currentOut, STDOUT);
				dup2(currentOut, STDOUT);
				close(currentOut);
			}

			if (tempPipe[0] != -1)
				close(tempPipe[0]);
			if (tempPipe[1] != -1)
				close(tempPipe[1]);

			execvp(cmds[i]->cmd, cmds[i]->args);
		} else {
			if (currentIn != -1)
				close(currentIn);
			if (currentOut != -1)
				close(currentOut);
			if (tempPipe[1])
				close(tempPipe[1]);
		}
	}
	pList->head_pid = cmds[0]->pid;
	pList->tail_pid = cmds[i-1]->pid;
}

int checkSingleExec(void *data, void *params) {
	TaskManager tm = (TaskManager) ((void **) params)[0];
	ProcessList pList = (ProcessList) data;
	if (pList->head_pid == -1 && (strcmp(pList->input, "") == 0 || tm_isFileQueued(tm, pList->input) == 0)) {
		_DEBUG("Found a process able to execute\n");
		execProcess(tm, pList);
	}

	return 1;
}

void tm_updateExec(TaskManager tm) {
	_DEBUG("Updating execution list\n");
	void* params[1];
	params[0] = tm;
	list_map(tm->processList, &checkSingleExec, params);
}

int checkFinishedProcessIndex(void *data, void *params) {
	//TaskManager tm = (TaskManager) ((void **) params)[0];
	int *pid = (int *) ((void **) params)[1];
	int *index = (int *) ((void **) params)[2];
	void *ret = (void *) ((void **) params)[3];

	ProcessList elem = (ProcessList) data;
	if (ret != NULL)
		return 0;
	if (*pid == elem->tail_pid) { //foi este que terminou
		((void **) params)[3] = elem;
	} else {
		(*index)++;
	}

	return 1;
}

int finishedProcess(TaskManager tm, int pid) {
	_DEBUG("Finishing process: %d\n", pid);
	int index = 0;
	void *params[4];
	params[0] = tm;
	params[1] = &pid;
	params[2] = &index;
	params[3] = NULL;
	
	ProcessList term = list_search(tm->processList, &pid);
	if (term == NULL)
		return 0;
	
	tm_removeFile(tm, term->output);
	tm_removeProcess(tm, pid);
	_DEBUG("Process data disposed\n");
	return 1;
}

int argSize(char *str1, char delim){
	int i=0, x;
	for(x=0;str1[x]!='\0';x++){
		if (str1[x] == delim) 
			i++;
	} 
	return i;
}

void trim(char *str) {
	int firstPos = 0;
	int lastPos;
	int i, j;
	
	if(str==NULL) return;
	
	while(str[firstPos] == ' ')
		firstPos++;
	
	lastPos = strlen(str) - 1;
	while(str[lastPos] == ' ')
		lastPos--;
	
	for(j = 0, i = firstPos; i <= lastPos; i++, j++) {
		str[j] = str[i];
	}
	str[j] = '\0';
}

ProcessList stringToProcessList(char* string){
	char *tok1, *tok2, *tok3, *strprocess, *stringcopy, *strarg;
	char **cmdArray;
	int arg, argsize, cmdsize, i;
	stringcopy=(char *)malloc(strlen(string)+1);
	strcpy(stringcopy, string);
	
	tok1 = strtok(stringcopy, ">");
	tok2 = strtok(NULL, ">");
	tok3 = strtok(NULL, ">");
	
	ProcessList pList;
	pList=(ProcessList)malloc(sizeof(struct process_list));
	pList->processes = list_create(NULL, NULL, LIST_SIMPLE_FREE);
	pList->head_pid = -1;
	pList->tail_pid = -1;
	
	
	if (tok3 != NULL) {
		strcpy(pList->input, tok1);
		trim(pList->input);
		strprocess = tok2;
		strcpy(pList->output, tok3);
		trim(pList->output);
	}
	else {
		strcpy(pList->input, "");
		strprocess = tok1;
		strcpy(pList->output, tok2);
		trim(pList->output);
	}
	//_DEBUG("input: %s\n", pList->input);
	//_DEBUG("output: %s\n", pList->output);
	
	cmdsize = argSize(strprocess, '|') + 1;
	cmdArray = (char **) malloc(sizeof(char *) * cmdsize);
	strprocess = strtok(strprocess, "|");
	i = 0;
	//_DEBUG("cmdsize: %d\n", cmdsize);
	while(i< cmdsize) {
		cmdArray[i] = strprocess;
		trim(cmdArray[i]);
		//_DEBUG("%s\n", cmdArray[i]);
		strprocess = strtok(NULL, "|");
		i++;
	}
	
	i = 0;
	while(i < cmdsize){
		strprocess = cmdArray[i];
		i++;
		Process p;
		p = (Process)malloc(sizeof(struct process));
		trim(strprocess);
		argsize=argSize(strprocess, ' ')+1;
		//_DEBUG("ArgSize :%d\n", argsize);
		p->args = (char **)malloc(sizeof(char *)*argsize);
		
		strcpy(stringcopy,strprocess);
		strarg = strtok(stringcopy, " ");
		
		for(arg=0;arg<argsize;arg++){
			trim(strarg);
			p->args[arg]=(char*)malloc(strlen(strarg)+1);
			strcpy(p->args[arg], strarg);
			//_DEBUG("argumentos :%s\n",p->args[arg]);
			strarg = strtok(NULL, " ");
		}
		//_DEBUG("Comand's String :%s\n", strprocess);
		p->args[arg]=NULL;
		strcpy(p->cmd, p->args[0]);
		//_DEBUG("Comando :%s\n",p->cmd);
		list_insertTail(pList->processes, (Process) p);
	}
/*	
	if(tok3){
		trim(tok1);
		strcpy(pList->input, tok1);
		_DEBUG("Input :%s\n",pList->input);
		strprocess = strtok(tok2, ">");
		_DEBUG("Comand's String :%s\n", strprocess);
		cmdsize = argSize(strprocess, '|') + 1;
		cmdArray = (char **) malloc(sizeof(char *) * cmdsize);
		strprocess = strtok(strprocess, "|");
		i = 0;
		_DEBUG("cmdsize: %d\n", cmdsize);
		while(i< cmdsize) {
			cmdArray[i] = strprocess;
			trim(cmdArray[i]);
			_DEBUG("%s\n", cmdArray[i]);
			strprocess = strtok(NULL, "|");
			i++;
		}
		
		int i = 0;
		while(i < cmdsize){
			strprocess = cmdArray[i];
			i++;
			Process p;
			p = (Process)malloc(sizeof(struct process));
			trim(strprocess);
			argsize=argSize(strprocess, ' ')+1;
			_DEBUG("ArgSize :%d\n", argsize);
			p->args = (char **)malloc(sizeof(char *)*argsize);
			
			strcpy(stringcopy,strprocess);
			strarg = strtok(stringcopy, " ");
			
			for(arg=0;arg<argsize;arg++){
				trim(strarg);
				p->args[arg]=(char*)malloc(strlen(strarg)+1);
				strcpy(p->args[arg], strarg);
				_DEBUG("argumentos :%s\n",p->args[arg]);
				strarg = strtok(NULL, " ");
			}
			_DEBUG("Comand's String :%s\n", strprocess);
			p->args[arg]=NULL;
			strcpy(p->cmd, p->args[0]);
			_DEBUG("Comando :%s\n",p->cmd);
			list_insertTail(pList->processes, (Process) p);
			
			strprocess = strtok(NULL, "|");
		}
		trim(tok3);
		strcpy(pList->output, tok3);
		_DEBUG("Output :%s\n",pList->output);
	}
	else {
		strprocess = strtok(tok1, ">");
		_DEBUG("Comand's String :%s\n", strprocess);
		strprocess = strtok(strprocess, "|");
		
		while(strprocess){
			Process p;
			p = (Process)malloc(sizeof(struct process));
			trim(strprocess);
			strcpy(p->cmd, strprocess);
			_DEBUG("Comando :%s\n",p->cmd);
			p->args = (char **)malloc(sizeof(char *)*1);
			p->args[0] = (char *)malloc(strlen(strprocess));
			list_insertTail(pList->processes, (Process) p);
			
			strprocess = strtok(NULL, "|");
		}
		strcpy(pList->output, tok2);
		_DEBUG("Output :%s\n",pList->output);
	}
*/
	return pList;
}
