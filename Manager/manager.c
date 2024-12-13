#define _POSIX_C_SOURCE 200809L

#include "manager.h"

#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

void acorda() {}

void *ler_pipe(void *tdata) {
    TData *thread_data = (TData *)tdata;
    msg mensagemRecebida, mensagemEnvia;
    man *manager = thread_data->manager;
    usr user;

    int fd_manager_pipe = thread_data->fd_manager_pipe, fd_feed_pipe;
    int sizeMan;
    


    do {
        sizeMan = read(fd_manager_pipe, &mensagemRecebida, sizeof(mensagemRecebida));
        if (sizeMan > 0) {

            mensagemEnvia.corpo[0] = '\0';
            mensagemEnvia.comando[0] = '\0';
            printf("Mensagem recebida do pipe:\n");
            printf("Comando: %s\n", mensagemRecebida.comando);
            if(strcmp(mensagemRecebida.corpo,"\0")==0){}else{
            printf("Corpo: %s\n", mensagemRecebida.corpo);}
            printf("PID: %d\n", mensagemRecebida.pid);

            mensagemEnvia.pid=mensagemRecebida.pid;
            pthread_mutex_lock(thread_data->m);

            if(strcmp(mensagemRecebida.comando,"msg")==0){
                char topico[20];
                int duracao;
                char msgp[20];

                sscanf(mensagemRecebida.corpo, "%s %d %[^\n]", topico, &duracao, msgp);
                printf("\nMensagem recebida para adicionar ao topico '%s', com duracao de %d segundos:\n%s\n",topico,duracao,msgp);

                processa_comando_feed(&mensagemEnvia, mensagemRecebida.corpo, mensagemRecebida.comando, mensagemRecebida.pid, manager, (void *)&user);
                strcpy(mensagemEnvia.comando, mensagemRecebida.comando);
                printf("Número de usuários no manager: %d\n", manager->nusers);
                for (int i = 0; i < manager->nusers; i++) {
                    for (int j = 0; j < manager->utilizadores[i].nsubscritos; j++) {
                        // Verifica se o utilizador está subscrito ao tópico
                        if (strcmp(manager->utilizadores[i].subscrito[j].ntopico, topico) == 0) {
                            printf("Enviando mensagem para o tópico '%s'.\n", manager->utilizadores[i].subscrito[j].ntopico);

                            // Prepara o nome do pipe do feed usando o PID correto
                            sprintf(feedpipe_final, "FEED_FIFO[%d]", manager->utilizadores[i].pid);

                            // Verifica se o pipe existe
                            if (access(feedpipe_final, F_OK) == -1) {
                                perror("Pipe não encontrado");
                                continue; // Passa para o próximo utilizador
                            }

                            // Abre o pipe para escrita
                            fd_feed_pipe = open(feedpipe_final, O_WRONLY);
                            if (fd_feed_pipe == -1) {
                                perror("Erro ao abrir o FIFO do feed");
                                continue;
                            }

                            // Envia a mensagem para o utilizador
                            printf("\n\n%s\n\n", mensagemEnvia.corpo);
                            if (write(fd_feed_pipe, &mensagemEnvia, sizeof(mensagemEnvia)) == -1) {
                                perror("Erro ao enviar mensagem para o utilizador");
                            } else {
                                printf("Mensagem enviada com sucesso para o utilizador '%s' no tópico '%s'.\n", 
                                    manager->utilizadores[i].nome_utilizador, manager->utilizadores[i].subscrito[j].ntopico);
                            }
                        
                            close(fd_feed_pipe);
                        }
                    
                    }            
                }
                printf("CMD> ");
                fflush(stdout);
                
                 
            
            }
            else {


                processa_comando_feed(&mensagemEnvia, mensagemRecebida.corpo, mensagemRecebida.comando, mensagemRecebida.pid, manager, (void *)&user);
                strcpy(mensagemEnvia.comando, mensagemRecebida.comando);
                printf("Mensagem enviada:\n|Comando: %s\n", mensagemRecebida.comando);
                if(strcmp(mensagemEnvia.corpo,"\0")==0){}else{
                printf("|Corpo: %s\n", mensagemEnvia.corpo);}
                printf("CMD>");
                fflush(stdout);


                // Constrói o nome do pipe de cada feed
                sprintf(feedpipe_final, "FEED_FIFO[%d]", mensagemRecebida.pid);

                // Verifica se o pipe existe antes de abrir
                while (access(feedpipe_final, F_OK) == -1) {
                    sleep(0.1); // Espera 1 ms
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

    manager.nusers = 0;
    manager.ntopicos = 0;


    //setup sinal para matar a thread em caso do manager querer sair falta fechar os feeds
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

    fd_manager_pipe = open(ManPipe, O_RDWR); //aberto para leitura e escrita para nao ficar preso
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

    // Loop de interação com o teclado do manager
    while(1){
        printf("CMD> ");
        scanf("%s", comando);
        if(strcmp(comando, "exit") == 0){
            thread_data[0].trinco = 1;
            pthread_kill(tid_pipe[0],SIGUSR1);
            break;
        }
        
        // Processa comandos dados pelo admin
        pthread_mutex_lock(thread_data->m);
        processa_comando_manager(comando, &manager);
        pthread_mutex_unlock(thread_data->m);


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
