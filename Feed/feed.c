#include "feed.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

int is_invalid_command(const char *comando) {
    return strcmp(comando, "msg") != 0 &&
           strcmp(comando, "subscribe") != 0 &&
           strcmp(comando, "topics") != 0 &&
           strcmp(comando, "unsubscribe") != 0 &&
           strcmp(comando, "exit") != 0 &&
           strcmp(comando, "users") != 0;
}

//int is_invalid_login(const char *comando, const char *corpo){
//    return strcmp(comando,"login") == 0 && strcmp(corpo,"login");
//}

int main(int argc, char * argv[])
{
    char comando[10], corpo[450];
    message msg;
    int fd_mngr_fifo, sizeM;
    usr user;

    if (!argv[1]) {
        printf("Nome não introduzido, por favor introduza um nome.\n");
        exit(1);
    }

    strcpy(user.nome_utilizador, argv[1]);
    if (strlen(user.nome_utilizador) > 20) {
        printf("Nome demasiado grande, tente outra vez com um nome menor\n");
        exit(1);
    }

    user.pid = getpid();
    printf("Bem vindo %s, com o pid: %d\n",  user.nome_utilizador,user.pid);

    // Criando o pipe com o pid do feed
    sprintf(feedpipe_final, "FEED_FIFO[%d]", getpid());
    if (mkfifo(feedpipe_final, 0666) == -1) {
        if (errno == EEXIST) {
            printf("Named pipe com este pid já existe\n");
        }
        printf("Erro na criação do named pipe\n");
        return 1;
    }

    // Abrindo o pipe do manager para escrita
    fd_mngr_fifo = open(ManPipe, O_WRONLY);
    if (fd_mngr_fifo == -1) {
        printf("Erro ao abrir o pipe do manager, ou pipe não existe\n");
        unlink(feedpipe_final);
        return -1;
    }
    //tenta login no manager
    msg.pid = getpid();
    strcpy(msg.comando,"login");
    strcpy(msg.corpo,user.nome_utilizador);
        if (write(fd_mngr_fifo, &msg, sizeof(msg)) == -1) {
            perror("Erro ao fazer login no servidor");
            close(fd_mngr_fifo);
            unlink(feedpipe_final);
            exit(1);
        }
    do {
        //limpa os campos a cada iteracao para evitar enviar a mensagem anterior
        msg.corpo[0] = '\0';
        msg.comando[0] = '\0';
        msg.duracao = 0;
        // Lê inputs do usuário até ser introduzido um comando válido
        do {
            printf("Insira um comando válido: ");
            fgets(corpo, sizeof(corpo) - 2, stdin);
            sscanf(corpo, "%s %[^\n]", msg.comando, msg.corpo);
        } while (is_invalid_command(msg.comando));


        // Print do que será enviado
        printf("\nEnviando comando: %s\n", msg.comando);
        printf("Corpo da mensagem: %s\n", msg.corpo);
        

        if (strcmp(msg.comando, "exit") == 0) {
            strcpy(msg.corpo, "Eu vou sair!");
        }
        
        // Enviar mensagem para o pipe
        if (write(fd_mngr_fifo, &msg, sizeof(msg)) == -1) {
            perror("Erro ao enviar mensagem para o servidor");
            close(fd_mngr_fifo);
            unlink(feedpipe_final);
            exit(1);
        }
        
        if (strcmp(msg.comando, "exit") == 0) {
            unlink(feedpipe_final);
            exit(1);
        }
        

    } while (1);

    return 0;
}
