#include "processocom.h"

void processa_comando_manager(char *comando, man *manager) {
    char username[20];
    char topic[20];

    if (strcmp(comando, "users") == 0) {
        //list_user(manager);  
    }
    else if (strcmp(comando, "remove") == 0) {
        scanf("%s", username);
        //remover(manager, username);
    }
    else if (strcmp(comando, "topics") == 0) {
        //list_topics(manager);
    }
    else if (strcmp(comando, "show") == 0) {
        scanf("%s", topic);
        //print_topic(manager, topic);
    } else if (strcmp(comando, "lock") == 0) {
        scanf("%s", topic);
        //lock_topic(manager, topic);
    }
    else if (strcmp(comando, "unlock") == 0) {
        scanf("%s", topic);
        //unlock_topic(manager, topic);
    }
    else if (strcmp(comando, "close") == 0) {
        exit(0); // Implementação de saída
    }
    else {
        printf("Comando desconhecido\n");
    }
}


void * processa_comando_feed(char *corpo, char *comando, int pid, void *manager, void *users) {
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

            } else {
                printf("Impossivel adicionar mais users\n");
            }
        } else {
            printf("User '%s' ja existe.\n", corpo);
        }
    }
    
    if (strcmp(comando,"users") == 0) {    //apenas para testes!!!! este comando é so do manager!!!!!
        for (int i = 0; i < mngr->nusers && i < MAXUSERS; i++) {
                printf("User %d: %s\n", i + 1, mngr->utilizadores[i].nome_utilizador);
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
        
        int encontra = 0;

        // encontra o user com o PID
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizadores[i].pid == pid) {
                encontra = 1;

                // verifica se esta subscrito
                int alr_subscribed = 0;
                for (int j = 0; j < MAXTOPICS; j++) {
                    if (strcmp(mngr->utilizadores[i].subscrito[j], topico) == 0) {
                        alr_subscribed = 1;
                        printf("User '%s' ja esta subscrito no topico: '%s'.\n",
                               mngr->utilizadores[i].nome_utilizador, topico);
                        break;
                    }
                }
                // se nao, subscreve
                if (!alr_subscribed) {
                    int add = 0;
                
                    for (int j = 0; j < MAXTOPICS; j++) {
                        if (strlen(mngr->utilizadores[i].subscrito[j]) == 0) { //se pos. array for vazia
                            strcpy(mngr->utilizadores[i].subscrito[j], topico);
                            printf("User '%s' subscreveu ao topico: '%s'.\n", mngr->utilizadores[i].nome_utilizador, topico);
                            add = 1;
                            break;
                        }
                    }
                
                    //se array cheio
                    if (!add) {
                        printf("User '%s' nao pode subscrever a mais tópicos, limite alcançado.\n",mngr->utilizadores[i].nome_utilizador);
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
        int findusr = 0;

        // encontra user no array com PDI
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizadores[i].pid == pid) {
                findusr = 1;

                // verifica se esta subbed
                int findtopic = 0;
                for (int j = 0; j < MAXTOPICS; j++) {
                    if (strcmp(mngr->utilizadores[i].subscrito[j], corpo) == 0) {
                        findtopic = 1;

                        // substitui o topico por string vazia
                        mngr->utilizadores[i].subscrito[j][0] = '\0';
                        printf("User '%s' subscreveu ao topico: '%s'.\n",mngr->utilizadores[i].nome_utilizador, corpo);

                        // puxa os elementos depois do apagado para a esquerda
                        for (int k = j; k < MAXTOPICS - 1; k++) {
                            strcpy(mngr->utilizadores[i].subscrito[k], mngr->utilizadores[i].subscrito[k + 1]);
                        }
                        // limpa o ultimo depois de empurrar para evitar copias
                        mngr->utilizadores[i].subscrito[MAXTOPICS - 1][0] = '\0';
                        break;
                    }
                }
                if (!findtopic) {
                    printf("User '%s' nao esta subscrito no topico: '%s'.\n", mngr->utilizadores[i].nome_utilizador, corpo);
                }
                break;
            }
        }
        if (!findusr) { //so chega aqui se o managar for fechado e aberto e o feed continuar aberto (falta de login)
                        //nao deve acontecer mas wtv
            printf("User %d nao encontrado, subscricao falhada\n", pid);
        }
    }

    //foda-se esqueci-me que isto tem de ser feito no array de topicos, OH EGUA
    return NULL;
}

