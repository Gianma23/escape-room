#ifndef UTILITY_H
#define UTILITY_H

typedef enum bool { false, true } bool;

bool invia_messaggio(int p_socket, char* p_messaggio, char* p_errore);
bool ricevi_messaggio(int p_socket, char* p_messaggio, char* p_errore);

#endif