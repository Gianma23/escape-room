#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "auth.h"
#include "gioco.h"
#include "timer.h"

static gruppo giocatori = {};

/* ===================== HANDLERS ===================== */
/*  cl_addr: indirizzo del client che ha invocato il comando
    opt: opzioni del comando */

char* handler_register(struct sockaddr_in cl_addr, char *opt)
{
    return register_user(opt);
}

char* handler_login(struct sockaddr_in cl_addr, char* opt)
{
    return login_user(opt, cl_addr);
}

char* handler_startgroup(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(is_game_started()) {
        return "Impossibile creare il gruppo, è già in corso uno scenario.\n";
    }
    if(giocatori.attivo) {
        return "Gruppo per giocare già creato.\n";
    }

    giocatori.attivo = true;
    giocatori.num_giocatori++;
    giocatori.indirizzi[0] = cl_addr;
    return "Gruppo per la stanza creato con successo!\n";
}

char* handler_joingroup(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!giocatori.attivo) {
        return "Gruppo non esistente, impossibile entrare.\n";
    }
    if(giocatori.num_giocatori == MAX_GIOCATORI_GRUPPO) {
        return "Gruppo già pieno!\n";
    }
    if(compara_addr(&giocatori.indirizzi[0], &cl_addr)) {
        return "Sei già nel gruppo!\n";
    }

    giocatori.num_giocatori++;
    giocatori.indirizzi[giocatori.num_giocatori++] = cl_addr;
    return "Ingresso nel gruppo! In attesa che il creatore inizi lo scenario.\n";
}

char* handler_start(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(is_game_started()) {
        return "Impossibile iniziare, il gioco è già iniziato!\n";
    }

    char* str_scenario = strtok(opt, " ");
    if(str_scenario == NULL) {
        return "Specificare lo scenario da iniziare.\n";
    }
    int id_scenario = atoi(str_scenario);
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    start_timer(600); /* 10 min di tempo per completare lo scenario */
    return inizia_scenario(id_scenario);
}

char* handler_look(struct sockaddr_in cl_addr, char* opt) 
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }

    char* opzione = strtok(opt, " ");
    if(opzione != NULL && strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return prendi_descrizione(opzione);
}

char* handler_take(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }

    char* oggetto = strtok(opt, " ");
    if(oggetto == NULL) {
        return "Specificare l'oggetto da prendere.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return prendi_oggetto(cl_addr, oggetto);
}

char* handler_drop(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }

    char* oggetto = strtok(opt, " ");
    if(oggetto == NULL) {
        return "Specificare l'oggetto da lasciare.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return /* lascia_oggetto(cl_addr, oggetto) */"TODO\n";
}

char* handler_use(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    
    char* obj1 = strtok(opt, " ");
    if(obj1 == NULL) {
        return "Specificare almeno un oggetto da usare.\n";
    }
    char* obj2 = strtok(NULL, " ");
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return utilizza_oggetti(cl_addr, obj1, obj2);
}

char* handler_objs(struct sockaddr_in cl_addr, char* opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    return prendi_inventario(cl_addr);
}

char* handler_end(struct sockaddr_in cl_addr, char *opt)
{
    if(!is_logged(cl_addr)) {
        return "Non sei loggato!\n";
    }
    return NULL;
}

char* handler_stop(struct sockaddr_in cl_addr, char* opt)
{
return NULL;
}

/* COMANDI =============== */

static const comando lista_comandi_client[] = {
    {"register", handler_register, " <username> <password>\tregistra un nuovo account"},
    {"login", handler_login, " <username> <password>\taccedi con un account"},
    {"startgroup", handler_startgroup, "\t\t\tinizializza un gruppo di gioco"},
    {"joingroup", handler_joingroup, "\t\t\tentra in un gruppo se è stato creato"},
    {"start", handler_start, " <stanza>\t\t\tinizia uno scenario"},
    {"look", handler_look,"<oggetto>\t\t\tmostra la descrizione dell'oggetto, se oggetto non è\n"
                          "\t\t\t\t\tspecificato mostra la descrizione della stanza"},
    {"take", handler_take, "<oggetto>\t\t\tprende l'oggetto se non bloccato"},
    {"drop", handler_drop, "<oggetto>\t\t\tlascia l'oggetto se nell'inventario"},
    {"use", handler_use, "<oggetto1> <oggetto2>\tutilizza l'oggetto1 su oggetto2"},
    {"objs", handler_objs, "\t\t\t\tmostra il tuo inventario"},
    {"end", handler_end, "\t\t\t\ttermina la partita. Se è avviato un gruppo termina per tutti"}
};

static const comando lista_comandi_server[] = {
    {"stop", handler_stop}
};

/* IMPLEMENTAZIONE FUNZIONI INTERFACCIA ============ */

/*  socket: client al quale mandare la risposta
    buffer: stringa contente il comando
    soggetto: chi ha invocato il comando, client o server
    
    La funzione gestisce i comandi, invocando il giusto handler in base
    al comando e passandogli le opzioni. Dopodichè manda un messaggio
    di risposta a socket.*/
void command_dispatcher(int socket, char *buffer, char *soggetto)
{
    int i;
    int time;
    char *opt;
    char *com;
    char risposta[1024];
    const int N_COMANDI = strcmp(soggetto, "server") == 0 ? N_COMANDI_SERVER : N_COMANDI_CLIENT;
    const comando *lista_comandi = strcmp(soggetto, "server") == 0 ? lista_comandi_server : lista_comandi_client;
    memset(risposta, 0, sizeof(risposta));

    /* prendo indirizzo del client */
    struct sockaddr_in cl_addr;
    socklen_t len = sizeof(cl_addr);
    memset(&cl_addr, 0, sizeof(cl_addr));
    getpeername(socket, (struct sockaddr*)&cl_addr, &len);
    
    /* TODO rivedere controllo se enigma è attivo, contando il multiplayer */
    /* se il comando è arrivato da un giocatore con un enigma attivo, 
       l'input deve essere visto come la risposta all'enigma */
    if(is_risposta_enigma(cl_addr)) {
        com = strtok(buffer, "\n");
        strcat(risposta, risolvi_enigma(com));
        invia_messaggio(socket, risposta, "Errore invio risposta enigma");
        return;
    }
    
    /* cerco il comando nella lista dei comandi */
    com = strtok(buffer, " ");
    for(i = 0; i < N_COMANDI; i++) {
        if(strcmp(com, lista_comandi[i].nome) == 0) {
            printf("comando trovato.\n");
            opt = strtok(NULL, "\0");
            /* caso in cui non ci sono opzioni */
            if(opt == NULL) {
                opt = buffer;
                *opt = '\0';
            }

           /* TODO fare questo controllo sempre (portarlo fuori dal for) */ 
            if(is_game_started() && (time = remaining_time()) <= 0) {
                strcpy(risposta, "Tempo scaduto! Il gioco è finito.\n");
                /* TODO: reset gioco */
            }
            else if(is_game_started()){
                sprintf(risposta, "- Rimanenti %d minuti e %d secondi.\n"
                                  "- Token rimasti da trovare: %d\n\n", time/60, time%60, token_rimasti());
            }
            strcat(risposta, lista_comandi[i].handler(cl_addr, opt));

            invia_messaggio(socket, risposta, "Errore invio risposta al comando");
            return;
        }
    }   
    printf("comando non trovato.\n");
    invia_messaggio(socket, "comando non trovato.\n", "Errore invio risposta comando non trovato");
}

/* La funzione restituisce i comandi disponibili al client */
void comandi_client(char* buf)
{
    int i;
    strcpy(buf, "Lista comandi disponibili:\n");
    char tmp[1024];
    for(i = 0; i < N_COMANDI_CLIENT; i++) {
        sprintf(tmp, "\t%s%s\n", lista_comandi_client[i].nome, lista_comandi_client[i].descrizione);
        strcat(buf, tmp);
    }
}