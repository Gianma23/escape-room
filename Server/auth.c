#define  _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "auth.h"
#include "gioco.h"

static authentication loggati[MAX_GIOCATORI_GRUPPO];
static int num_login = 0;

/* ================== IMPLEMENTAZIONE HEADER ================== */

/*  opt: opzioni del comando nel formato <username> <password>
    Registra l'utente con la password nel file users.txt */
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

/*  opt: opzioni del comando nel formato <username> <password>
    Effettua il login dell'utente, controllando che non sia 
    già loggato */
char* login_user(char* opt, int cl_sock)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char record_password[PASSWORD_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;
    bool trovato = false;

    /* parsing delle opzioni */
    user = strtok(opt, " ");
    if(user == NULL) {
        return "Parametri non sufficienti.\n";
    }
    password = strtok(NULL, " ");
    if(password == NULL) {
        return "Parametri non sufficienti.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
        
    /* controlli dimensioni input */
    if(strlen(user) > USER_DIM) {
        return "Username troppo lungo.\n";
    }
    if(strlen(password) > PASSWORD_DIM) {
        return "Password troppo lunga.\n";
    }

    if(num_login == MAX_GIOCATORI_GRUPPO) {
        return "Raggiunto il massimo numero di persone loggate insieme.\n";
    }
    if(num_login == 1 && cl_sock == loggati[0].sock) {
        return "Sei già loggato!\n";
    }
    printf("username loggato: %s, user: %s\n", loggati[0].username, user);
    if(num_login == 1 && strcmp(loggati[0].username, user) == 0) {
        return "Questo username è già loggato su un altro dispositivo.\n";
    }

    /* cerco l'utente nel file users.txt */
    filePtr = fopen("users.txt", "a+");
    if(filePtr == NULL) {
        printf("Errore di apertura file user.txt!\n");
        exit(1);
    }
    while (getline(&record, &len, filePtr) != -1) {
        sscanf(record, "%s %s", record_user, record_password);
        if(strcmp(record_user, user) != 0)
            continue;

        trovato = true;
        if(strcmp(record_password, password) != 0) {
            fclose(filePtr);
            return "Password errata! Ritenta.\n";
        }
        /* username e password corretti */
        fclose(filePtr);
        break;
    }
    if(!trovato) {
        return "Username non registrato.\n";
    }
    loggati[num_login].sock = cl_sock;
    strcpy(loggati[num_login].username, user);
    num_login++;
    return "Login avvenuto con successo!\n";
}

/*  cl_sock: socket del client 
    Effettua il logout dello user associato a cl_sock */
char* logout_user(int cl_sock)
{
    int i;
    for(i = 0; i < num_login; i++) {
        if(cl_sock == loggati[i].sock) {
            memset(&loggati[i], 0, sizeof(authentication));
            num_login--;
            break;
        }
    }
    return "Logout effettuato.\n";
}

/*  cl_sock: socket del client 
    La funzione controlla se lo user associato a cl_sock sia loggato */
bool is_logged(int cl_sock)
{
    int i;
    for(i = 0; i < num_login; i++) {
        if(cl_sock == loggati[i].sock) {
            return true;
        }
    }
    return false;
}