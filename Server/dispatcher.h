#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <netinet/in.h>
#include "../utility.h"

#define N_COMANDI 11

typedef struct comando {
    char *nome;
    char* (*handler)();
    char* descrizione;
} comando;

void command_dispatcher(int, char *, char *);
void comandi_client(char *);

#endif