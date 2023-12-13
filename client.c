#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utility.c"

int main(int argc, char *argv[])
{
    int ret;
    int cl_sock;
    struct sockaddr_in sv_addr;
    in_port_t porta = htons(atoi(argv[1]));
    char buffer[1024];

    cl_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = porta;
    inet_pton(AF_INET, "127.0.0.1", &sv_addr.sin_addr);

    ret = connect(cl_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0) {
        perror("errore di connessione");
        exit(1);
    }
    for(;;) {
        memset(buffer, 0, 50);
        fgets(buffer, 50, stdin);
        if(!invia_messaggio(cl_sock, buffer, "Errore in fase di invio riga di comando find.")) 
            exit(1);

        printf("messaggio inviato con successo\n");
    } 
}