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


void * processa_comando_feed(char *corpo, char *comando, int pid, void *manager) {
    man *mngr = (man*)manager;
    //char username[20];
    //char topic[20];

    if (strcmp(comando, "login") == 0) {
        int user_exists = 0;

        // Check if the user already exists
        for (int i = 0; i < mngr->nusers; i++) {
            if (strcmp(mngr->utilizador[i].nome_utilizador, corpo) == 0) {
                user_exists = 1;
                break;
            }
        }
        //mandar sinal para matar feed que existe
        if (!user_exists) {
            // Add the user if they don't exist
            if (mngr->nusers < MAXUSERS) { // Assuming MAX_USERS is defined
                strcpy(mngr->utilizador[mngr->nusers].nome_utilizador, corpo);
                mngr->nusers++;
            } else {
                printf("Impossivel adicionar mais users\n");
            }
        } else {
            printf("User '%s' is already logged in.\n", corpo);
        }
    }
    
    if  (strcmp(comando,"users") == 0) {
        for (int i = 0; i < mngr->nusers && i < MAXUSERS; i++) {
                printf("User %d: %s\n", i + 1, mngr->utilizador[i].nome_utilizador);
        }
    }
    
    if (strcmp(comando, "exit") == 0) {
        int found = 0;

        // Search for the user by pid
        for (int i = 0; i < mngr->nusers; i++) {
            if (mngr->utilizador[i].pid == pid) { // Match the user's PID with the message's PID
                found = 1;

                // Shift all subsequent users left by one position
                for (int j = i; j < mngr->nusers - 1; j++) {
                    mngr->utilizador[j] = mngr->utilizador[j + 1]; // Copy entire structure
                }

                // Decrement the user count
                mngr->nusers--;

                printf("User with PID %d has been removed.\n", pid);
                break;
            }
        }

        if (!found) {
            printf("User with PID %d not found.\n", pid);
        }
    }
    return NULL;
}

