#define _POSIX_C_SOURCE 200809L

#include "manager.h"

char feedpipe_final[100];

void acorda() {}

void abre_pipes(pid_t pid, int *fd_feed_p, msg *msg_env) {

    // cria o nome do pipe
    sprintf(feedpipe_final, "FEED_FIFO[%d]", pid);
 

    // pipe existe?
    if (access(feedpipe_final, F_OK) == -1) {
        perror("Pipe não encontrado");
        return; // sai
    }

    // Abre o pipe para escrita
    *fd_feed_p = open(feedpipe_final, O_WRONLY);
    if (*fd_feed_p == -1) {
        perror("Erro ao abrir o FIFO do feed");
        return;
    }

    // Envia a mensagem para o utilizador
    if (write(*fd_feed_p, msg_env, sizeof(*msg_env)) == -1) {
        perror("Erro ao enviar mensagem para o utilizador");
    } else {
        printf("\nMensagem enviada com sucesso para o utilizador no PID %d.\n", pid);
    }

    
}

void *decrementa(void *tdata){
    TData *thread_data = (TData *)tdata;
    man *manager = thread_data->manager;

    pthread_mutex_lock(thread_data->m);
    do {
        
        for (int i = 0; i < manager->ntopicos; i++) {
            if(manager->topicos->npersistentes != 0){
                for (int j = 0; j < manager->topicos->npersistentes; j++) {
                    manager->topicos->conteudo[j].duracao--;
                    if (manager->topicos->conteudo[j].duracao <= 0) {
                        printf("\nMensagem removida do tópico '%s': %s\n", 
                            manager->topicos->topico, manager->topicos->conteudo[j].corpo);

                        for (int k = j; k < manager->topicos->npersistentes - 1; k++) {
                            manager->topicos->conteudo[k] = manager->topicos->conteudo[k + 1];
                        }

                        manager->topicos->npersistentes--;
                        
                        j--;
                    }
                }
            }
        }

        sleep(1);
        pthread_mutex_unlock(thread_data->m);
    }while(thread_data->trinco == 0);
        
    return NULL;
}


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
            mensagemEnvia.fg1=0;
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
                char msgp[100];

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

                        abre_pipes(manager->utilizadores[i].pid, &fd_feed_pipe, &mensagemEnvia);
                        // Fecha o pipe após o envio
                        close(fd_feed_pipe);
                    }
                }
            }
                printf("\nCMD> ");
                fflush(stdout);
                
                 
            
            }
            else {

                mensagemEnvia.pid=0;
                processa_comando_feed(&mensagemEnvia, mensagemRecebida.corpo, mensagemRecebida.comando, mensagemRecebida.pid, manager, (void *)&user);
                strcpy(mensagemEnvia.comando, mensagemRecebida.comando);
                printf("Mensagem enviada:\n|Comando: %s\n", mensagemRecebida.comando);
                if (strcmp(mensagemEnvia.corpo, "\0") != 0) {
                printf("|Corpo: %s\n", mensagemEnvia.corpo);}
                printf("CMD>");
                fflush(stdout);

                abre_pipes(mensagemRecebida.pid,&fd_feed_pipe,&mensagemEnvia);
                
            }

            pthread_mutex_unlock(thread_data->m);
        } else {
            break; 
        }
    }while(thread_data->trinco == 0);
    close(fd_manager_pipe);
    return NULL;
}








int main() {
    man manager;
    pthread_t tid_pipe[2];
    TData thread_data[2];
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex,NULL);
    char comando[20];
    int fd_manager_pipe;

    manager.nusers = 0;
    manager.ntopicos = 0;


    //vai buscar mensagens persistentes ao ficheiro
    setenv("MSG_FICH", "persistentes.txt",1);
    load_mensagens_persistentes(&manager);


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
    thread_data[0].trinco = 0;

    // Cria a thread para ler do pipe
    if (pthread_create(&tid_pipe[0], NULL, ler_pipe, (void *)&thread_data[0]) != 0) {
        perror("Erro ao criar thread para ler do pipe");
        close(fd_manager_pipe);
        unlink(ManPipe);
        return -1;
    }

    // Prepara os dados para a thread tratar das mensagens persistentes
    thread_data[1].manager = &manager;
    thread_data[1].m = &mutex;
    thread_data[1].trinco = 0;

    // Cria a thread para decrementar a duracao das mensagens persistentes
    if (pthread_create(&tid_pipe[1], NULL, decrementa, (void *)&thread_data[1]) != 0) {
        perror("Erro ao criar thread para decrementar timer de mensagens");
        close(fd_manager_pipe);
        unlink(ManPipe);
        return -1;
    }


    // Loop de interação com o teclado do manager
    while(1){
        printf("CMD> ");
        scanf("%s", comando);
        if(strcmp(comando, "close") == 0){
            thread_data[0].trinco = 1;
            thread_data[1].trinco = 1;
            pthread_kill(tid_pipe[0],SIGUSR1);
            pthread_kill(tid_pipe[1],SIGUSR1);
            guarda_mensagens_persistentes(&manager);
            break;
        }
        
        // Processa comandos dados pelo admin
        pthread_mutex_lock(thread_data->m);
        processa_comando_manager(comando, &manager);
        pthread_mutex_unlock(thread_data->m);


    }

    // Espera a thread de leitura do pipe finalizar
    pthread_join(tid_pipe[0], NULL);
    pthread_join(tid_pipe[1], NULL);
    pthread_mutex_destroy(&mutex);

    // Finaliza a execução
    close(fd_manager_pipe);
    unlink(ManPipe);

    printf("FIM\n");
    return 0;
}
