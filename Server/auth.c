#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "auth.h"
#include "../utility.h"

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

char* login_user(char* opt)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char record_password[PASSWORD_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;
    bool trovato = false;

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
    return "Login avvenuto con successo!\n";
}