all: server client
server: server.o dispatcher.o utility.o auth.o
	gcc -Wall server.o dispatcher.o utility.o auth.o -o server 

client: client.o utility.o 
	gcc -Wall client.o -o client

client.o: client.c
	gcc -Wall -g -c client.c

utility.o: utility.c utility.h 
	gcc -Wall -g -c utility.c -o utility.o

server.o: server.c utility.o
	gcc -Wall -g -c server.c -o server.o

dispatcher.o: dispatcher.c dispatcher.h
	gcc -Wall -g -c dispatcher.c -o dispatcher.o

auth.o: auth.c auth.h
	gcc -Wall -g -c auth.c -o auth.o

clean: 
	rm *.o server client