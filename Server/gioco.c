#include <stdio.h>
#include "gioco.h"

static const oggetto oggetti_cimitero[] = {

};

static const oggetto oggetti_cimitero_cancello[] = {
    /* lucchetto */
};

static const oggetto oggetti_cimitero_albero[] = {

};

static const locazione locazioni_cimitero_albero[] = {

};

static const oggetto oggetti_cimitero_tomba[] = {

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
     NULL},
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

char* prendi_scenari()
{
    int i;
    char *res = "";
    for(i = 0; i < N_SCENARI; i++) {
        sprintf(res, "%d:%s", scenari[i]->id, scenari[i]->nome);
    }
    printf(res);
    return res;
}