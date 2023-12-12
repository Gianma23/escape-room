#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "cmd_handler.h"

#define COMANDO_DIM 64
#define USER_DIM 20
#define PASSWORD_DIM 20

void comando_register(char *opt)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;

    user = strtok(opt, " ");
    if(user == NULL) {
        /* errore */
    }
    password = strtok(NULL, " ");
    if(password == NULL) {
        /* errore */
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
            /* avvisa il client */
            return;
        }
    }
    /* inserisco il nuovo user */
    fprintf(filePtr, "%s %s", user, password);
    fclose(filePtr);
}

void comando_login(char* opt)
{
    char *record = NULL;
    char record_user[USER_DIM];
    char record_password[PASSWORD_DIM];
    char *user = NULL;
    char *password = NULL;
    FILE *filePtr = NULL;
    size_t len = 0;

    user = strtok(opt, " ");
    if(user == NULL) {
        /* errore */
    }
    password = strtok(NULL, " ");
    if(password == NULL) {
        /* errore */
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

        if(strcmp(record_password, password) != 0) {
            /* password errata */
            return;
        }
        /* utente e password corretti */
    }
    fclose(filePtr);
}

void comando_creategroup()
{
    
}

void comando_joingroup()
{
    
}

void comando_start()
{

}

void comando_look() 
{

}

void comando_use()
{
    
}

void comando_take()
{
    
}

void comando_stop()
{

}

const comando lista_comandi_client[] = {
    {"register", comando_register},
    {"login", comando_login},
    {"creategroup", comando_creategroup},
    {"joingroup", comando_joingroup},
    {"start", comando_start},
    {"look", comando_look},
    {"use", comando_use},
    {"take", comando_take}
};
const comando lista_comandi_server[] = {
    /* {"start", comando_start}, */
    {"stop", comando_stop}
};

void command_handler(char *buffer, char *soggetto)
{
    int i;
    char com[COMANDO_DIM];
    const comando *lista_comandi = strcmp(soggetto, "server") == 0 ? lista_comandi_server : lista_comandi_client;
    const int NUM_COMANDI = (sizeof(*lista_comandi)/sizeof(comando));
    
    sscanf(buffer, "%s", com);

    for(i = 0; i < NUM_COMANDI; i++) {
        if(strcmp(com, lista_comandi[i].nome) == 0) {
            printf("comando trovato");
            buffer = strchr(buffer, ' ') + 1;
            lista_comandi[i].handler(buffer);
            return;
        }
    }   
    printf("comando non trovato.\n");
}


