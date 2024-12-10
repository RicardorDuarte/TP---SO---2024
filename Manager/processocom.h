#include "processoman.h"
// Função que processa os comandos do usuário ou do pipe
void processa_comando_manager(char *comando, man *manager);

void processa_comando_feed(msg *resposta,char *corpo, char *comando, int pid, void *manager, void *users);

