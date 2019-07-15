DIRC=./src/client
DIRS=./src/server

all:	client	server

client	:	client.o
			gcc	-o	client	client.o
client.o	:	$(DIRC)/main.c	./src/constants.h
			gcc	-g3	-c	$(DIRC)/main.c	-o	client.o
server	:	server.o
				gcc	-pthread	-o	server	server.o
server.o	:	$(DIRS)/main.c	./src/constants.h
			gcc	-g3	-c	$(DIRS)/main.c	-o	server.o
clean	:	
			rm -rf -f *.o server	client	./data/temp
