#include <stdio.h>
#include <string.h>
#include "gioco.h"

static int scenario_scelto = -1;

/* SCENARI ============================ */

static oggetto oggetti_cimitero[] = {
    {"lucchetto", 
     "Sembra che serva una chiave per aprirlo...\n",
     "Il lucchetto è sbloccato adesso.\n",
     "\n",
     true, false},
    {"libro",
     "tapa tapa ", 
     "\n",
     "\n",
     true, false},
    {"boh", "hi hi",
     "\n",
     "\n",
     false, false}
};

static const utilizzo tabella_utilizzi_cimitero[] = {
    {"chiave", "lucchetto"}
};

static locazione locazioni_cimitero[] = {
    {"cancello",
     "cancellino carino\n",
     2,
     {&oggetti_cimitero[0], &oggetti_cimitero[1]}},
    {"albero",
     "albero waw\n",
     1},
    {"buco",
     "stocazzo\n",1},
    {"chiesa",
     "chiesa sesso",1},
    {"altare",
     "basdad",1},
    {"tomba",
     "buh",1}
};

static scenario scenario_cimitero = {
    "cimitero",
    "Ti trovi in un cimitero, l'unica uscita è un ++cancello++. Alla\n"
    "sua sinistra vedi un ++albero++ secco, lì accanto è presente una\n"
    "++tomba++. Dietro di te c'è una ++chiesa++.\n",
    3, 6,
    oggetti_cimitero,
    locazioni_cimitero
};

static scenario *scenari[] = {
    &scenario_cimitero
};

/* FUNZIONI DI UTILITÀ ==================== */

char* descrizione_locazione(locazione *loc)
{
/* TODO: concatenare la descrizione base con le descrizioni degli oggetti 
    non presi ancora. */
    return NULL;
}

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
    
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i]; 
        if(strcmp(opzione, obj->nome) == 0) {
            return obj->is_bloccato ? obj->descrizione_bloccato : obj->descrizione_sbloccato;
        }
    }
    for(i = 0; i < scen->n_locazioni; i++) {
        if(strcmp(opzione, scen->locazioni[i].nome) == 0) {
            return descrizione_locazione(&scen->locazioni[i]);
        }
    }
    return "Oggetto/Locazione non trovata.\n";
}

char* prendi_oggetto(struct sockaddr_in addr,char *obj)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        if(strcmp(obj, scen->oggetti[i].nome) != 0) {
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
        scen->oggetti[i].addr_possessore = addr;
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