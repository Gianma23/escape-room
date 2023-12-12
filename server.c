#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include "cmd_handler.h"

#define BUFFER_DIM 1024
#define QUEUE_DIM 10


int main(int argc, char *argv[])
{
    struct sockaddr_in sv_addr, cl_addr;
    fd_set master, read_fds;
    int fdmax;
    int ret, i;
    int len_msg;
    socklen_t addrlen;
    char buffer[BUFFER_DIM] = "";
    int list_sock, comm_sock;

    char input[10];
    while(1) {
        fgets(input, 6, stdin);
        if(strcmp(input, "start") == 0) {
            break;
        }
        if(strcmp(input, "stop") == 0) {
            return 0;
        }
        printf("Comando non disponibile.\n");
    }

    list_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = 4242;
    sv_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(list_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0) {
        /* gestisci errore */
    }

    ret = listen(list_sock, 10);
    if(ret < 0) {
        /* gestisci errore */
    }

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(list_sock, &master);
    FD_SET(0, &master);
    fdmax = list_sock;

    for(;;) {
        memset(buffer, 0, 1024);
        read_fds = master;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        for(i = 0; i <= fdmax; i++) {
            if(FD_ISSET(i, &read_fds)) {
                /* stdin */
                if(i == 0) {
                    scanf("%s", buffer);
                    command_handler(buffer, "server");
                }
                /* socket di ascolto */
                else if(i == list_sock) {
                    addrlen = sizeof(cl_addr);
                    comm_sock = accept(list_sock, (struct sockaddr*)&cl_addr, &addrlen);
                    if(comm_sock < 0) {
                        /* errore */
                    }

                    FD_SET(comm_sock, &master);
                    if(comm_sock > fdmax) {
                        fdmax = comm_sock;
                    }
                }
                /* socket diverso da quello di ascolto */
                else {
                    ret = recv(i, &len_msg, sizeof(uint16_t), 0);
                    len_msg = ntohs(len_msg);
                    ret = recv(i, buffer, len_msg, 0);
                    if(ret < 0) {
                        /* errore */
                    }
                    printf("comando ricevuto: %s", buffer);
                    command_handler(buffer, "client");
                }
            }
        }
    }
}