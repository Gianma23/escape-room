#ifndef GIOCO_H
#define GIOCO_H

#include "../utility.h"

#define OGGETTI_CIMITERO 3
#define LOCAZIONI_CIMITERO 6
#define MAX_GIOCATORI_GRUPPO 2
#define N_SCENARI 1

typedef struct oggetto {
    const char *nome;
    char *descrizione;
    bool is_bloccato;
    bool is_preso;
} oggetto;

typedef struct locazione {
    char *nome;
    char *descrizione;
} locazione;

typedef struct scenario {
    char *nome;
    char *descrizione;
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