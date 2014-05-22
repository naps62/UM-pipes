xCC=gcc
CFLAGS=-Wall -ggdb -O3
DEBUG=-ggdb

GESTOR_NAME=gestor
SUBMETER_NAME=submeter
TESTE_NAME=super_so_teste

linked_list.o: linked_list.c linked_list.h
gestor.o: gestor.c
submeter.o: submeter.c submeter.h
taskManager.o: taskManager.c taskManager.h
teste.o: teste.c

GESTOR_C= gestor.o linked_list.o taskManager.o
SUBMETER_C= submeter.o linked_list.o
TESTE_C= teste.o linked_list.o

default: gestor

gestor: $(GESTOR_C) Makefile
	$(CC) $(CFLAGS) -o $(GESTOR_NAME) $(GESTOR_C)

submeter: $(SUBMETER_C) Makefile
	$(CC) $(CFLAGS) $(SUBMETER_C) -o $(SUBMETER_NAME)

teste: $(TESTE_C) Makefile	
	$(CC) $(CFLAGS) $(TESTE_C) -o $(TESTE_NAME)
	./$(TESTE_NAME)

clean:
	rm -f *.o
