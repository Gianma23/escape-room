#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "auth.h"
#include "gioco.h"

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
        return "Gruppo per giocare già creato.";
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

    int id_scenario = atoi(strtok(opt, " "));
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
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
    {"startgroup", handler_startgroup, "\t\tinizializza un gruppo di gioco"},
    {"joingroup", handler_joingroup, "\t\tentra in un gruppo se è stato creato"},
    {"start", handler_start, " <stanza>\tinizia uno scenario"},
    {"look", handler_look,"bohhh"},
    {"take", handler_take, "<username> <password>\tregistra un nuovo account"},
    {"use", handler_use, "<username> <password>\tregistra un nuovo account"},
    {"end", handler_end, "<username> <password>\tregistra un nuovo account"}
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
    char *opt;
    char *com;
    const comando *lista_comandi = strcmp(soggetto, "server") == 0 ? lista_comandi_server : lista_comandi_client;
    const int N_COMANDI = strcmp(soggetto, "server") == 0 ? N_COMANDI_SERVER : N_COMANDI_CLIENT;
    
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
            /* prendo indirizzo del client */
            struct sockaddr_in cl_addr;
            socklen_t len = sizeof(cl_addr);
            memset(&cl_addr, 0, sizeof(cl_addr));
            getpeername(socket, (struct sockaddr*)&cl_addr, &len);
            /* chiamo l'handler del comando */
            char* risposta = lista_comandi[i].handler(cl_addr, opt);
            invia_messaggio(socket, risposta, "Errore invio risposta al comando");
            return;
        }
    }   
    /* TODO: riconoscimento comando simile */
    printf("comando non trovato.\n");
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