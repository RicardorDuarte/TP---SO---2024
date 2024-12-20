#include "feed.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"


int is_invalid_command(const char *comando) {
    return strcmp(comando, "msg") != 0 &&
           strcmp(comando, "subscribe") != 0 &&
           strcmp(comando, "topics") != 0 &&
           strcmp(comando, "unsubscribe") != 0 &&
           strcmp(comando, "exit") != 0 &&
           strcmp(comando, "users") != 0;
}


int main(int argc, char *argv[]) {
    char corpo[450];
    msg msg,msg2;
    int fd_mngr_fifo, fd_feed_fifo, res_sel;
    char feedpipe_final[50];
    usr user;
    fd_set fds;
    int size;

    if (argc < 2) {
        printf("Nome não introduzido, por favor introduza um nome.\n");
        exit(1);
    }

    strcpy(user.nome_utilizador, argv[1]);
    if (strlen(user.nome_utilizador) > 20) {
        printf("Nome demasiado grande, tente outra vez com um nome menor.\n");
        exit(1);
    }

    user.pid = getpid();
    printf("Bem vindo %s, com o pid: %d\n", user.nome_utilizador, user.pid);

    sprintf(feedpipe_final, "FEED_FIFO[%d]", getpid());
    if (mkfifo(feedpipe_final, 0666) == -1) {
        if (errno != EEXIST) {
            perror("Erro na criação do named pipe");
            return 1;
        }
        printf("Named pipe com este pid já existe.\n");
    }

    // abre o pipe do manager para escrita
    fd_mngr_fifo = open(ManPipe, O_WRONLY);
    if (fd_mngr_fifo == -1) {
        perror("Erro ao abrir o pipe do manager");
        unlink(feedpipe_final);
        return -1;
    }

    // Tentativa de login no manager
    msg.pid = user.pid;
    strcpy(msg.comando, "login");
    strcpy(msg.corpo, user.nome_utilizador);
    msg.fg1=0;

    if (write(fd_mngr_fifo, &msg, sizeof(msg)) == -1) {
        perror("Erro ao fazer login no servidor");
        close(fd_mngr_fifo);
        unlink(feedpipe_final);
        exit(1);
    }

    // abre o pipe do feed para leitura de respostas
    fd_feed_fifo = open(feedpipe_final, O_RDONLY );
    if (fd_feed_fifo == -1) {
        perror("Erro ao abrir pipe para receber resposta");
        close(fd_mngr_fifo);
        unlink(feedpipe_final);
        exit(1);
    }



    do {
        FD_ZERO(&fds);
        FD_SET(0, &fds); // leitura teclado
        FD_SET(fd_feed_fifo, &fds); // leitura de respostas vindas do manager

        // Select 
        res_sel = select(fd_feed_fifo + 1, &fds, NULL, NULL, NULL);
        if (res_sel == -1) {
            perror("Erro no select");
            printf("errno: %d\n", errno);
            close(fd_mngr_fifo);
            close(fd_feed_fifo);
            unlink(feedpipe_final);
            exit(1);
        }

        if (FD_ISSET(0, &fds)) { // teclado
            // limpa os campos para evitar lixo
            msg.corpo[0] = '\0';
            msg.comando[0] = '\0';
            msg.pid = getpid();

            fgets(corpo, sizeof(corpo) - 2, stdin);
            sscanf(corpo, "%s %[^\n]", msg.comando, msg.corpo);

            // validacao de comando
            while (is_invalid_command(msg.comando)) {
                printf("Comando inválido, tente novamente.\nCMD> ");
                fflush(stdout);
                fgets(corpo, sizeof(corpo) - 2, stdin);
                sscanf(corpo, "%s %[^\n]", msg.comando, msg.corpo);
            }

            

            if (strcmp(msg.comando, "exit") == 0) {
                strcpy(msg.corpo, "Eu vou sair!");
            }

            // mostra o comando introduzido
            printf("\nEnvia comando: %s\n", msg.comando);
            if(strcmp(msg.corpo,"\0")==0){}else{
                printf("Corpo da mensagem: %s\n", msg.corpo);}

            // Envia a mensagem para o manager
            if (write(fd_mngr_fifo, &msg, sizeof(msg)) == -1) {
                perror("Erro ao enviar mensagem para o servidor");
                close(fd_mngr_fifo);
                close(fd_feed_fifo);
                unlink(feedpipe_final);
                exit(1);
            }
        }

        if (FD_ISSET(fd_feed_fifo, &fds)) { // pipe
            size = read(fd_feed_fifo, &msg2, sizeof(msg2));
            if (size > 0) {
                if(msg2.fg1==1){
                printf("Foste expulso pelo administrador.\n");
                fflush(stdout);
                close(fd_feed_fifo);
                close(fd_mngr_fifo);
                unlink(feedpipe_final);
                exit(0);}

                printf("\nMensagem do Manager:\n%s\n", msg2.corpo);
                printf("CMD> ");
                fflush(stdout);
            } else {
                printf("O manager foi fechado, vou encerrar.\n");
                close(fd_feed_fifo);
                close(fd_mngr_fifo);
                unlink(feedpipe_final);
                exit(0);
            }
        }

        

        if (strcmp(msg.comando, "exit") == 0) {
            close(fd_feed_fifo);
            close(fd_mngr_fifo);
            unlink(feedpipe_final);
            exit(0);
        }

    } while (1);


    return 0;
}
