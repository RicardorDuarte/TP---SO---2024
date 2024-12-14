#include "processocom.h"

void guarda_mensagens_persistentes(man *manager) {
    char *file_name = getenv("MSG_FICH");
    if (!file_name) {
        printf("Erro: variável de ambiente 'MSG_FICH' não definida.\nNao foi possivel retornar mensagens persistentes\n");
        return;
    }

    FILE *file = fopen(file_name, "w");
    if (!file) {
        perror("Erro ao abrir o arquivo para salvar mensagens persistentes");
        return;
    }

    // percorre todos os topicos e guarda as suas mensagens persistentes
    for (int i = 0; i < manager->ntopicos; i++) {
        tp *topic = &manager->topicos[i];
        for (int j = 0; j < topic->npersistentes; j++) {
            TMensagem *msg = &topic->conteudo[j];

            // descobre o username do autor de cada mensagem
            char autor[20] = "desconhecido"; // nome default em caso de alguma falha?? mais vale ter do que nao ter
            for (int k = 0; k < manager->nusers; k++) {
                if (manager->utilizadores[k].pid == topic->inscritos[j]) {
                    strcpy(autor,manager->utilizadores[k].nome_utilizador);
                    break;
                }
            }

            // guarda a mensagem no ficheiro
            fprintf(file, "%s %s %d %s\n", topic->topico, autor, msg->duracao, msg->corpo);
        }
    }

    fclose(file);
}


void load_mensagens_persistentes(man *manager) {
    char *file_name = getenv("MSG_FICH");
    if (!file_name) {
        printf("Erro: variável de ambiente 'MSG_FICH' não definida.\nNao foi possivel retornar mensagens persistentes\n");
        return;
    }

    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("Erro ao abrir o ficheiro de mensagens persistentes");
        return;
    }

    char linha[700];
    while (fgets(linha, sizeof(linha), file)) {
        char topico[30], autor[20], corpo[450];
        int duracao;

        // dividir a linha pelos campos apropriados
        if (sscanf(linha, "%s %s %d %[^\n]", topico, autor, &duracao, corpo) != 4) {
            printf("Erro ao analisar uma linha do ficheiro\n");
            continue;
        }

        // encontrar ou criar topico no manager caso este nao exista
        int index_topico = -1;
        for (int i = 0; i < manager->ntopicos; i++) {
            if (strcmp(manager->topicos[i].topico, topico) == 0) {
                index_topico = i;
                break;
            }
        }

        if (index_topico == -1) {
            // topico nao existe, temos de criar
            if (manager->ntopicos < MAXTOPICS) {
                strcpy(manager->topicos[manager->ntopicos].topico, topico);
                manager->topicos[manager->ntopicos].npersistentes = 0;
                index_topico = manager->ntopicos++;
            } else {
                printf("Erro: número máximo de tópicos atingido.\n");
                continue;
            }
        }

        // adiciona mensagem ao topico
        tp *topic = &manager->topicos[index_topico];
        if (topic->npersistentes < MAXMSGS) {
            //define os campos da mensagem a acidionar ao topico
            TMensagem *msg = &topic->conteudo[topic->npersistentes++];
            msg->duracao = duracao;
            strcpy(msg->corpo, corpo);
        } else {
            printf("Erro: número máximo de mensagens no tópico '%s' atingido.\n", topico);
        }
    }

    fclose(file);
}


void processa_comando_manager(char *comando, man *manager) {
    man *mngr = (man*)manager;
    char topic[20];
    int fd_feed_pipe;
    if (strcmp(comando,"users") == 0) {    //apenas para testes!!!! muito incompleto!!!!!
        if(mngr->nusers==0)printf("\nNao existem Users\n");
            else{
            printf("%d Users:\n",mngr->nusers);
                for(int i=0;i<mngr->nusers;i++)
                    printf("%s\n",mngr->utilizadores[i].nome_utilizador);
            }
    }
    else if (strcmp(comando, "remove") == 0) {
        char username[15];
        msg mensagemEnvia,mensagemREcebida;
        usr user;
        scanf("%s", username);
        mensagemEnvia.corpo[0] = '\0';
        mensagemEnvia.comando[0] = '\0';
        pid_t pid;
        
        for(int i=0;i<manager->nusers;i++){
            if(strcmp(manager->utilizadores[i].nome_utilizador,username)==0)
            pid=manager->utilizadores[i].pid;
        }
        strcpy(mensagemREcebida.comando,"exit\0");
        strcpy(mensagemREcebida.corpo,"\0");
        mensagemREcebida.pid=pid;
        processa_comando_feed(&mensagemEnvia,mensagemREcebida.corpo,mensagemREcebida.comando,pid,manager,(void*)&user);
        mensagemEnvia.fg1=1;
        abre_pipes(mensagemREcebida.pid,&fd_feed_pipe,&mensagemEnvia);
        close(fd_feed_pipe);
    }
    else if (strcmp(comando,"topics") == 0) {  
        if(mngr->ntopicos==0)printf("\nNao existem topicos\n");
        printf("%d topicos:\n",mngr->ntopicos);
            for(int i=0;i<mngr->ntopicos;i++)
                printf("|%s, locked: %d (0 -> nao, 1-> sim), %d mensagens persistentes\n", mngr->topicos[i].topico,mngr->topicos[i].lock, mngr->topicos[i].npersistentes);
        return;
    }
    else if (strcmp(comando, "show") == 0) {
        scanf("%s", topic);
       int i=0;
        if(mngr->ntopicos==0);
        else{
            printf("Conteudo do topico %s\n",topic);
            if(strcmp(mngr->topicos[i].topico,topic)==0){
                for(int j=0;j<mngr->topicos[i].npersistentes;j++){
                if(mngr->topicos->conteudo[j].duracao > 0)
                    printf("%s\n",mngr->topicos[i].conteudo[j].corpo);
                }
            }
            i++;
            if(i==mngr->ntopicos) return;
        }
    } 
    else if (strcmp(comando, "lock") == 0) {
        msg msgenvia, mensagemREcebida;
        usr user;
        char topic[100];  // Defina o tópico corretamente

        scanf("%s", topic);  // Certifique-se de que o tópico não ultrapasse o tamanho do buffer
        strcpy(msgenvia.comando, "msg");
        strcpy(mensagemREcebida.comando, "\0");
        strcpy(mensagemREcebida.corpo, "\0");

        pid_t pid;
        pid = manager->utilizadores[0].pid;  // Verifique se existem utilizadores antes de acessar

        msgenvia.fg1 = 0;

        if (mngr->ntopicos == 0) {
            printf("Nenhum tópico disponível.\n");
            return;
        } else {
            // Verifique se o tópico existe
            int topico_encontrado = 0;
            for (int i = 0; i < mngr->ntopicos; i++) {
                if (strcmp(mngr->topicos[i].topico, topic) == 0) {
                    topico_encontrado = 1;  // Marca que o tópico foi encontrado
                    if (mngr->topicos[i].lock == 1) {
                        printf("Tópico já bloqueado\n");
                        processa_comando_feed(&msgenvia, mensagemREcebida.corpo, mensagemREcebida.comando, pid, manager, (void*)&user);

                        for (int k = 0; k < manager->nusers; k++) {  // Inicializando k em 0
                            for (int j = 0; j < manager->utilizadores[k].nsubscritos; j++) {
                                if (strcmp(manager->utilizadores[k].subscrito[j].ntopico, topic) == 0) {
                                    msgenvia.pid = manager->utilizadores[k].subscrito[j].pid;
                                    sprintf(msgenvia.corpo, "Tópico %s já bloqueado\n", topic);
                                    abre_pipes(mensagemREcebida.pid, &fd_feed_pipe, &msgenvia);
                                }
                            }
                        }
                    } else {
                        mngr->topicos[i].lock = 1;
                        printf("Tópico %s bloqueado\n", topic);
                        processa_comando_feed(&msgenvia, mensagemREcebida.corpo, mensagemREcebida.comando, pid, manager, (void*)&user);

                        for (int k = 0; k < manager->nusers; k++) {  // Inicializando k em 0
                            for (int j = 0; j < manager->utilizadores[k].nsubscritos; j++) {
                                if (strcmp(manager->utilizadores[k].subscrito[j].ntopico, topic) == 0) {
                                    msgenvia.pid = manager->utilizadores[k].subscrito[j].pid;
                                    sprintf(msgenvia.corpo, "Tópico %s bloqueado\n", topic);
                                    abre_pipes(manager->utilizadores[k].subscrito[j].pid, &fd_feed_pipe, &msgenvia);
                                }
                            }
                        }
                    }
                    break;  // Se o tópico foi encontrado, saímos do loop
                }
            }

            // Se o tópico não for encontrado
            if (!topico_encontrado) {
                printf("Tópico %s não encontrado\n", topic);
            }
        }
    }
    else if (strcmp(comando, "unlock") == 0) {
            msg msgenvia, mensagemREcebida;
        usr user;
        char topic[100];  // Defina o tópico corretamente

        scanf("%s", topic);  // Certifique-se de que o tópico não ultrapasse o tamanho do buffer
        strcpy(msgenvia.comando, "msg");
        strcpy(mensagemREcebida.comando, "\0");
        strcpy(mensagemREcebida.corpo, "\0");

        pid_t pid;
        pid = manager->utilizadores[0].pid;  // Verifique se existem utilizadores antes de acessar

        msgenvia.fg1 = 0;

        if (mngr->ntopicos == 0) {
            printf("Nenhum tópico disponível.\n");
            return;
        } else {
            // Verifique se o tópico existe
            int topico_encontrado = 0;
            for (int i = 0; i < mngr->ntopicos; i++) {
                if (strcmp(mngr->topicos[i].topico, topic) == 0) {
                    topico_encontrado = 1;  // Marca que o tópico foi encontrado
                    if (mngr->topicos[i].lock == 0) {
                        printf("Tópico já desbloqueado\n");
                        processa_comando_feed(&msgenvia, mensagemREcebida.corpo, mensagemREcebida.comando, pid, manager, (void*)&user);

                        for (int k = 0; k < manager->nusers; k++) {  // Inicializando k em 0
                            for (int j = 0; j < manager->utilizadores[k].nsubscritos; j++) {
                                if (strcmp(manager->utilizadores[k].subscrito[j].ntopico, topic) == 0) {
                                    msgenvia.pid = manager->utilizadores[k].subscrito[j].pid;
                                    sprintf(msgenvia.corpo, "Tópico %s já desbloqueado\n", topic);
                                    abre_pipes(mensagemREcebida.pid, &fd_feed_pipe, &msgenvia);
                                }
                            }
                        }
                    } else {
                        mngr->topicos[i].lock = 0;
                        printf("Tópico %s desbloqueado\n", topic);
                        processa_comando_feed(&msgenvia, mensagemREcebida.corpo, mensagemREcebida.comando, pid, manager, (void*)&user);

                        for (int k = 0; k < manager->nusers; k++) {  // Inicializando k em 0
                            for (int j = 0; j < manager->utilizadores[k].nsubscritos; j++) {
                                if (strcmp(manager->utilizadores[k].subscrito[j].ntopico, topic) == 0) {
                                    msgenvia.pid = manager->utilizadores[k].subscrito[j].pid;
                                    sprintf(msgenvia.corpo, "Tópico %s desbloqueado\n", topic);
                                    abre_pipes(manager->utilizadores[k].subscrito[j].pid, &fd_feed_pipe, &msgenvia);
                                }
                            }
                        }
                    }
                    break;  // Se o tópico foi encontrado, saímos do loop
                }
            }

            // Se o tópico não for encontrado
            if (!topico_encontrado) {
                printf("Tópico %s não encontrado\n", topic);
            }
        }
    }
   
    else {
        printf("Comando desconhecido\n");
    }
}


void processa_comando_feed(msg *resposta,char *corpo, char *comando, int pid, void *manager, void *users) {
    man *mngr = (man*)manager;
    usr *user = (usr*)users;
    strcpy(user->nome_utilizador, corpo);  
    user->pid = pid; 

    if (strcmp(comando, "login") == 0) {
        int user_exists = 0;

        //ja existe?
        for (int i = 0; i < mngr->nusers; i++) {
            if (strcmp(mngr->utilizadores[i].nome_utilizador, corpo) == 0) {
                user_exists = 1;
                break;
            }
        }
        //mandar sinal para matar feed que existe
        if (!user_exists) {
            if (mngr->nusers < MAXUSERS) { 
                          
                
                mngr->utilizadores[mngr->nusers] = *user;
                mngr->nusers++;

                strcpy(resposta->corpo, "login feito\n"); 
               

            } else {
                printf("Impossivel adicionar mais users\n");
                strcpy(resposta->corpo, "Limite users, nao adicionado\n"); 
            }
        } else {
            printf("User '%s' ja existe.\n", corpo);
        }

        
    }
    
    if (strcmp(comando,"users") == 0) {    //apenas para testes!!!! este comando é so do manager!!!!!
        for (int i = 0; i < mngr->nusers && i < MAXUSERS; i++) {
                printf("User %d: %s\n", i + 1, mngr->utilizadores[i].nome_utilizador);
                sprintf(resposta->corpo,"user %d:%s\n",i+1,mngr->utilizadores[i].nome_utilizador);
        }
        
    }
    
    if (strcmp(comando, "exit") == 0) {

        int encontra = 0;
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizadores[i].pid == user->pid) { 
                encontra = 1;
                //empurra os elementos para a esquerda
                for (int j = i; j < mngr->nusers - 1; j++) {
                    mngr->utilizadores[j] = mngr->utilizadores[j + 1]; 
                }
                mngr->nusers--;
                printf("User com PID %d foi removido.\n", pid);
                sprintf(resposta->corpo,"\nUser com PID %d foi removido.\n", pid);
                if(pid!=0)
                resposta->pid=pid;
                
                break;
            }
        }
        if (!encontra) {
            printf("User com PID %d nao encontrado.\n", pid);
        }
    }
    
    if (strcmp(comando, "subscribe") == 0) {
        char topico[20];
        sscanf(corpo, "%s", topico);

        

        // Verifica se o tópico já existe no manager e se esta bloqueado
        int verificatopico = -1;
        for (int i = 0; i < mngr->ntopicos; i++) {
            if (strcmp(mngr->topicos[i].topico, topico) == 0) {
                verificatopico = i;
                break;
            }
        }

        // Se o tópico não existir, cria-o
        if (verificatopico == -1) {
            if (mngr->ntopicos < MAXTOPICS) {
                strcpy(mngr->topicos[mngr->ntopicos].topico, topico);
                mngr->topicos[mngr->ntopicos].npersistentes = 0;
                mngr->topicos[mngr->ntopicos].lock = 0;
                mngr->topicos[mngr->ntopicos].ninscritos = 0;  // Inicializa o número de inscritos
                mngr->ntopicos++;
                printf("Tópico '%s' adicionado ao manager.\n", topico);
            } else {
                printf("Manager cheio de tópicos, limite de %d alcançado.\n", MAXTOPICS);
                strcpy(resposta->corpo, "Limite de tópicos alcançado, não criado.\n");
                return;
            }
        }

        // Encontra o User com o PID
        int encontra = 0;
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizadores[i].pid == pid) {
                encontra = 1;

                // Verifica se o User já está subscrito ao tópico
                int alr_subbed = 0;
                for (int j = 0; j < mngr->utilizadores[i].nsubscritos; j++) {
                    if (strcmp(mngr->utilizadores[i].subscrito[j].ntopico, topico) == 0) {
                        alr_subbed = 1;
                        printf("User '%s' já está subscrito ao tópico: '%s'.\n", mngr->utilizadores[i].nome_utilizador, topico);
                        strcpy(resposta->corpo, "User já subscrito.\n");
                        break;
                    }
                }

                // Se o User não estiver subscrito, inscreve-o
                if (!alr_subbed) {
                    if (mngr->utilizadores[i].nsubscritos < MAXTOPICS) {
                        strcpy(mngr->utilizadores[i].subscrito[mngr->utilizadores[i].nsubscritos].ntopico, topico);
                        mngr->utilizadores[i].subscrito[mngr->utilizadores[i].nsubscritos].pid = pid;
                        mngr->utilizadores[i].nsubscritos++;

                        // Adiciona o PID do User à lista de inscritos no tópico
                        int novo = -1;
                        for (int j = 0; j < mngr->ntopicos; j++) {
                            if (strcmp(mngr->topicos[j].topico, topico) == 0) {
                                novo = j;
                                break;
                            }
                        }
                        if (novo != -1) {
                            mngr->topicos[novo].inscritos[mngr->topicos[novo].ninscritos] = pid;
                            mngr->topicos[novo].ninscritos++;
                        }


                        printf("User '%s' (PID %d) subscreveu ao tópico '%s'. (%d subscritos)\n", 
                        mngr->utilizadores[i].nome_utilizador, pid, topico, mngr->topicos[novo].ninscritos);
                        sprintf(resposta->corpo, "Adicionado ao tópico: %s\nMensagens:\n", topico);
                        
                        if(mngr->topicos[novo].ninscritos>0){
                            for(int k=0;k<mngr->ntopicos;k++){
                                if(strcmp(mngr->topicos[k].topico,topico)==0){
                                    for(int l=0;l<mngr->topicos[k].npersistentes;l++){
                                        if(mngr->topicos[k].conteudo[l].duracao >= 0)
                                        sprintf(resposta->corpo,"%s ('%ds restantes')\n",mngr->topicos[k].conteudo[l].corpo,mngr->topicos[k].conteudo[l].duracao);
                                        }
                                }
                            }
                        }
                    } else {
                        printf("User '%s' não pode subscrever a mais tópicos, limite alcançado.\n", mngr->utilizadores[i].nome_utilizador);
                        strcpy(resposta->corpo, "Limite de tópicos alcançado.\n");
                    }
                }
                break;
            }
        }

        if (!encontra) {
            // Caso o User não seja encontrado
            printf("User com PID %d não encontrado.\n", pid);
            strcpy(resposta->corpo, "Erro: User não encontrado.\n");
        }
    }


    if(strcmp(comando,"unsubscribe") == 0){
        char topico[20];
        sscanf(corpo,"%s",topico);

        int findusr = 0;

       // procura o user com o PID
       for (int i = 0; i < mngr->nusers; i++) {
           if (mngr->utilizadores[i].pid == pid) {
                findusr = 1;

                // esuer esta subscrito no topico?
                int findtopic = 0;
                for (int j = 0; j < MAXTOPICS; j++) {
                    if (strcmp(mngr->utilizadores[i].subscrito[j].ntopico, topico) == 0) {
                        findtopic = 1;

                        // Remove o tópico do array do User
                        printf("User '%s' deixou de subscrever ao tópico: '%s'.\n", mngr->utilizadores[i].nome_utilizador, topico);
                        sprintf(resposta->corpo, "User deixou de subscrever ao tópico: '%s'\n", topico);

                        // Reorganiza o array removendo o elemento
                        for (int k = j; k < mngr->utilizadores[i].nsubscritos - 1; k++) {
                            mngr->utilizadores[i].subscrito[k] = mngr->utilizadores[i].subscrito[k + 1];
                        }

                        // Reduz o número de tópicos subscritos e limpa a última posição
                        mngr->utilizadores[i].nsubscritos--;
                        mngr->utilizadores[i].subscrito[mngr->utilizadores[i].nsubscritos].ntopico[0] = '\0';//remove ultimo
                        mngr->utilizadores[i].subscrito[mngr->utilizadores[i].nsubscritos].pid = 0;

                        break;
                    }
                }

                if (!findtopic) {
                    printf("User '%s' nao e subscritor do topico '%s'.\n", mngr->utilizadores[i].nome_utilizador, topico);
                    sprintf(resposta->corpo, "User nao subscrito ao tópico: '%s'\n",topico);
                } else {
                    // verifica se o topico tem de ser removido
                    int temusers = 0;
                    for (int u = 0; u < mngr->nusers; u++) {
                        for (int t = 0; t < MAXTOPICS; t++) {
                            if (mngr->utilizadores[i].nsubscritos == 0) {
                                temusers = 1;
                                break;
                            }
                        }
                        if (temusers) break;
                    }

                    // remover topico  se este nao tiver subs
                    if (!temusers) {
                        for (int t = 0; t < mngr->ntopicos; t++) {
                            if (strcmp(mngr->topicos[t].topico, topico) == 0) {
                                for (int k = t; k < mngr->ntopicos - 1; k++) {
                                    strcpy(mngr->topicos[k].topico, mngr->topicos[k + 1].topico);
                                }
                                mngr->topicos[mngr->ntopicos - 1].topico[0] = '\0';
                                mngr->ntopicos--;
                                printf("O topico: '%s' foi removido do manager por falta de subscricoes\n", topico);
                                sprintf(resposta->corpo, "O topico: '%s' foi removido do manager por falta de subscricoes\n",topico);
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
        if (!findusr) { //so chega aqui se o managar for fechado e aberto e o feed continuar aberto (manager nao tem esse login)
                        //nao deve acontecer mas wtv
            printf("User %d nao encontrado, subscricao falhada\n", pid);
        }
    }

    if (strcmp(comando,"topics") == 0) {  
        if(mngr->ntopicos==0){printf("\nNão ha topicos\n");strcpy(resposta->corpo,"\nNão ha topicos");}
        else{
            
            for (int i = 0; i < mngr->ntopicos && i < MAXTOPICS; i++) {
                    printf("topico %d: %s, (%d subscritos)\n", i + 1, mngr->topicos[i].topico, mngr->topicos[i].ninscritos);
                    sprintf(resposta->corpo, "topico %d: %s, (%d subscritos)\n", i + 1, mngr->topicos[i].topico, mngr->topicos[i].ninscritos);
            }
        }
    }

    if (strcmp(comando, "msg") == 0) {
        char topico[20];
        int duracao;
        char msgp[200]; 

        // Recebe os parâmetros do corpo da mensagem
        sscanf(corpo, "%s %d %[^\n]", topico, &duracao, msgp);

        // Procura o tópico
        for (int i = 0; i < mngr->ntopicos && i < MAXTOPICS; i++) {
            if (strcmp(mngr->topicos[i].topico, topico) == 0) {

                if(mngr->topicos[i].lock==1){
                    printf("\nTopico bloqueado\n");
                    sprintf(resposta->corpo,"\nERROO||\nTopico bloquado\n");
                    break;
                }
                
                 if (duracao > 0) {
                    // Verifica se o número de mensagens não ultrapassou o limite
                    if (mngr->topicos[i].npersistentes < MAXMSGS) {
                        int j = mngr->topicos[i].npersistentes;

                        // Adiciona a mensagem ao tópico
                        strcpy(mngr->topicos[i].conteudo[j].corpo, msgp);
                        mngr->topicos[i].conteudo[j].duracao = duracao;
                        (mngr->topicos[i].npersistentes)++;

                        printf("\nMensagem adicionada ao tópico '%s', com duração de %d segundos:\n%s\n\n",
                            mngr->topicos[i].topico,
                            mngr->topicos[i].conteudo[j].duracao,
                            mngr->topicos[i].conteudo[j].corpo);

                        // Envia a resposta de sucesso
                        sprintf(resposta->corpo, "Conteúdo adicionado ao tópico %s:\n%s", mngr->topicos[i].topico, msgp);
                        break;
                        
                    } else {
                        printf("\nErro: limite de mensagens para o tópico '%s' foi atingido.\n", topico);
                        sprintf(resposta->corpo, "Erro: limite de mensagens no tópico %s atingido.", topico);
                        break;
                    }
                } else if(duracao==0){
                    printf(resposta->corpo, "\nMensagem nova no topico %s:\n%s", mngr->topicos[i].topico, msgp);
                    sprintf(resposta->corpo, "\nMensagem nova no topico %s:\n%s", mngr->topicos[i].topico, msgp);
                    break;
                }else {
                    printf("\nErro: duração inválida para a mensagem no tópico '%s'.\n", topico);
                    sprintf(resposta->corpo, "Erro: duração inválida para o tópico %s.", topico);
                }

                // Se o tópico não for encontrado
                 
            }
            sprintf(resposta->corpo, "Erro: tópico %s não encontrado.", topico);

        
        }

    }
}
