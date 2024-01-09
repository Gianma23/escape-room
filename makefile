all: server client other clean

server: server.o utility.o dispatcher.o auth.o timer.o gioco.o cimitero.o
	gcc -Wall server.o utility.o dispatcher.o auth.o timer.o gioco.o cimitero.o -o server 

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

cimitero.o: Server/scenari/cimitero.c Server/scenari/cimitero.h
	gcc -Wall -g -c -std=c89 Server/scenari/cimitero.c -o cimitero.o

auth.o: Server/auth.c Server/auth.h
	gcc -Wall -g -c -std=c89 Server/auth.c -o auth.o

timer.o: Server/timer.c Server/timer.h
	gcc -Wall -g -c -std=c89 Server/timer.c -o timer.o

.PHONY: clean

clean: 
	rm *.o