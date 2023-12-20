#ifndef AUTH_H
#define AUTH_H

#include <netinet/in.h>

#define USER_DIM 20
#define PASSWORD_DIM 20

char* register_user(char*);
char* login_user(char*, struct sockaddr_in);

#endif