#include "processocom.h"

void processa_comando_manager(char *comando, man *manager) {
    man *mngr = (man*)manager;
    //usr *user = (usr*)users;
    char topic[20];

    if (strcmp(comando,"users") == 0) {    //apenas para testes!!!! muito incompleto!!!!!
        for (int i = 0; i < mngr->nusers && i < MAXUSERS; i++) {
                printf("User %d: %s\n", i + 1, mngr->utilizadores[i].nome_utilizador);
        }
    }
    //else if (strcmp(comando, "remove") == 0) {
    //    scanf("%s", username);    
    //    //remover(manager, username);
    //}
    else if (strcmp(comando,"topics") == 0) {  
        for (int i = 0; i < mngr->ntopicos && i < MAXTOPICS; i++) {
                printf("topico %d: %s", i + 1, mngr->topicos[i].topico);
                printf(", Nº mensagens persistentes: %d\n",mngr->topicos[i].npersistentes);
        }
    }
    else if (strcmp(comando, "show") == 0) {
        scanf("%s", topic);
       print_topic(manager, topic);
    } 
    //else if (strcmp(comando, "lock") == 0) {
    //    scanf("%s", topic);
    //    //lock_topic(manager, topic);
    //}
    //else if (strcmp(comando, "unlock") == 0) {
    //    scanf("%s", topic);
    //    //unlock_topic(manager, topic);
    //}
    //else if (strcmp(comando, "close") == 0) {
    //    exit(0); // Implementação de saída
    //}
    //else {
    //    printf("Comando desconhecido\n");
    //}
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
                break;
            }
        }
        if (!encontra) {
            printf("User com PID %d nao encontrado.\n", pid);
        }
    }
    
    if (strcmp(comando,"subscribe") == 0) {
        char topico[20];
        sscanf(corpo,"%s",topico);
        
        // verifica se o topico existe no array de topicos do manager
        int verificatopico = -1;
        for (int i = 0; i < mngr->ntopicos; i++) {
            if (strcmp(mngr->topicos[i].topico, topico) == 0) {
                verificatopico = i;
                break;
            }
        }

        // se nao existir cria-o
        if (verificatopico == -1) {
            if (mngr->ntopicos < MAXTOPICS) {
                strcpy(mngr->topicos[mngr->ntopicos].topico, topico);
                mngr->ntopicos++;
                printf("Topico: '%s' adicionado ao manager.\n", topico);
            } else {
                printf("Manager cheio de topicos, chegou ao limite de %d.\n", MAXTOPICS);
                strcpy(resposta->corpo, "Limite topicos, nao criado\n"); 
            }
        }

        int encontra = 0;
        // encontra o user com o PID
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizadores[i].pid == pid) {
                encontra = 1;

                // verifica se esta subscrito
                int alr_subbed = 0;
                for (int j = 0; j < MAXTOPICS; j++) {
                    if (strcmp(mngr->utilizadores[i].subscrito[j].ntopico, topico) == 0) {
                        alr_subbed = 1;
                        printf("User '%s' ja esta subscrito no topico: '%s'.\n",
                               mngr->utilizadores[i].nome_utilizador, topico);
                        strcpy(resposta->corpo, "User ja subscrito\n"); 
                        break;
                    }
                }
                // se nao, subscreve
                if (!alr_subbed) {
                    int add = 0;
                
                    for (int j = 0; j < MAXTOPICS; j++) {
                        if (strlen(mngr->utilizadores[i].subscrito[j].ntopico) == 0) { //se pos. array for vazia
                            strcpy(mngr->utilizadores[i].subscrito[j].ntopico, topico);
                            mngr->utilizadores[i].nsubscritos++;
                            mngr->utilizadores[i].subscrito[j].pid=pid;
                            printf("User '%s' pid '%d' subscreveu ao topico: '%s' (%d users subscritos).\n", mngr->utilizadores[i].nome_utilizador,
                            mngr->utilizadores[i].pid, topico,mngr->utilizadores[i].nsubscritos);
                            add = 1;
                            sprintf(resposta->corpo, "Adicinado ao topico: %s\n",topico); 
                            break;
                        }
                    }
                
                    //se array cheio
                    if (!add) {
                        printf("User '%s' nao pode subscrever a mais tópicos, limite alcançado.\n",mngr->utilizadores[i].nome_utilizador);
                        strcpy(resposta->corpo, "User nao pode subscrever a mais tópicos, limite alcançado."); 
                    }
                }
                break;
            }
        }
        if (!encontra) { //so chega aqui se o managar for fechado e aberto e o feed continuar aberto (falta de login)
                         //nao deve acontecer mas wtv
            printf("user com pid: %d nao encontrado.\n", pid);
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

                        // Remove o tópico do array do usuário
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
            for (int i = 0; i < mngr->ntopicos && i < MAXTOPICS; i++) {
                    printf("topico %d: %s\n", i + 1, mngr->topicos[i].topico);
                    sprintf(resposta->corpo, "topico %d: %s\n", i + 1, mngr->topicos[i].topico);
            }
        }

    if (strcmp(comando, "msg") == 0) {
    char topico[20];
    int duracao;
    char msgp[100]; 

    sscanf(corpo, "%s %d %[^\n]", topico, &duracao, msgp);

    printf("\n\ntest:%s\n\n",msgp);

    for (int i = 0; i < mngr->ntopicos && i < MAXTOPICS; i++) {
        if (strcmp(mngr->topicos[i].topico, topico) == 0) {
            if (duracao > 0) {
                if (mngr->topicos[i].npersistentes < MAXMSGS) {
                    int j = mngr->topicos[i].npersistentes;

                    strcpy(mngr->topicos[i].conteudo[j].corpo, msgp);
                    mngr->topicos[i].conteudo[j].duracao = duracao;

                    (mngr->topicos[i].npersistentes)++;

                    printf("\nMensagem adicionada ao tópico '%s', com duração de %d segundos:\n%s\n\n",
                           mngr->topicos[i].topico,
                           mngr->topicos[i].conteudo[j].duracao,
                           mngr->topicos[i].conteudo[j].corpo);

                    sprintf(resposta->corpo, "Conteúdo adicionado ao tópico %s:\n%s", mngr->topicos[i].topico, msgp);
                    printf("resposta->corpo:%s\n\n",resposta->corpo);
                } else {
                    printf("\nErro: limite de mensagens para o tópico '%s' foi atingido.\n", topico);
                    sprintf(resposta->corpo, "Erro: limite de mensagens no tópico %s atingido.", topico);
                }
            }
            return; 
        }
    }

    sprintf(resposta->corpo, "Erro: tópico %s não encontrado.", topico);
}


}

