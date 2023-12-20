#ifndef GIOCO_H
#define GIOCO_H

#include "../utility.h"

#define MAX_OGGETTI_LOCAZIONE 10
#define MAX_OGGETTI_SCENARIO 20
#define MAX_LOCAZIONI_SCENARIO 5
#define MAX_GIOCATORI_GRUPPO 2
#define N_SCENARI 1

typedef struct oggetto {
    char *nome;
    char *descrizione;
    bool is_bloccato;
    bool has_enigma;
} oggetto;

typedef struct locazione {
    char *nome;
    char *descrizione;
    const oggetto *oggetti;
    const struct locazione *sottolocazioni;
} locazione;

typedef struct scenario {
    int id;
    char *nome;
    char *descrizione;
    const oggetto *oggetti;
    const locazione *locazioni;
} scenario;

typedef struct gruppo {
    bool attivo;
    struct sockaddr_in *indirizzi[MAX_GIOCATORI_GRUPPO];
} gruppo;

void prendi_scenari(char*);

#endif