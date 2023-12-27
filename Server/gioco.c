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
     false, false},
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
     "Provi ad aprire il cancello ma c'è un **lucchetto** che lo blocca.\n",
     2, {}},
    {"albero",
     "L'albero è secco e nodoso. Nel suo tronco vedi un ++buco++.",
     1,
     {&oggetti_cimitero[0]}},
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

/*  loc: locazione della quale va ritornata la descrizione
    buffer: buffer nel quale ritornare la descrizione
    
    Mette in buffer la descrizione della locazione, usando solo gli oggetti
    effettivamente presenti. */
char* descrizione_locazione(locazione *loc)
{
    /* TODO: capire come implementare questa funzione */
    /* int i;
    strcpy(buffer, loc->descrizione_iniziale);
    for(i = 0; i < loc->n_oggetti; i++) {
        strcat(buffer, loc->oggetti[i]->descrizione_locazione);
    } */
    static char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, loc->descrizione_iniziale);
    return tmp;
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

char* prendi_oggetto(struct sockaddr_in addr, char *nome_obj)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i]; 
        if(strcmp(nome_obj, obj->nome) != 0) {
            continue;
        }
        /* if(obj->is_bloccato) {
            return "Oggetto bloccato!\n";
        } */
        if(obj->is_preso) {
            return "Oggetto già preso!\n";
        }
        if(obj->has_enigma) {
        /* TODO: se c'è enigma attivarlo */
        }

        obj->is_preso = true;
        obj->addr_possessore = addr;
        return "Oggetto raccolto!\n";
    }
    return "Oggetto non trovato.\n";
}

char* utilizza_oggetti(struct sockaddr_in cl_addr, char *nome_obj1, char *nome_obj2)
{
    
    return "sium\n";
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