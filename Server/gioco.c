#include <stdio.h>
#include <string.h>
#include "gioco.h"

static const oggetto oggetti_cimitero[] = {
    
};

static const oggetto oggetti_cimitero_cancello[] = {
    {"lucchetto", "Sembra che serva una chiave per aprirlo...", true, false}
};

static const oggetto oggetti_cimitero_albero[] = {

};

static const locazione locazioni_cimitero_albero[] = {

};

static const oggetto oggetti_cimitero_tomba[] = {

};

static const oggetto oggetti_cimitero_chiesa[] = {
    {"libro", "", true, true},
    {"boh",}
};

static const locazione locazioni_cimitero_chiesa[] = {
    
};

static const locazione locazioni_cimitero[] = {
    {"cancello",
     "cancellino carino",
     oggetti_cimitero_cancello,
     NULL},
    {"albero",
     "albero waw",
     oggetti_cimitero_albero,
     locazioni_cimitero_albero},
    {"chiesa",
     "chiesa sesso",
     oggetti_cimitero_chiesa,
     locazioni_cimitero_chiesa},
    {"tomba",
     "buh",
     oggetti_cimitero_tomba,
     NULL}
};

static const scenario scenario_cimitero = {
    1,
    "cimitero",
    "blablabla",
    oggetti_cimitero,
    locazioni_cimitero
};

static const scenario *scenari[] = {
    &scenario_cimitero
};

/*  buf: buffer nel quale scrivere.
    Ritorna gli scenari disponibili */
void prendi_scenari(char *buf)
{
    int i;
    strcpy(buf, "Scenari disponibili:\n");
    char tmp[1024];
    for(i = 0; i < N_SCENARI; i++) {
        sprintf(tmp, "%d:%s\n", scenari[i]->id, scenari[i]->nome);
        strcat(buf, tmp);
    }
}