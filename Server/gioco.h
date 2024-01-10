#ifndef GIOCO_H
#define GIOCO_H

#include "../utility.h"

#define MAX_OGGETTI_LOCAZIONE 5
#define MAX_GIOCATORI_GRUPPO 2
#define N_SCENARI 1

typedef struct oggetto {
    char *nome;
    char *descrizione_bloccato;
    char *descrizione_sbloccato;
    char *descrizione_locazione;
    /* se è bloccato non può essere il primo parametro di use */
    bool is_bloccato;
    /* se ha un enigma deve essere risolto prima di prenderlo */
    bool has_enigma;
    bool is_preso;
    struct sockaddr_in addr_possessore;
    /* TODO: salvare IP di chi prende l'oggetto per verificare se può usarlo con la use
        (non ci sarebbe questo problema senza multiplayer) */
} oggetto;

typedef struct locazione {
    char *nome;
    char *descrizione_iniziale;
    int n_oggetti;
    oggetto *oggetti[MAX_OGGETTI_LOCAZIONE];
} locazione;

typedef struct utilizzo {
    char *primo;
    char *secondo;
} utilizzo;

typedef struct scenario {
    char *nome;
    char *descrizione;
    int n_oggetti;
    int n_locazioni;
    int n_utilizzi;
    oggetto *oggetti;
    locazione *locazioni;
    utilizzo *utilizzi;
} scenario;

typedef struct gruppo {
    bool attivo;
    int num_giocatori;
    struct sockaddr_in indirizzi[MAX_GIOCATORI_GRUPPO];
} gruppo;

void prendi_scenari(char *);
char* prendi_descrizione(char *);
char* prendi_oggetto(struct sockaddr_in, char *);
char* utilizza_oggetti(struct sockaddr_in, char *, char *);
char* inizia_scenario(int);
bool reset_scenario(int);
bool is_game_started();

#endif