#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include "manager.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_fin[100];

list_user(pman manager){

};
remove(pman manager, const char username){

};
list_topics(pman manager){

};

print_topic(pman manager,const char topic){

};
lock_topic(pman manager,const char topic){

};
unlock_topic(pman manager,const char topic){

};

int main()
{
	pman manager;
	manager->nusers=0; 
	manager->ntopicos=0;
	char username[20];
	char comando[20];
	printf("Insira comando: ");
	scanf("%c",comando);
	char topic[20];


	if(strcmp(comando,"users")==0){
		list_user(manager);
	}
	else if(strcmp(comando,"remove")==0){
		scanf("%s",username);
		remove(manager, username);}
	else if(strcmp(comando,"topics")==0){
		list_topics(manager);
	}
	else if(strcmp(comando,"show")==0){
		scanf("%s", topic);
		print_topic(manager,topic);
	}else if(strcmp(comando,"lock")==0){
		scanf("%s", topic);
		lock_topic(manager,topic);
	}
	else if(strcmp(comando,"unlock")==0){
		scanf("%s", topic);
		unlock_topic(manager,topic);
	}
	else if(strcmp(comando,"close")==0){
		exit(0);}//falta impletmentar saida ordeira
	else{
		printf("Comando desconhecido\n");//enviar pipe(falta implementar)
	}


}
