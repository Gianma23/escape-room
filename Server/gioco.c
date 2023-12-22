#include <stdio.h>
#include <string.h>
#include "gioco.h"

static int scenario_scelto = -1;

/* SCENARI ============================ */

static const oggetto oggetti_cimitero[] = {
    {"lucchetto", "Sembra che serva una chiave per aprirlo...\n", true, false},
    {"libro", "tapa tapa ", true, true},
    {"boh", "hi hi", false, false}
};

static const utilizzo tabella_utilizzi_cimitero[] = {

};

static const locazione locazioni_cimitero[] = {
    {"cancello",
     "cancellino carino\n"},
    {"albero",
     "albero waw\n"},
    {"buco",
     "stocazzo\n"},
    {"chiesa",
     "chiesa sesso"},
    {"altare",
     "basdad"},
    {"tomba",
     "buh"}
};

static const scenario scenario_cimitero = {
    "cimitero",
    "blablabla\n",
    oggetti_cimitero,
    locazioni_cimitero
};

static const scenario *scenari[] = {
    &scenario_cimitero
};

/* IMPLEMENTAZIONE FUNZIONI HEADER */

/*  buf: buffer nel quale scrivere.
    Ritorna gli scenari disponibili */
void prendi_scenari(char *buf)
{
    int i;
    strcpy(buf, "Scenari disponibili:\n");
    char tmp[1024];
    for(i = 0; i < N_SCENARI; i++) {
        sprintf(tmp, "%d:%s\n", i, scenari[i]->nome);
        strcat(buf, tmp);
    }
}

char* prendi_descrizione(char *opzione)
{
    int i;
    const scenario *scen = scenari[scenario_scelto];
    if(opzione == NULL) {
        return scen->descrizione;
    }
    
    for(i = 0; i < OGGETTI_CIMITERO; i++) {
        if(strcmp(opzione, scen->oggetti[i].nome) == 0) {
            return scen->oggetti[i].descrizione;
        }
    }
    for(i = 0; i < LOCAZIONI_CIMITERO; i++) {
        if(strcmp(opzione, scen->locazioni[i].nome) == 0) {
            return scen->locazioni[i].descrizione;
        }
    }
    return "Oggetto/Locazione non trovata.\n";
}

char* inizia_scenario(int id_scenario)
{
    if(id_scenario < 0 || id_scenario >= N_SCENARI) {
        return "Scenario non disponibile.\n";
    }
    scenario_scelto = id_scenario;
    return "Scenario iniziato, buona fortuna!\n";
}

/* Ritorna true se è stato scelto uno scenario e dunque è in corso una partita */
bool is_game_started()
{
    return scenario_scelto != -1;
}