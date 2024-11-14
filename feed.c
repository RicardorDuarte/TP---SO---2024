#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include "feed.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

int is_invalid_command(const char *comando) {
    return strcmp(comando, "msg") != 0 &&
           strcmp(comando, "login") != 0 &&
           strcmp(comando, "subscribe") != 0 &&
           strcmp(comando, "topics") != 0 &&
           strcmp(comando, "unsubscribe") != 0 &&
           strcmp(comando, "exit") != 0;
}


int main()
{
    char comando[10],corpo[450];
    message msg;
    int fd_feed_fifo,sizeF;
    int fd_mngr_fifo,sizeM;

    sprintf(feedpipe_final,"FEED_FIFO[%d]",getpid());
    if(mkfifo(feedpipe_final,0666) == -1){
        if(errno == EEXIST){
            printf("named pipe com este pid ja existe\n");
        }
        printf("erro na criacao do named pipe\n");
        return 1;
    }

    
    do{
        //lê inputs do user até ser introduzido um comando
        do {
            printf("\ninsira um comando valido:");
            fgets(corpo, sizeof(corpo) - 1, stdin);
            sscanf(corpo,"%s %[^\n]", comando, corpo);
        }while(is_invalid_command(comando));
        //talvez criar isto numa função, menos clutter.
        //comparar comando com os diferentes tipos de comando, se diferente pedir outra vez
        printf("\ncomando: %s \t resto:%s\n ", comando,corpo);
        exit(1);
    }while(1);


    return 0;
}
