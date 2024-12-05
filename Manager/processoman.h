#include "utils.h"
#include "manager.h"
#include "processocom.h"
struct Manager;
typedef struct Manager man,*pman;

int list_user(pman m);
int remover(pman m, const char *username);
int list_topics(pman m);
int print_topic(pman m,const char *topic);
int lock_topic(pman m,const char *topic);
int unlock_topic(pman m,const char *topic);
