#include "../Utils/utils.h"
#include"manager.h"
// Função que processa os comandos do usuário ou do pipe
void guarda_mensagens_persistentes(man *manager);

void load_mensagens_persistentes(man *manager);

void processa_comando_manager(char *comando, man *manager) ;

void processa_comando_feed(msg *resposta,char *corpo, char *comando, int pid, void *manager, void *users);

