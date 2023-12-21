#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <netinet/in.h>
#include "../utility.h"

#define N_COMANDI_SERVER 1
#define N_COMANDI_CLIENT 9
#define COMANDO_DIM 64

typedef struct comando {
    char *nome;
    char* (*handler)();
    char* descrizione;
} comando;

void command_dispatcher(int, char *, char *);
void comandi_client(char *);

#endif