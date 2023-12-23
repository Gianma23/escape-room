#ifndef GIOCO_H
#define GIOCO_H

#include "../utility.h"

#define MAX_OGGETTI_LOCAZIONE 5
#define MAX_GIOCATORI_GRUPPO 2
#define N_SCENARI 1

typedef struct oggetto {
    const char *nome;
    char *descrizione_bloccato;
    char *descrizione_sbloccato;
    char *descrizione_locazione;
    bool is_bloccato;
    bool is_preso;
} oggetto;

typedef struct locazione {
    const char *nome;
    char *descrizione_iniziale;
    char *n_oggetti;
    char *oggetti[MAX_OGGETTI_LOCAZIONE];
} locazione;

typedef struct scenario {
    const char *nome;
    const char *descrizione;
    const int n_oggetti;
    const int n_locazioni;
    oggetto *oggetti;
    locazione *locazioni;
} scenario;

typedef struct utilizzo {
    
} utilizzo;

typedef struct gruppo {
    bool attivo;
    int num_giocatori;
    struct sockaddr_in indirizzi[MAX_GIOCATORI_GRUPPO];
} gruppo;

void prendi_scenari(char *);
char* prendi_descrizione(char *);
char* prendi_oggetto(char *);
char* inizia_scenario(int);
bool is_game_started();

#endif