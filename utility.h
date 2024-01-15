#ifndef UTILITY_H
#define UTILITY_H

#include <netinet/in.h>

typedef enum bool { false, true } bool;

int invia_messaggio(int, char *, char*);
int ricevi_messaggio(int, char *, char *);
void print_addr(struct sockaddr_in *);
bool compara_addr(struct sockaddr_in *, struct sockaddr_in *);

#endif