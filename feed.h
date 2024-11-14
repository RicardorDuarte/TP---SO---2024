#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>


int is_invalid_command(const char *comando);


typedef struct User{
    char nome_utilizador[10];
    pid_t pid;
}usr;

typedef struct{
    char topico[10];
    int duracao;
    char texto[300];
}message;

