#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <netinet/in.h>
#include "utility.h"

#define N_COMANDI_SERVER 1
#define N_COMANDI_CLIENT 9

typedef struct comando {
    char *nome;
    char* (*handler)();
} comando;

void command_handler(int, char *, char *);

#endif