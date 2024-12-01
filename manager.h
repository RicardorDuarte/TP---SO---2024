#ifndef MANAGER_H
#define MANAGER_H
#include "utils.h"
#include "processoman.h"
#include "processocom.h"
typedef struct User usr, *pusr;
struct User{
	char nome_utilizador[20];
	pid_t pid;
};


typedef struct Mensagem msg,*pmsg;
struct Mensagem{
    char comando[15];
    char topico[15];
    char corpo[450];
    int duracao;
    //int npersistentes; este n persistentes n pode ficar aqui, isto so recebe msgs
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

#endif

