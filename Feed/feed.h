#include "../Utils/utils.h"


int is_invalid_command(const char *comando);


typedef struct User{
    char nome_utilizador[20];
    pid_t pid;
}usr;

typedef struct{
    char comando[15];
    char topico[15];
    char corpo[450];
    int duracao;
    //int duracao;
    //char texto[300];
    pid_t pid;
}message;

