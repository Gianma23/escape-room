all: server client other clean

server: server.o utility.o dispatcher.o auth.o gioco.o
	gcc -Wall server.o utility.o dispatcher.o auth.o gioco.o -o server 

client: client.o utility.o 
	gcc -Wall client.o -o client

other: client.o utility.o 
	gcc -Wall client.o -o other

client.o: Client/main.c
	gcc -Wall -g -c -std=c89 Client/main.c -o client.o

server.o: Server/main.c
	gcc -Wall -g -c -std=c89 Server/main.c -o server.o

utility.o: utility.c utility.h 
	gcc -Wall -g -c -std=c89 utility.c -o utility.o

dispatcher.o: Server/dispatcher.c Server/dispatcher.h
	gcc -Wall -g -c -std=c89 Server/dispatcher.c -o dispatcher.o

gioco.o: Server/gioco.c Server/gioco.h
	gcc -Wall -g -c -std=c89 Server/gioco.c -o gioco.o

auth.o: Server/auth.c Server/auth.h
	gcc -Wall -g -c -std=c89 Server/auth.c -o auth.o

.PHONY: clean

clean: 
	rm *.o