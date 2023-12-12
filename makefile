all: server client
server: server.o cmd_handler.o
	gcc -Wall server.o cmd_handler.o -o server 

client: client.o utility.o
	gcc -Wall client.o -o client

client.o: client.c
	gcc -Wall -g -c client.c

utility.o: utility.c utility.h 
	gcc -Wall -g -c utility.c -o utility.o

server.o: server.c 
	gcc -Wall -g -c server.c -o server.o

cmd_handler.o: cmd_handler.c cmd_handler.h
	gcc -Wall -g -c cmd_handler.c -o cmd_handler.o

clean: 
	rm *.o server client