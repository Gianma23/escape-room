#include <stdio.h>
#include <string.h>
#include "gioco.h"
#include "scenari/cimitero.h"

static int scenario_scelto = -1;

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
    static char tmp[1024];
    memset(tmp, 0, sizeof(tmp));
    strcpy(tmp, loc->descrizione_iniziale);
    if(loc->n_oggetti == 0) {
        return tmp;
    }

    for(i = 0; i < loc->n_oggetti; i++) {
        if(loc->oggetti[i]->is_preso) {
            continue;
        }
        strcat(tmp, loc->oggetti[i]->descrizione_locazione);
    }
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
    printf("%d\n", scen->n_oggetti);
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i]; 
        if(strcmp(opzione, obj->nome) == 0) {
            return obj->is_bloccato ? obj->descrizione_bloccato : obj->descrizione_sbloccato;
        }
    }
    printf("ciao\n");
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
    int i;
    oggetto *obj = NULL;
    scenario *scen = scenari[scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        if(strcmp(nome_obj1, scen->oggetti[i].nome) != 0) {
            obj = &scen->oggetti[i]; 
            break;
        }
    }
    if(obj == NULL) {
        return "Oggetto non trovato.\n";
    }
    if(!obj->is_preso) {
        return "Non hai questo oggetto nel tuo inventario.\n";
    }
    if(!compara_addr(&cl_addr, &obj->addr_possessore)) {
        return "L'altro giocatore ha l'oggetto.\n";
    }
    
    for(i = 0; i < scen->n_utilizzi; i++) {
        utilizzo *util = &scen->utilizzi[i];
        if(strcmp(nome_obj1, util->primo) == 0 && strcmp(nome_obj2, util->secondo) == 0) {
            return "Utilizzo corretto.\n";
        }
    }
    
    return "sium\n";
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