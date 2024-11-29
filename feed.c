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

int is_invalid_login(const char *comando, const char *corpo){
    return strcmp(comando,"login") == 0 && strcmp(corpo,"login");
}

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
        printf("\nNome demasiado grande, tente outra vez com um nome menor\n");
        exit(1);
    }

    user.pid = getpid();
    printf("%d, %s\n", user.pid, user.nome_utilizador);

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

    do {
        // Lê inputs do usuário até ser introduzido um comando válido
        do {
            printf("\nInsira um comando válido: ");
            fgets(corpo, sizeof(corpo) - 1, stdin);
            sscanf(corpo, "%s %[^\n]", comando, corpo);
        } while (is_invalid_command(comando));

        if (strcmp(comando, "exit") == 0) {
            unlink(feedpipe_final);
            exit(1);
        }

        // Print do que será enviado
        printf("\nEnviando comando: %s\n", comando);
        printf("Corpo da mensagem: %s\n", corpo);

        // Copiar os dados para a mensagem
        strcpy(msg.corpo, corpo);
        strcpy(msg.comando, comando);
        msg.pid = getpid();

        // Enviar mensagem para o pipe
        if (write(fd_mngr_fifo, &msg, sizeof(msg)) == -1) {
            perror("Erro ao enviar mensagem para o servidor");
            close(fd_mngr_fifo);
            unlink(feedpipe_final);
            exit(1);
        }

    } while (1);

    return 0;
}
