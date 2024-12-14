#ifndef UTILS_H
#define UTILS_H

// Definições de limites para usuários e tópicos
#define MAXUSERS 10
#define MAXTOPICS 20
#define MAXMSGS 50

//pipes
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"

// Inclusão de bibliotecas necessárias para o programa
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <fcntl.h>
#include <pthread.h>



typedef struct {
    char ntopico[30];
    pid_t pid;
} Sub;


typedef struct User usr;
struct User {
    char nome_utilizador[20];
    pid_t pid;
    int nsubscritos;
    Sub subscrito[MAXTOPICS]; 
};


typedef struct{
    int duracao;
    char corpo[450];
}TMensagem;

typedef struct Mensagem msg;
struct Mensagem{
    char comando[15];
    char corpo[650];
    pid_t pid;
    int fg1;
};

typedef struct Topic {
    char topico[20];
    TMensagem conteudo[MAXMSGS];
    int npersistentes;
    int lock;
    int ninscritos;  // Novo campo para armazenar o número de inscritos
    pid_t inscritos[MAXUSERS]; // Lista de PIDs de usuários inscritos
} tp;

typedef struct Manager man;
struct Manager{
    usr utilizadores[MAXUSERS];
    int nusers;
    int ntopicos;
    tp topicos[MAXTOPICS];
};

typedef struct {
    int fd_manager_pipe;
    int trinco;
    man *manager;
    pthread_mutex_t *m;
    usr *usr;
} TData;



#endif 
