default: all

all: server client

server: src/server.o
	gcc -o bin/server src/server.o

client: src/client.o
	gcc -o bin/client src/client.o

clean:
	$(RM) src/*.o bin/*
