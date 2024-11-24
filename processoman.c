#include "processoman.h"

int list_user(pman m){
    if(m->nusers==0)return 0;
    else{
    printf("%d Users:\n",m->nusers);
        for(int i=0;i<m->nusers;i++)
            printf("%c\n",m->utilizador[i].nome_utilizador);
    return 1;
    }
};
int remove(pman m, const char username){
    if(m->nusers==0)return 0;
    else{
        for(int i=0;i<m->nusers;i++){
            if(strcmp(m->utilizador[i].nome_utilizador,username)==0){
                if(i==m->nusers-1){
                    m=NULL;
                    return 1;
                }else{
                m->utilizador[i]=m->utilizador[m->nusers-1];
                return 1;
                }
            }
        }
    }
};
int list_topics(pman m){
    if(m->ntopicos==0)return 0;
    else{
    printf("%d topicos:\n",m->ntopicos);
        for(int i=0;i<m->ntopicos;i++)
            printf("%c %d\n",m->topicos[i].topico,m->topicos->conteudo.npersistentes);
    return 1;
    }
};

int print_topic(pman m,const char topic){
    int i=0;
    if(m->ntopicos==0)return 0;
    else{
        printf("Conteudo do topico %c",topic);
        if(strcmp(m->topicos[i].topico,topic)==0){
            if(m->topicos->conteudo.duracao > 0)
                printf("%c\n",m->topicos->conteudo.corpo);
        }
        i++;
        if(i==m->ntopicos) return 1;
    }
};
int lock_topic(pman m,const char topic){
    if(m->ntopicos==0)return 0;
    else{
        for(int i=0;i<m->ntopicos;i++){
        if(strcmp(m->topicos->topico,topic)==0){
        if(m->topicos[i].lock==1)
            printf("topico ja bloqueado");
        else
            m->topicos->lock=1;
        }
    }
    }
};
int unlock_topic(pman m,const char topic){
if(m->ntopicos==0)return 0;
    else{
        for(int i=0;i<m->ntopicos;i++){
        if(strcmp(m->topicos->topico,topic)==0){
        if(m->topicos[i].lock==0)
            printf("topico ja desbloqueado");
        else
            m->topicos->lock=0;
        }
    }
    }
};
