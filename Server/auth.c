#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "auth.h"
#include "../utility.h"
#include "gioco.h"
#include <arpa/inet.h>

static struct sockaddr_in indirizzi_login[MAX_GIOCATORI_GRUPPO] = {0, 0};
static int num_login = 0;

/* DESCRIZIONE */
char* register_user(char *opt)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;

    user = strtok(opt, " ");
    if(user == NULL) {
        return "Errore in fase di registrazione.\n";
    }
    password = strtok(NULL, " ");
    if(password == NULL) {
        return "Errore in fase di registrazione.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    
    filePtr = fopen("users.txt", "a+");
    if(filePtr == NULL) {
        printf("Errore di apertura file user.txt!\n");
        exit(1);
    }
    /* guardo se lo username è già stato preso */
    while (getline(&record, &len, filePtr) != -1) {
        sscanf(record, "%s", record_user);
        if(strcmp(record_user, user) == 0) {
            return "Username già preso! Provane un altro.\n";
        }
    }
    /* inserisco il nuovo user */
    fprintf(filePtr, "%s %s", user, password);
    fclose(filePtr);
    return "Registrazione avvenuta con successo!\n";
}

/* DESCRIZIONE */
char* login_user(char* opt, struct sockaddr_in client_sock)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char record_password[PASSWORD_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;
    bool trovato = false;
    
    if(num_login == MAX_GIOCATORI_GRUPPO) {
        return "Raggiunto il massimo numero di persone loggate insieme.\n";
    }
    /* TODO: struttura che salva lo username insieme all'ip, cosicchè non ci si possa loggare con lo stesso
        account da due client diversi */
    if(num_login == 1) {
        if(client_sock.sin_addr.s_addr == indirizzi_login[0].sin_addr.s_addr && client_sock.sin_port == indirizzi_login[0].sin_port)
            return "Sei già loggato!\n";
    }

    user = strtok(opt, " ");
    if(user == NULL) {
        return "Errore in fase di login.\n";
    }
    password = strtok(NULL, " ");
    if(password == NULL) {
        return "Errore in fase di login.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.";
    }
    
    filePtr = fopen("users.txt", "a+");
    if(filePtr == NULL) {
        printf("Errore di apertura file user.txt!\n");
        exit(1);
    }
    /* cerco lo username */
    while (getline(&record, &len, filePtr) != -1) {
        sscanf(record, "%s %s", record_user, record_password);
        if(strcmp(record_user, user) != 0)
            continue;

        trovato = true;
        printf("%s %s", record_password, password);
        if(strcmp(record_password, password) != 0) {
            return "Password errata! Ritenta.\n";
        }
        /* Utente e password corretti */
        break;
    }
    fclose(filePtr);
    indirizzi_login[num_login++] = client_sock;
    return "Login avvenuto con successo!\n";
}