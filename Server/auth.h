#ifndef AUTH_H
#define AUTH_H

#include <netinet/in.h>
#include "../utility.h"

#define USER_DIM 20
#define PASSWORD_DIM 20

typedef struct authentication {
    struct sockaddr_in addr;
    char* username[USER_DIM];
} authentication;

char* register_user(char*);
char* login_user(char*, struct sockaddr_in);
char* logout_user(int);
bool is_logged(int);

#endif