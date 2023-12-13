#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "utility.h"
#include "auth.h"
#include "gioco.h"

#define COMANDO_DIM 64

char* handler_register(int sock, char *opt)
{
    return register_user(opt);
}

char* handler_login(int sock, char* opt)
{
    return login_user(opt);
}

static gruppo gruppo_giocatori = {
    false,
    {NULL, NULL}
};

static int scenario_scelto = 0;

char* handler_startgroup(int sock, char* opt)
{
    /* TODO: controllare che non sia già partita una stanza, poi 
    che non esista già un gruppo */
    if(scenario_scelto != 0) {
        return "Impossibile creare il gruppo, è già in corso uno scenario.\n";
    }
    if(gruppo_giocatori.attivo) {
        return "Gruppo per giocare già creato.";
    }
    socklen_t len = 0;
    gruppo_giocatori.attivo = true;
    getsockname(sock, (struct sockaddr*)gruppo_giocatori.giocatori[0], &len);
    return "Gruppo per la stanza creato con successo!\n";
}

char* handler_joingroup(int sock, char* opt)
{
    socklen_t len = 0;
    if(!gruppo_giocatori.attivo) {
        return "Gruppo non esistente, impossibile entrare.\n";
    }
    /* TODO: controllare che non ci siano già 2 giocatori(mettere un bool di completo) */
    getsockname(sock, (struct sockaddr*)gruppo_giocatori.giocatori[1], &len);
    return "Ingresso nel gruppo! In attesa che il creatore inizi lo scenario.\n";
}

char* handler_start(int sock, char* opt)
{
    if(scenario_scelto != 0) {
        return "Impossibile iniziare, il gioco è già iniziato!\n";
    }
    /* TODO: controllare opt */
    scenario_scelto = atoi(strtok(opt, " "));
    if(scenario_scelto == 0) {
        return "Errore in fase di start scenario.\n";
    }
    if(strtok(NULL, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    /* allocare scenario, decidere se nello stack o nello heap */
    return "Scenario iniziato, buona fortuna!\n";
}

char* handler_look(int sock, char* opt) 
{
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    /* Sostituisci template con descrizione e invia al client */
    return NULL;
}

char* handler_use(int sock, char* opt)
{
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    return NULL;
}

char* handler_take(int sock, char* opt)
{
    if(scenario_scelto == 0) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    return NULL;
}

char* handler_end(int sock, char *opt)
{
    return NULL;
}

char* handler_stop(int sock, char* opt)
{
return NULL;
}

static const comando lista_comandi_client[] = {
    {"register", handler_register},
    {"login", handler_login},
    {"startgroup", handler_startgroup},
    {"joingroup", handler_joingroup},
    {"start", handler_start},
    {"look", handler_look},
    {"use", handler_use},
    {"take", handler_take},
    {"end", handler_end}
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
void command_handler(int socket, char *buffer, char *soggetto)
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


