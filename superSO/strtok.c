#include <string.h>
#include "defs.h"
#include "linked_list.h"
#include "taskManager.h"

struct process_list stringToProcessList(char* string){
	char *tok1, *tok2, *tok3, *strProcess;
	
	tok1 = strtok(string, ">");
	printf("%s\n",tok1);
	tok2 = strtok(NULL, ">");
	printf("%s\n",tok2);
	tok3 = strtok(NULL, ">");
	printf("%s\n",tok3);
	
	ProcessList pList;
	pList=(ProcessList)malloc(sizeof(struct process_list));
	pList->processes = list_create(NULL, NULL, LIST_SIMPLE_FREE);
	
	if(tok3){
		strcpy(pList->input, tok1);
		
		strprocess = strtok(tok2, ">");
		strprocess = strtok(NULL, "|");
		while(strprocess){
			Process p;
			p = (Process)malloc(sizeof(struct process));
			strcpy(p->cmd, strprocess);
			p->args = (char **)malooc(sizeof((char *)*1));
			p->args[0] = (char *)malloc(strlen(strprocess));
			list_insertTail(pList->processes, (Process) p);
			
			strprocess = strtok(NULL, "|");
		}
		strcpy(pList->output, tok3);
	}
	else {
		strprocess = strtok(tok1, ">");
		strprocess = strtok(NULL, "|");
		while(strprocess){
			Process p;
			p = (Process)malloc(sizeof(struct process));
			strcpy(p->cmd, strprocess);
			p->args = (char **)malooc(sizeof((char *)*1));
			p->args[0] = (char *)malloc(strlen(strprocess));
			list_insertTail(pList->processes, (Process) p);
			
			strprocess = strtok(NULL, "|");
		}
		strcpy(pList->output, tok2);
	}
}


int main(int argc, char** argv){
	stringToProcessList(argv[1]);
	return 0;
}
