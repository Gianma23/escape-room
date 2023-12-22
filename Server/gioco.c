#include <stdio.h>
#include <string.h>
#include "gioco.h"

static int scenario_scelto = -1;

/* SCENARI ============================ */

static oggetto oggetti_cimitero[] = {
    {"lucchetto", "Sembra che serva una chiave per aprirlo...\n", true, false},
    {"libro", "tapa tapa ", true, false},
    {"boh", "hi hi", false, false}
};

static const utilizzo tabella_utilizzi_cimitero[] = {

};

static locazione locazioni_cimitero[] = {
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

static scenario scenario_cimitero = {
    "cimitero",
    "blablabla\n",
    oggetti_cimitero,
    locazioni_cimitero
};

static scenario *scenari[] = {
    &scenario_cimitero
};

/* IMPLEMENTAZIONE FUNZIONI HEADER ============= */

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

char* prendi_oggetto(char *oggetto)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < OGGETTI_CIMITERO; i++) {
        if(strcmp(oggetto, scen->oggetti[i].nome) != 0) {
            continue;
        }
        if(scen->oggetti[i].is_bloccato) {
            return "Oggetto bloccato!\n";
        }
        if(scen->oggetti[i].is_preso) {
            return "Oggetto già preso!\n";
        }
        /* TODO: se c'è enigma attivarlo */
        scen->oggetti[i].is_preso = true;
        return "Oggetto raccolto!\n";
    }
    return "Oggetto non trovato.\n";
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