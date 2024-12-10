#include "manager.h"
#define _POSIX_C_SOURCE 200809L

#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

void acorda() {}

void *ler_pipe(void *pdata) {
    TData *thread_data = (TData *)tdata;
    PipeData *pipe_data = (PipeData *)pdata;
    msg mensagemRecebida;
    int fd_manager_pipe = pipe_data->fd_manager_pipe, fd_feed_pipe;
    man *manager = pipe_data->manager;
    usr user;
    int sizeMan;
    msg mensagemEnvia;
    mensagemEnvia.corpo[0] = '\0';
    mensagemEnvia.comando[0] = '\0';
    mensagemEnvia.duracao = 0;
    mensagemEnvia.npersistentes=0;
    do {
        sizeMan = read(fd_manager_pipe, &mensagemRecebida, sizeof(mensagemRecebida));
        if (sizeMan > 0) {
            printf("Mensagem recebida do pipe:\n");
            printf("Comando: %s\n", mensagemRecebida.comando);
            printf("Corpo: %s\n", mensagemRecebida.corpo);
            printf("PID: %d\n", mensagemRecebida.pid);
            mensagemEnvia.pid=mensagemRecebida.pid;
            pthread_mutex_lock(thread_data->m);
            if (strcmp(mensagemRecebida.corpo, "sair") == 0) {
                close(fd_manager_pipe);
                unlink(ManPipe);
                exit(1);
            } else {
                processa_comando_feed(&mensagemEnvia, mensagemRecebida.corpo, mensagemRecebida.comando, mensagemRecebida.pid, manager, (void *)&user);
                strcpy(mensagemEnvia.comando, mensagemRecebida.comando);
                printf("\n%s, %s", mensagemEnvia.comando,mensagemEnvia.corpo);
                printf("CMD>");
                fflush(stdout);
                // Constrói o nome do FIFO do feed
                sprintf(feedpipe_final, "FEED_FIFO[%d]", mensagemRecebida.pid);

                // Verifica se o FIFO existe antes de abrir
                while (access(feedpipe_final, F_OK) == -1) {
                    usleep(1000); // Espera 1 ms
                }

                fd_feed_pipe = open(feedpipe_final, O_WRONLY);
                if (fd_feed_pipe == -1) {
                    perror("Erro ao abrir o FIFO do feed");
                }


                if (write(fd_feed_pipe, &mensagemEnvia, sizeof(msg)) == -1) {
                    perror("Erro ao enviar mensagem para o cliente");
                }
            
            }

            pthread_mutex_unlock(thread_data->m);
        } else {
            perror("Erro ao ler do pipe");
            break; // Encerra o loop em caso de erro.
        }
    }while(thread_data->trinco == 0);
    close(fd_manager_pipe);
    return NULL;
}

int main() {
    man manager;
    pthread_t tid_pipe[1];
    TData thread_data[1];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    char comando[20];

    int fd_manager_pipe;

    // Inicializacao do manager
    manager.nusers = 0;
    manager.ntopicos = 0;


    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = acorda;
    sigaction(SIGUSR1, &sa, NULL);


    

    if (access(ManPipe, F_OK) == -1) {  // Verifica se o FIFO já existe
        if (mkfifo(ManPipe, 0666) == -1) {
            perror("Erro ao criar o FIFO");
            return -1;
        } else {
            printf("FIFO '%s' criado com sucesso.\n",ManPipe);
        }
    } else {
        printf("FIFO '%s' já existe. Ignorando criação.\n", ManPipe);
    }

    fd_manager_pipe = open(ManPipe, O_RDWR);//aberto para leitura e escrita para nao ficar preso
    if (fd_manager_pipe == -1) {
        perror("Erro ao abrir o FIFO");
        return -1;
    }
    printf("FIFO '%s' aberto com sucesso.\n", ManPipe);

    // Prepara os dados para a thread ler o pipe
    thread_data[0].fd_manager_pipe = fd_manager_pipe;
    thread_data[0].manager = &manager;
    thread_data[0].m = &mutex;

    // Cria a thread para ler do pipe
    if (pthread_create(&tid_pipe[0], NULL, ler_pipe, (void *)&thread_data[0]) != 0) {
        perror("Erro ao criar thread para ler do pipe");
        close(fd_manager_pipe);
        unlink(ManPipe);
        return -1;
    }

    // Loop de interação com o usuário
    while(1){
        printf("CMD> ");
        scanf("%s", comando);
        if(strcmp(comando, "exit") == 0){
            thread_data[0].trinco = 1;
            pthread_kill(tid_pipe[0],SIGUSR1);
            break;
        }
        
        //// Processa o comando digitado pelo admin
        //processa_comando_manager(comando, &manager);

    }

    // Espera a thread de leitura do pipe finalizar
    pthread_join(tid_pipe[0], NULL);
    pthread_mutex_destroy(&mutex);

    // Finaliza a execução
    close(fd_manager_pipe);
    unlink(ManPipe);

    printf("FIM\n");
    return 0;
}
