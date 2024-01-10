#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include "dispatcher.h"
#include "gioco.h"
#include "auth.h"
#include "../utility.h"

#define BUFFER_DIM 1024
#define QUEUE_DIM 10


int main(int argc, char *argv[])
{
    struct sockaddr_in sv_addr, cl_addr;
    fd_set master, read_fds;
    int fdmax;
    int ret, i;
    socklen_t addrlen;
    in_port_t porta = htons(atoi(argv[1]));
    char buffer[BUFFER_DIM] = "";
    int list_sock, comm_sock;
    char input[6];

    printf("======================================================\n"
           "                    SERVER STARTED                    \n"
           "======================================================\n"
           "Comandi disponibili:\n"
           "    start       avvia il server di gioco\n"
           "    stop        termina il server\n");
    
    while(1) {
        memset(input, 0, sizeof(input));
        fgets(input, 6, stdin);
        if(strcmp(input, "start") == 0) {
            break;
        }
        if(strcmp(input, "stop\n\0") == 0) {
            printf("Arresto del server\n");
            exit(1);
        }
        printf("Comando non disponibile.\n");
    }

    printf("Avvio del server in corso...\n");
    list_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(list_sock < 0) {
        perror("Errore in fase di creazione del socket");
        exit(1);
    }

    ret = setsockopt(list_sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if(ret < 0) {
        perror("Errore in fase di setsockopt");
        exit(1);
    }

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = porta;
    sv_addr.sin_addr.s_addr = INADDR_ANY;

    ret = bind(list_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0) {
        perror("Errore in fase di bind");
        exit(1);
    }

    ret = listen(list_sock, 10);
    if(ret < 0) {
        perror("Errore in fase di listen");
        exit(1);
    }
    printf("Socket di ascolto creato. fd: %d\n", list_sock);

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    FD_SET(list_sock, &master);
    FD_SET(0, &master);
    fdmax = list_sock;

    for(;;) {
        memset(buffer, 0, sizeof(buffer));
        read_fds = master;
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        for(i = 0; i <= fdmax; i++) {
            if(FD_ISSET(i, &read_fds)) {
                /* stdin */
                if(i == 0) {
                    scanf("%s", buffer);
                    command_dispatcher(i, buffer, "server");
                }
                /* socket di ascolto */
                else if(i == list_sock) {
                    addrlen = sizeof(cl_addr);
                    memset(&cl_addr, 0, sizeof(cl_addr));
                    comm_sock = accept(list_sock, (struct sockaddr*)&cl_addr, &addrlen);
                    if(comm_sock < 0) {
                        perror("Errore in fase di accept");
                        exit(1);
                    }
                    printf("Nuovo client connesso, socket di comunicazione: %d\n", comm_sock);

                    /* invio al client degli scenari e comandi disponibili */
                    /* TODO: mettere in una sola send */
                    memset(buffer, 0, sizeof(buffer));
                    prendi_scenari(buffer);
                    invia_messaggio(comm_sock, buffer, "Errore invio scenari\n");
                    comandi_client(buffer);
                    invia_messaggio(comm_sock, buffer, "Errore invio comandi\n");
                    printf("Scenari e comandi disponibili inviati.\n\n");

                    FD_SET(comm_sock, &master);
                    if(comm_sock > fdmax) {
                        fdmax = comm_sock;
                    }
                }
                /* socket diverso da quello di ascolto */
                else {
                    ret = ricevi_messaggio(i, buffer, "Errore ricezione comando dal client\n");
                    if(ret == STDIN_FILENO) {
                        printf("Sconnessione client in corso...\n");
                        /* printf("%s", logout_user(i)); */
                        /* TODO: funzione reset scenario, gruppo e logout di tutti */
                        close(i);
                        printf("Socket %d chiuso.\n", i);
                        FD_CLR(i, &master);
                        printf("Socket rimosso dal set dei descrittori.\n"
                               "Sconnessione client effettuata con successo.\n\n");
                        continue;
                    }
                    
                    printf("comando ricevuto: '%s' dal socket %d\n", buffer, i);
                    command_dispatcher(i, buffer, "client");
                    printf("\n");
                }
            }
        } /* fine for */
    } /* fine for(;;) */
}