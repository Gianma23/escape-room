#include <stdio.h>
#include <string.h>
#include "gioco.h"
#include "scenari/cimitero.h"

#define MAX_GIOCATORI_GRUPPO 2
#define MAX_OGGETTI_INVENTARIO 2
#define N_SCENARI 1

typedef struct giocatore {
    int sock;
    int n_oggetti_presi;
} giocatore;

/* variabili globali */

static struct gioco {
    int scenario_scelto;
    enigma *enigma_attivato;
    int giocatore_enigma_attivato;
    int token;
    bool send_both;
    bool gruppo_attivo;
    int num_giocatori;
} gioco = {-1, NULL, -1, 0, false, false, 0};

giocatore giocatori[MAX_GIOCATORI_GRUPPO] = {{-1, 0}, {-1, 0}};

static scenario *scenari[] = {
    &scenario_cimitero
    /* qui possono essere aggiunti altri scenari, 
    includendo la loro interfaccia */
};

/* ==================== FUNZIONI DI UTILITÀ ==================== */

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
        n_obj++;
        strcat(tmp, loc->oggetti[i]->descrizione_locazione);
    }
    if(n_obj == 0) {
        strcat(tmp, "\n");
    }
    return tmp;
}

oggetto* cerca_oggetto(char* obj)
{
    int i;
    scenario *scen = scenari[gioco.scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        if(strcmp(obj, scen->oggetti[i].nome) == 0) {
            return &scen->oggetti[i];
        }
    }
    return NULL;
}

char* attiva_enigma(oggetto *obj, int sock)
{
    if(gioco.enigma_attivato != NULL) {
        return "L'altro giocatore sta risolvendo l'enigma!\n";
    }
    gioco.enigma_attivato = obj->enigma;
    gioco.giocatore_enigma_attivato = sock;

    static char tmp[512] = "Enigma attivato!\n";
    strcat(tmp, obj->enigma->descrizione);
    strcat(tmp, obj->is_bloccato ? obj->descrizione_bloccato : obj->descrizione_sbloccato);
    return tmp;
}

bool is_game_ended()
{
    return gioco.token == scenari[gioco.scenario_scelto]->n_token;
}

/* ============= IMPLEMENTAZIONE FUNZIONI HEADER ============= */

/*  buf: buffer nel quale scrivere.
    Ritorna gli scenari disponibili */
void prendi_scenari(char *buf)
{
    int i;
    strcpy(buf, "Scenari disponibili:\n");
    char tmp[256];
    for(i = 0; i < N_SCENARI; i++) {
        sprintf(tmp, "%d: %s. Presenti %d token.\n", i, scenari[i]->nome, scenari[i]->n_token);
        strcat(buf, tmp);
    }
}

/* Setta lo scenario scelto con id_scenario */
char* inizia_scenario(int sock, int id_scenario)
{
    if(id_scenario < 0 || id_scenario >= N_SCENARI) {
        return "Scenario non disponibile.\n";
    }
    if(gioco.gruppo_attivo && giocatori[0].sock != sock) {
        return "Solo l'owner del gruppo può avviare uno scenario.\n";
    }
    if(gioco.gruppo_attivo && gioco.num_giocatori < MAX_GIOCATORI_GRUPPO) {
        return "Gruppo avviato, lo scenario può partire solo se il gruppo è pieno.\n";
    }
    if(gioco.gruppo_attivo) {
        set_send_both(true);
    }
    gioco.scenario_scelto = id_scenario;
    return "Scenario iniziato, buona fortuna!\n";
}

char* termina_scenario(int sock)
{
    if(gioco.gruppo_attivo && giocatori[0].sock != sock) {
        return "Solo l'owner del gruppo può terminare uno scenario.\n";
    }
    if(gioco.gruppo_attivo) {
        set_send_both(true);
    }
    reset_scenario();
    return "Scenario terminato.\n";
}

/*  opzione: locazione o oggetto del quale bisogna prendere la descrizione
    
    Ritorna la descrizione di opzione. Se è una locazione la sua descrizione
    contiene solo gli oggetti presenti, cioè non presi dal giocatore */
char* prendi_descrizione(char *opzione)
{
    int i;
    const scenario *scen = scenari[gioco.scenario_scelto];
    if(opzione == NULL) {
        return scen->descrizione;
    }

    oggetto *obj = cerca_oggetto(opzione);
    if(obj != NULL && !obj->is_nascosto) {
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
char* prendi_oggetto(int sock, char *nome_obj)
{
    oggetto *obj = cerca_oggetto(nome_obj);
    if(obj == NULL || obj->is_nascosto) {
    return "Oggetto non trovato.\n";
    }

    if(obj->enigma != NULL && !obj->enigma->is_risolto && !obj->is_bloccato) {
        return attiva_enigma(obj, sock);
    }
    if(obj->is_bloccato || obj->enigma != NULL) {
        return "Non puoi prendere questo oggetto!\n";
    }
    if(obj->is_preso) {
        if(gioco.gruppo_attivo && obj->sock_possessore != sock) {
            return "L'altro giocatore ha già preso questo oggetto!\n";
        }
        return "Oggetto già preso!\n";
    }
    giocatore *g = (sock == giocatori[0].sock) ? &giocatori[0] : &giocatori[1];
    if(g->n_oggetti_presi >= MAX_OGGETTI_INVENTARIO) {
        return "Inventario pieno!\n";
    }
    g->n_oggetti_presi++;
    obj->is_preso = true;
    obj->sock_possessore = sock;
    return "Oggetto raccolto!\n";
}

char* lascia_oggetto(int sock, char *nome_obj)
{
    oggetto *obj = cerca_oggetto(nome_obj);
    if(obj == NULL || obj->is_nascosto) {
        return "Oggetto non trovato.\n";
    }
    if(!obj->is_preso) {
        return "Non hai questo oggetto nel tuo inventario.\n";
    }
    giocatore *g = (sock == giocatori[0].sock) ? &giocatori[0] : &giocatori[1];
    g->n_oggetti_presi--;
    obj->is_preso = true;
    obj->sock_possessore = sock;
    return "Oggetto lasciato dove lo avevi raccolto.\n";
}

/*  addr: indirizzo del giocatore
    nome_obj1: oggetto attivo che deve avere nell'inventario
    nome_obj2: oggetto passivo, può anche essere bloccato
    
    Utilizza l'oggetto nome_obj1 su nome_obj2, solo se nome_obj1 è nell'inventario
    del giocatore che chiama il comando e l'utilizzo è previsto dallo scenario  */
char* utilizza_oggetti(int sock, char *nome_obj1, char *nome_obj2)
{
    int i;
    scenario *scen = scenari[gioco.scenario_scelto];
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
    if(sock != obj1->sock_possessore) {
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
            obj1->is_nascosto = true;
            obj1->is_preso = false;
            obj1->sock_possessore = -1;
            /* sblocco oggetto2 */
            obj2->is_bloccato = false;
            /* assegno un token e guardo se la partita è finita */
            if(util->has_token) {
                gioco.token++; 
            }
            if(is_game_ended()) {
                if(gioco.gruppo_attivo) {
                    set_send_both(true);
                }
                reset_scenario();
                return "Lo scenario è stato completato, congratulazioni!\n";
            }
            strcat(ret, util->messaggio);
            return ret;
        }
    }
    return "Utilizzo non previsto.\n";
}

char* prendi_inventario(int sock)
{
    int i;
    scenario *scen = scenari[gioco.scenario_scelto];
    static char ret[256];
    
    memset(ret, 0, sizeof(ret));
    strcat(ret, "Inventario:\n");
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i];
        if(sock == obj->sock_possessore) {
            strcat(ret, "- ");
            strcat(ret, obj->nome);
            strcat(ret, "\n");
        }
    }
    return ret;
}

/*  risposta: risposta che ha dato il giocatore all'enigma
    ATTENZIONE: non controlla se c'è un enigma realmente attivo */
char* risolvi_enigma(char *risposta)
{
    static char tmp[256];
    if(strcmp(risposta, gioco.enigma_attivato->soluzione) != 0) {
        strcpy(tmp, "Soluzione errata.\n");
    }
    /* Soluzione corretta */
    else {
        gioco.enigma_attivato->oggetto_nascosto->is_nascosto = false;
        gioco.enigma_attivato->is_risolto = true;
        gioco.token++;
        if(is_game_ended()) {
                if(gioco.gruppo_attivo) {
                    set_send_both(true);
                }
                reset_scenario();
                return "Lo scenario è stato completato, congratulazioni!\n";
            }
        strcpy(tmp, gioco.enigma_attivato->messaggio_risoluzione);
    }
    gioco.enigma_attivato = NULL;
    gioco.giocatore_enigma_attivato = -1;
    return tmp;
}

int token_rimasti()
{
    return scenari[gioco.scenario_scelto]->n_token - gioco.token;
}

/* Resetta lo scenario id_scenario ai valori di default */
bool reset_scenario()
{
    if(gioco.scenario_scelto == -1) {
        return false;
    }
    if(gioco.gruppo_attivo) {
        set_send_both(true);
    }
    int i;
    const scenario *scen = scenari[gioco.scenario_scelto];
    for(i = 0; i < scen->n_oggetti; i++) {
        oggetto *obj = &scen->oggetti[i];
        if(i < scen->n_bloccati) {
            obj->is_bloccato = true;
            obj->is_nascosto = false;
        }
        else if(i < (scen->n_bloccati + scen->n_nascosti)) {
            obj->is_bloccato = false;
            obj->is_nascosto = true;
        }
        else {
            obj->is_bloccato = false;
            obj->is_nascosto = false;
        }
        obj->is_preso = false;
        obj->sock_possessore = -1;
    }
    for(i = 0; i < scen->n_enigmi; i++) {
        scen->enigmi[i].is_risolto = false;
    }
    /* reset variabili gioco */
    gioco.scenario_scelto = -1;
    gioco.token = 0;
    return true;
}

bool is_risposta_enigma(int sock)
{
    return gioco.enigma_attivato != NULL && sock == gioco.giocatore_enigma_attivato;
}

/* Ritorna true se è stato scelto uno scenario e dunque è in corso una partita */
bool is_game_started()
{
    return gioco.scenario_scelto != -1;
}

bool get_send_both()
{
    return gioco.send_both;
}

void set_send_both(bool value)
{
    gioco.send_both = value;
}

/* INTERFACCIA GRUPPO */

char* avvia_gruppo(int sock)
{
    if(gioco.gruppo_attivo) {
        return "Gruppo per giocare già creato.\n";
    }

    gioco.gruppo_attivo = true;
    gioco.num_giocatori++;
    giocatori[0].sock = sock;
    return "Gruppo per la stanza creato con successo!\n";
}

char* entra_gruppo(int sock)
{
    if(!gioco.gruppo_attivo) {
        return "Gruppo non esistente, impossibile entrare.\n";
    }
    if(gioco.num_giocatori == MAX_GIOCATORI_GRUPPO) {
        return "Gruppo già pieno!\n";
    }
    if(sock == giocatori[0].sock) {
        return "Sei già nel gruppo!\n";
    }
    giocatori[gioco.num_giocatori++].sock = sock;
    set_send_both(true);
    return "Gruppo completo! In attesa che il creatore inizi lo scenario.\n";
}

char* elimina_gruppo()
{
    if(!gioco.gruppo_attivo) {
        return "Gruppo non esistente, impossibile eliminare.\n";
    }
    gioco.gruppo_attivo = false;
    giocatori[0].sock = giocatori[1].sock = -1;
    gioco.num_giocatori = 0;
    return "Gruppo eliminato con successo.\n";
}

int prendi_giocatore2()
{
    return giocatori[1].sock;
}

/* Ritorna il socket dell'altro giocatore, altrimenti -2
    se sock non è di un giocatore presente. */
int prendi_altro_giocatore(int sock)
{
    if(giocatori[0].sock == sock) {
        return giocatori[1].sock;
    }
    if(giocatori[1].sock == sock) {
        return giocatori[0].sock;
    }
    return -2;
}