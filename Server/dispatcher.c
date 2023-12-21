#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "auth.h"
#include "gioco.h"

static gruppo gruppo_giocatori = {};

static int scenario_scelto = 0;

char* handler_register(int sock, char *opt)
{
    return register_user(opt);
}

char* handler_login(int sock, char* opt)
{
    struct sockaddr_in cl_addr;
    socklen_t len = sizeof(cl_addr);
    memset(&cl_addr, 0, sizeof(cl_addr));
    getpeername(sock, (struct sockaddr*)&cl_addr, &len);
    return login_user(opt, cl_addr);
}

char* handler_startgroup(int sock, char* opt)
{
    socklen_t len = sizeof(struct sockaddr_in);
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(scenario_scelto != 0) {
        return "Impossibile creare il gruppo, è già in corso uno scenario.\n";
    }
    if(gruppo_giocatori.attivo) {
        return "Gruppo per giocare già creato.";
    }

    gruppo_giocatori.attivo = true;
    gruppo_giocatori.num_giocatori = 1;
    getpeername(sock, (struct sockaddr*)&gruppo_giocatori.indirizzi[0], &len);
    return "Gruppo per la stanza creato con successo!\n";
}

char* handler_joingroup(int sock, char* opt)
{
    socklen_t len = sizeof(struct sockaddr_in);
    struct sockaddr_in cl_addr;
    memset(&cl_addr, 0, len);

    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(!gruppo_giocatori.attivo) {
        return "Gruppo non esistente, impossibile entrare.\n";
    }
    if(gruppo_giocatori.indirizzi[1] != 0) {
        return "Gruppo già pieno!\n";
    }
    getpeername(sock, (struct sockaddr*)&cl_addr, &len);
    if(compara_addr(gruppo_giocatori.indirizzi[0], &cl_addr)) {
        return "Sei già nel gruppo!\n";
    }
    return "Ingresso nel gruppo! In attesa che il creatore inizi lo scenario.\n";
}

char* handler_start(int sock, char* opt)
{
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(scenario_scelto != 0) {
        return "Impossibile iniziare, il gioco è già iniziato!\n";
    }

    /* prendo i parametri */
    scenario_scelto = atoi(strtok(opt, " "));
    if(scenario_scelto <= 0 || scenario_scelto > N_SCENARI) {
        return "Scenario non disponibile.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }

    return "Scenario iniziato, buona fortuna!\n";
}

char* handler_look(int sock, char* opt) 
{
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    /* TODO: Sostituisci template con descrizione e invia al client */
    return NULL;
}

char* handler_use(int sock, char* opt)
{
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    return NULL;
}

char* handler_take(int sock, char* opt)
{
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    return NULL;
}

char* handler_end(int sock, char *opt)
{
    if(!is_logged(sock)) {
        return "Non sei loggato!\n";
    }
    return NULL;
}

char* handler_stop(int sock, char* opt)
{
return NULL;
}

static const comando lista_comandi_client[] = {
    {"register", handler_register, "<username> <password>\tregistra un nuovo account"},
    {"login", handler_login, "<username> <password>\taccedi con un account"},
    {"startgroup", handler_startgroup, "\tinizializza un gruppo di gioco"},
    {"joingroup", handler_joingroup, "\tentra in un gruppo se è stato creato"},
    {"start", handler_start, "<stanza>\tinizia uno scenario"},
    {"look", handler_look,"bohhh"},
    {"use", handler_use, "<username> <password>\tregistra un nuovo account"},
    {"take", handler_take, "<username> <password>\tregistra un nuovo account"},
    {"end", handler_end, "<username> <password>\tregistra un nuovo account"}
};

static const comando lista_comandi_server[] = {
    {"stop", handler_stop}
};

/*  socket: client al quale mandare la risposta
    buffer: stringa contente il comando
    soggetto: chi ha invocato il comando, client o server
    
    La funzione gestisce i comandi, invocando il giusto handler in base
    al comando e passandogli le opzioni. Dopodichè manda un messaggio
    di risposta a socket.*/
void command_dispatcher(int socket, char *buffer, char *soggetto)
{
    int i;
    char com[COMANDO_DIM];
    const comando *lista_comandi = strcmp(soggetto, "server") == 0 ? lista_comandi_server : lista_comandi_client;
    const int N_COMANDI = strcmp(soggetto, "server") == 0 ? N_COMANDI_SERVER : N_COMANDI_CLIENT;
    
    sscanf(buffer, "%s", com);

    for(i = 0; i < N_COMANDI; i++) {
        if(strcmp(com, lista_comandi[i].nome) == 0) {
            printf("comando trovato.\n");
            buffer = strchr(buffer, ' ') + 1;

            char* risposta = lista_comandi[i].handler(socket, buffer);
            invia_messaggio(socket, risposta, "Errore invio risposta al comando");
            return;
        }
    }   
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