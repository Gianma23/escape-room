#ifndef AUTH_H
#define AUTH_H

#include <netinet/in.h>
#include "../utility.h"

char* register_user(char*);
char* login_user(char*, int);
char* logout_user(int);
bool is_logged(int);

#endif