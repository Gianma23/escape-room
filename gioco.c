#include "gioco.h"
#include <stdio.h>

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
    "blablabla",
    oggetti_cimitero,
    locazioni_cimitero
};