#include "manager.h"
#include "processoman.h"
#include "processocom.h"
#define ManPipe "MANAGER_FIFO"
#define FeedPipe "FEED_FIFO[%d]"
char feedpipe_final[100];

void *ler_pipe(void *pdata) {
    PipeData *pipe_data = (PipeData *)pdata;
    msg mensagemRecebida;
    int fd_manager_pipe = pipe_data->fd_manager_pipe;
    man *manager = pipe_data->manager;
    int sizeMan;
    
    while (1) {
        sizeMan = read(fd_manager_pipe, &mensagemRecebida, sizeof(mensagemRecebida));

        if (sizeMan > 0) {
            // Printf adicional para mostrar os dados recebidos
            printf("Mensagem recebida do pipe:\n");
            printf("Comando: %s\n", mensagemRecebida.comando);
            printf("Corpo: %s\n", mensagemRecebida.corpo);
            printf("PID: %d\n", mensagemRecebida.pid);

            // Processa o comando recebido do pipe (como se fosse digitado pelo usuário)
            processa_comando(mensagemRecebida.corpo, manager);

            // Verifica se a mensagem "sair" foi recebida
            if (strcmp(mensagemRecebida.corpo, "sair") == 0) {
                close(fd_manager_pipe);
                unlink(ManPipe);
                exit(1);
            }
        } else if (sizeMan == 0) {
            printf("Pipe fechado pelo outro processo.\n");
            break; // Encerra o loop se o pipe for fechado.
        } else {
            perror("Erro ao ler do pipe");
            break; // Encerra o loop em caso de erro.
        }
	}
    return NULL;
}

int main() {
    man manager;
    pthread_t tid_pipe;
    PipeData pipe_data;
    char comando[20];
    //char username[20];
    //char topic[20];
    int fd_manager_pipe;

    // Inicializa o manager
    manager.nusers = 0;
    manager.ntopicos = 0;

    if (access(ManPipe, F_OK) == -1) {  // Verifica se o FIFO já existe
        if (mkfifo(ManPipe, 0666) == -1) {
            perror("Erro ao criar o FIFO");
            return -1;
        } else {
            printf("FIFO '%s' criado com sucesso.\n", ManPipe);
        }
    } else {
        printf("FIFO '%s' já existe. Ignorando criação.\n", ManPipe);
    }

    fd_manager_pipe = open(ManPipe, O_RDWR);//aberto para leitura e escrita para nao ficar preso
    if (fd_manager_pipe == -1) {
        perror("Erro ao abrir o FIFO");
        return -1;
    }
    printf("FIFO '%s' aberto com sucesso.\n", ManPipe);

    // Prepara os dados para a thread ler o pipe
    pipe_data.fd_manager_pipe = fd_manager_pipe;
    pipe_data.manager = &manager;

    // Cria a thread para ler do pipe
    if (pthread_create(&tid_pipe, NULL, ler_pipe, (void *)&pipe_data) != 0) {
        perror("Erro ao criar thread para ler do pipe");
        close(fd_manager_pipe);
        unlink(ManPipe);
        return -1;
    }

    // Loop de interação com o usuário
    do {
        printf("CMD> ");
        fflush(stdout);
        scanf("%s", comando);
        
        // Processa o comando digitado pelo usuário
        processa_comando(comando, &manager);

    } while (strcmp(comando, "exit") != 0);

    // Espera a thread de leitura do pipe finalizar
    pthread_join(tid_pipe, NULL);

    // Finaliza a execução
    close(fd_manager_pipe);
    unlink(ManPipe);

    printf("FIM\n");
    return 0;
}
