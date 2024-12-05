#ifndef UTILS_H
#define UTILS_H

// Definições de limites para usuários e tópicos
#define MAXUSERS 10
#define MAXTOPICS 20

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
#include <fcntl.h>
#include <pthread.h>

typedef struct User usr, *pusr;
struct User{
	char nome_utilizador[20];
	pid_t pid;
    char subscrito[MAXTOPICS];
};


typedef struct Mensagem msg,*pmsg;
struct Mensagem{
    char comando[15];
    char topico[15];
    char corpo[450];
    int duracao;
    int npersistentes;//este n persistentes n pode ficar aqui, isto so recebe msgs
	pid_t pid;
};

typedef struct Topic tp,*ptp;
struct Topic{
	char topico[20];
	msg conteudo;
    int lock;
};

typedef struct Manager man,*pman;
struct Manager{
    usr utilizador[MAXUSERS];
    int nusers;
    int ntopicos;
    tp topicos[MAXTOPICS];
};

typedef struct {
    int fd_manager_pipe;
    man *manager;
} PipeData;



#endif // UTILS_H
