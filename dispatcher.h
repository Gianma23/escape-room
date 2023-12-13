#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <netinet/in.h>
#include "utility.h"

typedef struct comando {
    char *nome;
    char* (*handler)();
} comando;

typedef struct gruppo {
    bool attivo;
    struct sockaddr_in* giocatori[2];
} gruppo;

void command_handler(int, char *, char *);

#endif