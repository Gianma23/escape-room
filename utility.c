#include "utility.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

bool invia_messaggio(int p_socket, char* p_messaggio, char* p_errore){
    int len = strlen(p_messaggio) + 1;
    uint16_t lmsg = htons(len);
    int ret;

    // Invio al partner la dimensione del messaggio
    ret = send(p_socket, &lmsg, sizeof(uint16_t), 0);
    if(ret < 0){
        perror(strcat(p_errore, " (lunghezza)\n"));
        return false;
    }

    //Invio al partner il messaggio
    ret = send(p_socket, p_messaggio, len, 0);
    if(ret < 0){
        perror(strcat(p_errore, "\n"));
        return false;
    }
    return true;
}

bool ricevi_messaggio(int p_socket, char* p_messaggio, char* p_errore){
    int ret;
    uint16_t lmsg;
    int len;

    ret = recv(p_socket, &lmsg, sizeof(uint16_t), 0);
    if(ret < 0){
        perror(strcat(p_errore, " (lunghezza)\n"));
        return false;
    }
				
    // Conversione in formato 'host'
    len = ntohs(lmsg);

    // Ricezione del messaggio
    ret = recv(p_socket, p_messaggio, len, 0);

    if(ret < 0){
        perror("Errore in fase di ricezione comando da un qualche client: \n");
        return false;
    }
    return true;
}