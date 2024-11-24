#include "feed.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

int is_invalid_command(const char *comando) {
    return strcmp(comando, "msg") != 0 &&
           strcmp(comando, "subscribe") != 0 &&
           strcmp(comando, "topics") != 0 &&
           strcmp(comando, "unsubscribe") != 0 &&
           strcmp(comando, "exit") != 0;
// verifica se o comando introduzido na consola é um comando válido
}

int is_invalid_login(const char *comando, const char *corpo){
    return strcmp(comando,"login") == 0 && strcmp(corpo,"login");
}


int main(int argc, char * argv[])
{
    char comando[10],corpo[450];
    message msg;
    int fd_mngr_fifo,sizeM;
    usr user;
    if(!argv[1]){
        printf("Nome nao introduzido, por favor introduza um nome.\n");
        exit(1);
    }
    strcpy(user.nome_utilizador,argv[1]);
    if(strlen(user.nome_utilizador) > 20){
        printf("\nNome demasiado grande, tente outra vez com um nome menor\n");
        exit(1);
    }
    user.pid = getpid();
    printf("%d, %s\n",user.pid, user.nome_utilizador);
    //fazer aqui averificação de possibilidade de adicionar utilizador no manager
    //melhor fazer com sinais, 3 situações: sucesso - falha por nome igual - falha por max

    sprintf(feedpipe_final,"FEED_FIFO[%d]",getpid());
    if(mkfifo(feedpipe_final,0666) == -1){
        if(errno == EEXIST){
            printf("named pipe com este pid ja existe\n");
        }
        printf("erro na criacao do named pipe\n");
        return 1;
    }

    fd_mngr_fifo = open (ManPipe,O_WRONLY);
        if (fd_mngr_fifo == -1){
            printf("Erro a abrir o pipe do manager, ou pipe nao existe\n");
            unlink(feedpipe_final);
            return -1;
        }

    do{

        //lê inputs do user até ser introduzido um comando
        do {
            printf("\ninsira um comando valido:");
            fgets(corpo, sizeof(corpo) - 1, stdin);
            sscanf(corpo,"%s %[^\n]", comando, corpo);
        }while(is_invalid_command(comando));

        if(strcmp(comando,"exit") == 0){
            unlink(feedpipe_final);
            exit(1);
        }

        printf("\ncomando: %s \t resto:%s\n ", comando,corpo);
        strcpy(msg.corpo,corpo);
        strcpy(msg.comando,comando);
        msg.pid = getpid();
        if(write(fd_mngr_fifo,&msg,sizeof(msg)) == -1){
            perror("Erro ao enviar mensagem para o servidor");
            close(fd_mngr_fifo);
            unlink(feedpipe_final);
            exit(1);
        }

        
    }while(1);


    return 0;
}
