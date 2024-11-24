#include "utils.h"
#include "manager.h"

int list_user(pman m);
int remove(pman m, const char username);
int list_topics(pman m);
int print_topic(pman m,const char topic);
int lock_topic(pman m,const char topic);
int unlock_topic(pman m,const char topic);
