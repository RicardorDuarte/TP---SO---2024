#include "manager.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];



int main()
{
	man manager;
	msg mensagemRecebida;
	manager.nusers=0; 
	manager.ntopicos=0;
	char username[20];
	char comando[20];
	int fd_manager_pipe,sizeMan;
	printf("Manager iniciado:\n");
	//scanf("%c",comando);
	char topic[20];
	

	if(mkfifo(ManPipe,0666) == -1){
		perror("erro a criar named pipe\n");
		return -1;
	}

	fd_manager_pipe = open(ManPipe,O_RDONLY);
	if(fd_manager_pipe == -1){
		perror("erro a abrir named pipe\n");
		return -1;
	}


	//receber e tratar mensagens:
	while(1){
		sizeMan = read(fd_manager_pipe,&mensagemRecebida,sizeof(mensagemRecebida));
		if(sizeMan > 0){
			printf("%s, %s, %d\n ",mensagemRecebida.topico,mensagemRecebida.corpo,mensagemRecebida.pid);
			
			if(strcmp(mensagemRecebida.corpo,"sair") == 0){
				close(fd_manager_pipe);
				unlink(ManPipe);
				exit(1);
			}
			sprintf(feedpipe_final,"FEED_FIFO[%d]",getpid());
			
		}
	}


	
	
	//interação com teclado, não implementado ainda por falta de threads:
	//if(strcmp(comando,"users")==0){
	//	list_user(manager);
	//}
	//else if(strcmp(comando,"remove")==0){
	//	scanf("%s",username);
	//	remove(manager, username);}
	//else if(strcmp(comando,"topics")==0){
	//	list_topics(manager);
	//}
	//else if(strcmp(comando,"show")==0){
	//	scanf("%s", topic);
	//	print_topic(manager,topic);
	//}else if(strcmp(comando,"lock")==0){
	//	scanf("%s", topic);
	//	lock_topic(manager,topic);
	//}
	//else if(strcmp(comando,"unlock")==0){
	//	scanf("%s", topic);
	//	unlock_topic(manager,topic);
	//}
	//else if(strcmp(comando,"close")==0){
	//	exit(0);}//falta impletmentar saida ordeira
	//else{
	//	printf("Comando desconhecido\n");//enviar pipe(falta implementar)
	//}

	return 0;
}
