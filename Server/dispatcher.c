#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include "dispatcher.h"
#include "auth.h"
#include "gioco.h"
#include "timer.h"

/* ===================== HANDLERS ===================== */
/*  cl_sock: socket del client che ha invocato il comando
    opt: opzioni del comando */

char* handler_register(int cl_sock, char *opt)
{
    return register_user(opt);
}

char* handler_login(int cl_sock, char* opt)
{
    return login_user(opt, cl_sock);
}

char* handler_startgroup(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
        return "Non sei loggato!\n";
    }
    if(is_game_started()) {
        return "Impossibile creare il gruppo, è già in corso uno scenario.\n";
    }
    if(strtok(opt, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return avvia_gruppo(cl_sock);
}

char* handler_joingroup(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
        return "Non sei loggato!\n";
    }
    return entra_gruppo(cl_sock);
}

char* handler_start(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
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
    start_timer(1200); /* 20 min di tempo per completare lo scenario */
    return inizia_scenario(cl_sock, id_scenario);
}

char* handler_look(int cl_sock, char* opt) 
{
    if(!is_logged(cl_sock)) {
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

char* handler_take(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
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
    return prendi_oggetto(cl_sock, oggetto);
}

char* handler_drop(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
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
    return lascia_oggetto(cl_sock, oggetto);
}

char* handler_use(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
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
    return utilizza_oggetti(cl_sock, obj1, obj2);
}

char* handler_objs(int cl_sock, char* opt)
{
    if(!is_logged(cl_sock)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato. Fare prima start <stanza>\n";
    }
    if(strtok(opt, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return prendi_inventario(cl_sock);
}

/*  termina lo scenario, resettandolo. Se è presente un gruppo, non
    viene eliminato */
char* handler_end(int cl_sock, char *opt)
{
    if(!is_logged(cl_sock)) {
        return "Non sei loggato!\n";
    }
    if(!is_game_started()) {
        return "Nessuno scenario iniziato.\n";
    }
    if(strtok(opt, " ") != NULL) {
        return "Troppi parametri.\n";
    }
    return termina_scenario(cl_sock);
}

char* handler_stop(int cl_sock, char* opt)
{
    printf("Chiusura server...\n");
    close(cl_sock);
    int sock_giocatore2 = prendi_giocatore2();
    if(sock_giocatore2 != -1) {
        close(sock_giocatore2);
    }
    exit(0);
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
    char risposta[512];
    char invio[1024];
    const int N_COMANDI = strcmp(soggetto, "server") == 0 ? N_COMANDI_SERVER : N_COMANDI_CLIENT;
    const comando *lista_comandi = strcmp(soggetto, "server") == 0 ? lista_comandi_server : lista_comandi_client;
    memset(risposta, 0, sizeof(risposta));
    memset(invio, 0, sizeof(invio));

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

            /* gioco finito */
            if(is_game_started() && (time = remaining_time()) <= 0) {
                strcpy(buffer, "Tempo scaduto! Il gioco è finito.\n");
                reset_scenario();
            }
            else {
                if(is_risposta_enigma(socket)) {
                    com = strtok(buffer, "\n");
                    strcpy(risposta, risolvi_enigma(com));
                }
                else {
                    strcpy(risposta, lista_comandi[i].handler(socket, opt));
                }
                /* header per quando c'è uno scenario iniziato */
                if(is_game_started()){
                    sprintf(invio, "- Rimanenti %d minuti e %d secondi.\n"
                                        "- Token rimasti da trovare: %d\n\n", time/60, time%60, token_rimasti());
                }
                strcat(invio, risposta);
            }

            invia_messaggio(socket, invio, "Errore invio risposta al comando");
            if(get_send_both()) {
                invia_messaggio(prendi_giocatore2(), invio, "Errore invio risposta al comando");
                set_send_both(false);
            }
            return;
        }
    }   
    printf("comando non trovato.\n");
    if(strcmp(soggetto, "client") == 0) {
        invia_messaggio(socket, "comando non trovato.\n", "Errore invio risposta comando non trovato");
    }
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