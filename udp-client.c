/* includes */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Cabecalhos */
char **split(char frase[], char separador);

/* main */
int main(int argc, char**argv) {
	
	/* declara variaveis */
	int sockfd, n;
	struct sockaddr_in servaddr, cliaddr;
	char mensagemAEnviar[1000];
	char mensagemAReceber[1000];

	/* execucao faltando argumentos: exibe mensagem de erro e finaliza execucao */
	if (argc != 3) {
		printf("Formato:  udpclient <IP> <PORTA>\n");
		exit(1);
	}

	/* cria socket */
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	/* inicializa a struct que contem os dados do servidor */
	bzero(&servaddr, sizeof(servaddr));
	
	/* determina o protocolo de comunicacao a ser utilizado */
	servaddr.sin_family = AF_INET;
	
	/* determina o endereco IP do servidor, passado como argumento */
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	
	/* determina a porta de comunicacao, passada como argumento */
	servaddr.sin_port = htons(atoi(argv[2]));

	/* Enquanto houver mensagem... */
	while (fgets(mensagemAEnviar, 10000, stdin) != NULL) {
		
		/* Envia mensagem ao servidor */
		sendto(sockfd, mensagemAEnviar, strlen(mensagemAEnviar), 0,
				(struct sockaddr *) &servaddr, sizeof(servaddr));
	}
}


/* funcao para dar split em uma string */
char **split(char frase[], char separador) {
	
	int i, j, k, contsep = 0;

	for (i = 0, contsep = 0; i < strlen(frase); i++)
		if (frase[i] == separador)
			contsep++;

	char aux[contsep][20];
	char **result = (char**) malloc(contsep * sizeof(char));

	if (contsep) {
		for (i = 0; i <= contsep; i++)
			*(result + i) = (char*) malloc(40 * sizeof(char));

		for (i = 0, k = 0, j = 0; i < strlen(frase); i++)
			if (frase[i] != separador) {
				aux[k][j] = frase[i];
				j++;
			} else {
				aux[k][j] = 0;
				k++;
				j = 0;
			}
		aux[k][j] = 0;

		for (i = 0; i <= contsep; i++)
			*(result + i) = strcpy(*(result + i), aux[i]);

		return result;
	} else
		printf("Nenhum Separador Encontrado");
}

