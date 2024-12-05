#include "processocom.h"

void processa_comando_manager(char *comando, man *manager) {
    char username[20];
    char topic[20];

    if (strcmp(comando, "users") == 0) {
        list_user(manager);  
    }
    else if (strcmp(comando, "remove") == 0) {
        scanf("%s", username);
        remover(manager, username);
    }
    else if (strcmp(comando, "topics") == 0) {
        list_topics(manager);
    }
    else if (strcmp(comando, "show") == 0) {
        scanf("%s", topic);
        print_topic(manager, topic);
    } else if (strcmp(comando, "lock") == 0) {
        scanf("%s", topic);
        lock_topic(manager, topic);
    }
    else if (strcmp(comando, "unlock") == 0) {
        scanf("%s", topic);
        unlock_topic(manager, topic);
    }
    else if (strcmp(comando, "close") == 0) {
        exit(0); // Implementação de saída
    }
    else {
        printf("Comando desconhecido\n");
    }
}

