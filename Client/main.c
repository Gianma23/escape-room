#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../utility.c"

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 4242
#define BUFFER_DIM 1024
#define COMANDO_DIM 64
#define DIVISORE "======================================================\n"

int main(int argc, char *argv[])
{
    int ret;
    int cl_sock;
    struct sockaddr_in sv_addr;
    in_port_t porta = htons(SERVER_PORT);
    char buffer[BUFFER_DIM];

    cl_sock = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sv_addr, 0, sizeof(sv_addr));
    sv_addr.sin_family = AF_INET;
    sv_addr.sin_port = porta;
    inet_pton(AF_INET, SERVER_IP, &sv_addr.sin_addr);

    ret = connect(cl_sock, (struct sockaddr*)&sv_addr, sizeof(sv_addr));
    if(ret < 0) {
        perror("errore di connessione");
        exit(1);
    }
    
    printf(DIVISORE);
    memset(buffer, 0, sizeof(buffer));
    ricevi_messaggio(cl_sock, buffer, "errore");
    printf("%s", buffer);
    printf(DIVISORE);
    memset(buffer, 0, sizeof(buffer));
    ricevi_messaggio(cl_sock, buffer, "errore");
    printf("%s", buffer);
    printf(DIVISORE);

    for(;;) {
        memset(buffer, 0, sizeof(buffer));
        fgets(buffer, COMANDO_DIM, stdin);
        buffer[strcspn(buffer, "\n")] = 0; /* elimino carattere \n messo dalla fgets */
        ret = invia_messaggio(cl_sock, buffer, "Errore in fase di invio riga di comando find.");
        printf("%d\n", ret);
        if(ret < 0) {
            /* errore di invio */
            printf("bug\n");
            exit(1);
        }

        memset(buffer, 0, sizeof(buffer));
        ret = ricevi_messaggio(cl_sock, buffer, "Errore in fase di ricezione");
        if(ret == 0) {
            printf("Connessione chiusa dal server.\n");
            exit(0);
        }
        printf("%s", buffer);
        printf(DIVISORE);
    } 
}