#include "utility.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int invia_messaggio(int p_socket, char* p_messaggio, char* p_errore)
{
    int ret;
    char err[256];
    size_t len = strlen(p_messaggio) + 1;
    uint16_t lmsg = htons(len);

    /* Invio dimensione del messaggio */
    ret = send(p_socket, &lmsg, sizeof(uint16_t), 0);
    if(ret < 0){
        strcpy(err, p_errore);
        perror(strcat(err, " (invio lunghezza)"));
        return ret;
    }
    if(ret == 0) {
        return  ret;
    }
    /* Invio messaggio */
    ret = send(p_socket, p_messaggio, len, 0);
    if(ret < 0){
        strcpy(err, p_errore);
        perror(strcat(err, " (invio messaggio)"));
    }
    return ret;
}

int ricevi_messaggio(int p_socket, char* p_messaggio, char* p_errore){
    int ret;
    char err[256];
    uint16_t lmsg;
    size_t len;

    /* Ricezione della lunghezza del messaggio */
    ret = recv(p_socket, &lmsg, sizeof(uint16_t), 0);
    if(ret < 0){
        strcpy(err, p_errore);
        perror(strcat(err, " (ricezione lunghezza)"));
        return ret;
    }
    if(ret == 0) {
        return  ret;
    }
    /* Conversione in formato 'host' */
    len = ntohs(lmsg);

    /* Ricezione del messaggio */
    ret = recv(p_socket, p_messaggio, len, 0);
    if(ret < 0){
        strcpy(err, p_errore);
        perror(strcat(err, " (ricezione messaggio)"));
    }
    return ret;
}

/* debug */
void print_addr(struct sockaddr_in *addr)
{
    char tmp[20];
    memset(tmp, 0, sizeof(tmp));
    inet_ntop(AF_INET, &(addr->sin_addr.s_addr), tmp, 20);
    printf("%s:%d\n", tmp, addr->sin_port);
}