#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_MSG 1024

/*
    Experimento 01
    Servidor aguarda por mensagem do cliente, imprime na tela
    e depois envia resposta. Quando o cliente enviar "fim" a
    conexão é finalizada.
*/

int main(void)
{
    //Variáveis
    int socket_desc, conexao, c;
    struct sockaddr_in servidor, cliente;
    char *mensagem;
    char resposta[MAX_MSG];
    int tamanho, count;

    //Para pegar o IP e porta do cliente
    char *cliente_ip;
    int cliente_port;


    //Criando um socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Nao foi possivel criar o socket\n");
        return -1;
    }

    int reuso = 1;
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuso, sizeof(reuso)) < 0)
    {
        perror("Não foi possível reusar endereço");
        return -1;
    }
#ifdef SO_REUSEPORT
    if (setsockopt(socket_desc, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuso, sizeof(reuso)) < 0)
    {
        perror("Não foi possível reusar porta");
        return -1;
    }
#endif

    //Preparando a struct do socket
    servidor.sin_family = AF_INET;
    servidor.sin_addr.s_addr = INADDR_ANY; // Obtem IP do S.O.
    servidor.sin_port = htons(1234);

    //Associando o socket a porta e endereco
    if (bind(socket_desc, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
    {
        perror("Erro ao fazer bind\n");
        return -1;
    }
    puts("Bind efetuado com sucesso\n");

    //Ouvindo por conexoes
    listen(socket_desc, 3);


    //Aceitando e tratando conexoes
    puts("Aguardando por conexoes...");
    c = sizeof(struct sockaddr_in);


    //Iniciando conexão
    conexao = accept(socket_desc, (struct sockaddr *)&cliente, (socklen_t *)&c);

    if (conexao < 0)
    {
        perror("Erro ao receber conexao\n");
        return -1;
    }

    cliente_ip = inet_ntoa(cliente.sin_addr);
    cliente_port = ntohs(cliente.sin_port);
    printf("cliente conectou\nIP:PORTA -> %s:%d\n", cliente_ip, cliente_port);
    mensagem = "Seja bem-vindo cliente!";
    write(conexao, mensagem, strlen(mensagem));

    int k = 0;

    do{
        if (conexao < 0)
        {
            perror("Erro ao receber conexao\n");
            return -1;
        }


        // lendo dados enviados pelo cliente
        tamanho = read(conexao, resposta, MAX_MSG);

        if (tamanho < 0) {
            perror("Erro ao receber dados do cliente: ");
            return -1;
        }

        resposta[tamanho] = '\0';

        int x = strcmp(resposta, "fim");

        if(x == 0){
            mensagem = "Encerrando conexão";
            write(conexao, mensagem, strlen(mensagem));
            printf("Cliente encerrou a conexão!\n");
            k = 1;
        }
        else{
            printf("O cliente falou: %s\n", resposta);
            mensagem = "Recebi sua mensagem!";
            write(conexao, mensagem, strlen(mensagem));
        }

    }while(k<1);

    //Finalizando conexão
    close(socket_desc);
    shutdown(socket_desc, 2);

    printf("Servidor finalizado...\n");
    return 0;
}