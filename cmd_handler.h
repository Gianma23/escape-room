typedef struct comando {
    char *nome;
    void (*handler)();
} comando;

void command_handler(char *, char *);