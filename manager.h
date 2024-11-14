#ifndef MANAGER_H
#define MANAGER_H
#define MAXUSERS 10
#define MAXTOPICS 20

typedef struct User usr, *pusr;
struct User{
	char nome_utilizador[10];
	pid_t pid;
};


typedef struct Mensagem msg,*pmsg;
struct Mensagem{
	char topico[10];
    int duracao;
    char texto[300];
	pid_t pid;
};

typedef struct Topic tp,*ptp;
struct Topic{
	char topico[20];
	msg conteudo;
};

typedef struct Manager man,*pman;
struct Manager{
usr utilizador[MAXUSERS];
int nusers;
int ntopicos;
tp topicos[MAXTOPICS]
};

list_user(pman manager);
remove(pman manager, const char username);
list_topics(pman manager);
print_topic(pman manager,const char topic);
lock_topic(pman manager,const char topic);
unlock_topic(pman manager,const char topic);

#endif

