#ifndef MANAGER_H
#define MANAGER_H
#include "processocom.h"
void abre_pipes(pid_t pid, int *fd_feed_p, msg *msg_env) ;
void *ler_pipe(void *tdata);
#endif

