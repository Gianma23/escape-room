#include <stdio.h>
#include <string.h>
#include "gioco.h"
#include "scenari/cimitero.h"

static int scenario_scelto = -1;
static int token = 0;

static scenario *scenari[] = {
    &scenario_cimitero
    /* qui possono essere aggiunti altri scenari, 
    includendo la loro interfaccia */
};

/* FUNZIONI DI UTILITÀ ==================== */

/*  loc: locazione della quale va ritornata la descrizione
    buffer: buffer nel quale ritornare la descrizione
    
    Mette in buffer la descrizione della locazione, usando solo gli oggetti
    effettivamente presenti. */
char* descrizione_locazione(locazione *loc)
{
    int i;
    int n_obj = 0;
    static char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, loc->descrizione_iniziale);
    if(loc->n_oggetti == 0) {
        return tmp;
    }

    for(i = 0; i < loc->n_oggetti; i++) {
        if(loc->oggetti[i]->is_preso || loc->oggetti[i]->is_nascosto) {
            continue;
        }
        strcat(tmp, loc->oggetti[i]->descrizione_locazione);
    }
    return tmp;
}

oggetto* cerca_oggetto(char* obj)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        if(strcmp(obj, scen->oggetti[i].nome) == 0) {
            return &scen->oggetti[i];
        }
    }
    return NULL;
}

bool is_game_ended()
{
    return token == scenari[scenario_scelto]->n_token;
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

/*  opzione: locazione o oggetto del quale bisogna prendere la descrizione
    
    Ritorna la descrizione di opzione. Se è una locazione la sua descrizione
    contiene solo gli oggetti presenti, cioè non presi dal giocatore */
char* prendi_descrizione(char *opzione)
{
    int i;
    const scenario *scen = scenari[scenario_scelto];
    if(opzione == NULL) {
        return scen->descrizione;
    }

    oggetto *obj = cerca_oggetto(opzione);
    if(obj != NULL) {
        return obj->is_bloccato ? obj->descrizione_bloccato : obj->descrizione_sbloccato;
    }

    for(i = 0; i < scen->n_locazioni; i++) {
        if(strcmp(opzione, scen->locazioni[i].nome) == 0) {
            return descrizione_locazione(&scen->locazioni[i]);
        }
    }
    return "Oggetto/Locazione non trovata.\n";
}

/*  addr: indirizzo del giocatore che vuole prendere
    nome_obj: oggetto che vuole prendere
    
    Assegna l'oggetto nome_obj al giocatore di indirizzo addr, se non
    già preso. Se l'oggetto ha un enigma viene attivato. */
char* prendi_oggetto(struct sockaddr_in addr, char *nome_obj)
{
    int i;
    oggetto *obj;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        obj = &scen->oggetti[i]; 
        if(strcmp(nome_obj, obj->nome) != 0) {
            continue;
        }
        if(obj->is_bloccato) {
            return "Non puoi prendere questo oggetto!\n";
        }
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

/*  addr: indirizzo del giocatore
    nome_obj1: oggetto attivo che deve avere nell'inventario
    nome_obj2: oggetto passivo, può anche essere bloccato
    
    Utilizza l'oggetto nome_obj1 su nome_obj2, solo se nome_obj1 è nell'inventario
    del giocatore che chiama il comando e l'utilizzo è previsto dallo scenario  */
char* utilizza_oggetti(struct sockaddr_in addr, char *nome_obj1, char *nome_obj2)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    static char ret[256];

    oggetto *obj1 = cerca_oggetto(nome_obj1);
    if(obj1 == NULL) {
        return "Oggetto1 non trovato.\n";
    }
    oggetto *obj2 = cerca_oggetto(nome_obj2);
    if(obj2 == NULL) {
        return "Oggetto2 non trovato.\n";
    }
    if(!obj1->is_preso) {
        return "Non hai oggetto1 nel tuo inventario.\n";
    }
    if(!compara_addr(&addr, &obj1->addr_possessore)) {
        return "L'altro giocatore ha oggetto1.\n";
    }
    
    for(i = 0; i < scen->n_utilizzi; i++) {
        utilizzo *util = &scen->utilizzi[i];
        if(strcmp(nome_obj1, util->primo) == 0 && strcmp(nome_obj2, util->secondo) == 0) {
            /* sblocco l'oggetto nascosto se devo */
            if(util->oggetto_nascosto != NULL) {
                util->oggetto_nascosto->is_bloccato = false;
                util->oggetto_nascosto->is_nascosto = false;
            }
            /* blocco oggetto1 */
            obj1->is_bloccato = true;
            obj1->is_nascosto = true;
            obj1->is_preso = false;
            memset(&obj1->addr_possessore, 0, sizeof(obj1->addr_possessore));
            /* sblocco oggetto2 */
            obj2->is_bloccato = false;
            /* assegno un token e guardo se la partita è finita */
            token++;
            if(is_game_ended()) {
                /* TODO vittoria, resetta il gioco */
            }
            sprintf(ret, "Oggetto %s usato su %s con successo!\n", nome_obj1, nome_obj2);
            return ret;
        }
    }
    return "Utilizzo non previsto.\n";
}

char* prendi_inventario(struct sockaddr_in addr)
{
    int i;
    scenario *scen = scenari[scenario_scelto];
    static char ret[1024];
    
    memset(ret, 0, sizeof(ret));
    strcat(ret, "Inventario:\n");
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i];
        if(compara_addr(&addr, &obj->addr_possessore)) {
            strcat(ret, "- ");
            strcat(ret, obj->nome);
            strcat(ret, "\n");
        }
    }
    return ret;
}

/* Setta lo scenario scelto con id_scenario */
char* inizia_scenario(int id_scenario)
{
    if(id_scenario < 0 || id_scenario >= N_SCENARI) {
        return "Scenario non disponibile.\n";
    }
    scenario_scelto = id_scenario;
    return "Scenario iniziato, buona fortuna!\n";
}

/* Resetta lo scenario id_scenario ai valori di default */
bool reset_scenario(int id_scenario)
{
    if(id_scenario < 0 || id_scenario >= N_SCENARI) {
        return false;
    }
    /* TODO:  resettare per ogni oggetto le impostazioni di default */
    scenario_scelto = -1;
    return true;
}

/* Ritorna true se è stato scelto uno scenario e dunque è in corso una partita */
bool is_game_started()
{
    return scenario_scelto != -1;
}