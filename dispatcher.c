#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "utility.h"
#include "auth.h"

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

char* comando_startgroup(int sock, char* opt)
{
    /* TODO: controllare che non sia già partita una stanza, poi 
    che non esista già un gruppo */
    socklen_t len = 0;
    gruppo_giocatori.attivo = true;
    getsockname(sock, (struct sockaddr*)gruppo_giocatori.giocatori[0], &len);
    return "Gruppo per la stanza creato con successo!";
}

char* comando_joingroup(int sock, char* opt)
{
    socklen_t len = 0;
    if(!gruppo_giocatori.attivo) {
        return "Gruppo non esistente, impossibile entrare.";
    }
    /* TODO: controllare che non ci siano già 2 giocatori(mettere un bool di completo) */
    getsockname(sock, (struct sockaddr*)gruppo_giocatori.giocatori[1], &len);
    return "Ingresso nel gruppo!\nIn attesa che il creatore inizi lo scenario.";
}

static bool is_partita_iniziata = false;

char* comando_start(int sock, char* opt)
{
    if(is_partita_iniziata) {
        return "Impossibile iniziare, il gioco è già iniziato!\n";
    }
}

char* comando_look(int sock, char* opt) 
{
return NULL;
}

char* comando_use(int sock, char* opt)
{
    return NULL;
}

char* comando_take(int sock, char* opt)
{
    return NULL;
}

char* comando_stop(int sock, char* opt)
{
return NULL;
}

static const comando lista_comandi_client[] = {
    {"register", handler_register},
    {"login", handler_login},
    {"startgroup", comando_startgroup},
    {"joingroup", comando_joingroup},
    {"start", comando_start},
    {"look", comando_look},
    {"use", comando_use},
    {"take", comando_take}
};
static const comando lista_comandi_server[] = {
    {"stop", comando_stop}
};

/*  socket:
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
    const int NUM_COMANDI = (sizeof(*lista_comandi)/sizeof(comando));
    
    sscanf(buffer, "%s", com);

    for(i = 0; i < NUM_COMANDI; i++) {
        if(strcmp(com, lista_comandi[i].nome) == 0) {
            printf("comando trovato.\n");
            buffer = strchr(buffer, ' ') + 1;

            char* risposta = lista_comandi[i].handler(socket, buffer);
            invia_messaggio(socket, risposta, "Errore invio risposta al comando");
        }
    }   
    printf("comando non trovato.\n");
}


