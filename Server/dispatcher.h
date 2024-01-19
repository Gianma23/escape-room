#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <netinet/in.h>
#include "../utility.h"

void command_dispatcher(int, char *, char *);
void comandi_client(char *);

#endif