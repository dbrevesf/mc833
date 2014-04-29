/* includes */
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/* cabecalhos */
char* concat(int count, ...);
char* normalizeCoordinate(char* coordinate);
char** split(char* string, char delimiter);
int validaString(char* str, char validator);
sqlite3* openDB(char* database);
static int callback(void *data, int argc, char **argv, char **azColName);
void executeDB(char* sql, sqlite3 *db);

/* variavel global para indicar se a posicao do usario ja foi inserida ou nao */
int positionIserted = 0;

/* main */
int main(int argc, char**argv) {
	
	/* define variaveis */
    int listenfd, connfd, n;
	struct sockaddr_in servaddr, cliaddr;
	socklen_t clilen;
	pid_t childpid;
	char mesg[1000];
	
	/* cria struct para armazenar a localizacao do usuario */
	typedef struct {
		char *x;
		char *y;
	} Posicao;

	/* execucao faltando argumentos: exibe mensagem de erro e finaliza execucao */
	if (argc != 2) {
		printf("Formato:  tcpserver <PORTA>\n");
		exit(1);
	}

	/* cria socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	/* inicializa a struct que contem os dados do servidor */
	bzero(&servaddr, sizeof(servaddr));
	
	/* determina o protocolo de comunicacao a ser utilizado */
	servaddr.sin_family = AF_INET;
	
	/* obtem o endereco IP da maquina onde o servidor ira rodar */
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	/* determina a porta de comunicacao, passada como argumento */
	servaddr.sin_port = htons(atoi(argv[1]));
	
	/* determina o endereco IP ao socket criado */
	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	/* determina o socket como passivo */
	listen(listenfd, 1024);

	/* loop infinito */
	for (;;) {
		
		/* aceita coneccao do cliente */
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

		/* fork executado com sucesso: prepara servidor para escutar cliente (um fork por cliente) */
		if ((childpid = fork()) == 0) {
			
			/* fecha o arquivo descritor do socket de escuta */
			close(listenfd);

			/* loop infinito */
			for (;;) {
				
				/* recebe mensagem do cliente*/
				n = recvfrom(connfd, mesg, 1000, 0,
						(struct sockaddr *) &cliaddr, &clilen);

				/* instancia struct de posicao */
				Posicao posicao;

				/* verifica se ha o caracter ':', que valida a mensagem */
				int correctEntry = 0;
				correctEntry = validaString(mesg, ':');
				
				
				/* entrada incorreta: exibe mensagem de ajuda */
				if (correctEntry == 0) {
					fprintf(stderr,
							"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
				}
				
				/* entrada correta: executa as acoes enviadas */ 
				else {
			
					char* mensagemAux = (char*)malloc(sizeof(mesg));
					strcpy(mensagemAux, mesg);
					
					/* Split da mensagem */
					char** entrada = split(strtok(mesg, "\n"), ':');
					
					/* MENSAGEM de posicao enviada: executa as acoes de posicao */
					if (strcmp(entrada[0], "posicao") == 0) {

						if (strlen(entrada[1])<3){
								fprintf(stderr,
									"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
						}
						
						else {
						
							/* remove \n da entrada */
							char* position = strtok(entrada[1], "\n");

							/* MENSAGEM: posicao:get -> imprime posicao do usuario na tela */
							if (strcmp(position, "get") == 0 && positionIserted==1) {
				
								/* Exibe mensagem de sucesso */
								printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x,
										posicao.y);
							}

							/* MENSAGEM: posicao:<x>,<y> -> armazena x e y na posicao do usuario */
							else if (strcmp(position, "set") == 0) {
								
								int stringValida = 0;
								
								/* verifica se há o separador na string */
								stringValida = validaString(entrada[2], ',');
								
								if (stringValida == 0){
									fprintf(stderr,
										"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
								}
								else{
									/* split pra pegar x e y */
									char** posicoes = split(entrada[2], ',');
									int X = atoi(posicoes[0]);
									int Y = atoi(posicoes[1]);

									/* Posicao fora da area de cobertura: exibe mensagem de erro */
									if (X > 1000 || X < 0 || Y > 1000 || Y < 0) {

										fprintf(stderr,
												"\nERRO: fora da area de cobertura do servico \n");
									} 
									
									/* Posicao dentro da area de cobertura: atribui-as a struct de posicao */
									else {

										/* aloca memoria pras posicoes */
										posicao.x = (char*) malloc(
												strlen(posicoes[0]) * sizeof(char));
										posicao.y = (char*) malloc(
												strlen(posicoes[1]) * sizeof(char));
												
										/* seta posicoes */
										strcpy(posicao.x, posicoes[0]);
										strcpy(posicao.y, posicoes[1]);
										
										positionIserted = 1;
										
										/* Exibe mensagem de sucesso */
										fprintf(stderr, "\nPosicao do usuario-> X: %s Y: %s \n",
												posicao.x, posicao.y);
									}
								}
							} 
							
							/* MENSAGEM posicao:<tag invalida> -> exibe mensagem de ajuda */
							else {
								
								fprintf(stderr,
										"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
							}
						}
					}

					/* mensagem de estabelecimento enviada: executa as acoes de estabelecimento */
					else if (strcmp(entrada[0], "est") == 0) {
						
						if (strlen(entrada[1])<3){
								fprintf(stderr,
									"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
						}
						else {
							/* remove \n da entrada */
							char* entry = strtok(entrada[1], "\n");

							/* MENSAGEM est:cat:<categoria> -> lista estabelecimentos da categoria <categoria> 
							 * num raio de 100 m do usuario */
							if (strcmp(entry, "cat") == 0 && positionIserted==1) {

								if (strlen(entrada[2])<3){
									fprintf(stderr,
										"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
								}
								else{
									/* remove \n da entrada */
									char* category = strtok(entrada[2], "\n");

									/* aloca memoria pras posicoes do raio de 100 metro do usuario */
									char* x_plus_100 = (char*) malloc(4 * sizeof(char));
									char* x_minus_100 = (char*) malloc(4 * sizeof(char));
									char* y_plus_100 = (char*) malloc(4 * sizeof(char));
									char* y_minus_100 = (char*) malloc(4 * sizeof(char));

									/* seta posicoes */
									sprintf(x_plus_100, "%d", ((atoi(posicao.x) + 100)));
									sprintf(x_minus_100, "%d", ((atoi(posicao.x) - 100)));
									sprintf(y_plus_100, "%d", ((atoi(posicao.y) + 100)));
									sprintf(y_minus_100, "%d", ((atoi(posicao.y) - 100)));

									/* monta query */
									char* query1 =
											"SELECT * from estabelecimentos WHERE coord_x >= ";
									char* query2 = " AND coord_x <= ";
									char* query3 = " AND coord_y >= ";
									char* query4 = " AND coord_y <= ";
									char* query5 = " AND category = ";
									char* sql1 = concat(2, query1, x_minus_100);
									char* sql2 = concat(3, sql1, query2, x_plus_100);
									char* sql4 = concat(3, sql2, query3, y_minus_100);
									char* sql5 = concat(3, sql4, query4, y_plus_100);
									char* sql6 = concat(5, sql5, query5, "'", category, "';");

									/* Abre o bd */
									sqlite3* db = openDB("estabelecimentos.db");

									/* Executa a query */
									executeDB(sql6, db);
								}
							}

							/* MENSAGEM est:around -> lista todos estabelecimentos num raio de 100 m do usuario */
							else if (strcmp(entry, "around") == 0 && positionIserted==1) {

								/* aloca memoria pras posicoes do raio de 100 metro do usuario */
								char* x_plus_100 = (char*) malloc(4 * sizeof(char));
								char* x_minus_100 = (char*) malloc(4 * sizeof(char));
								char* y_plus_100 = (char*) malloc(4 * sizeof(char));
								char* y_minus_100 = (char*) malloc(4 * sizeof(char));

								/* seta posicoes */
								sprintf(x_plus_100, "%d", ((atoi(posicao.x) + 100)));
								sprintf(x_minus_100, "%d", ((atoi(posicao.x) - 100)));
								sprintf(y_plus_100, "%d", ((atoi(posicao.y) + 100)));
								sprintf(y_minus_100, "%d", ((atoi(posicao.y) - 100)));

								/* monta query */
								char* query1 =
										"SELECT * from estabelecimentos WHERE coord_x >= ";
								char* query2 = " AND coord_x <= ";
								char* query3 = " AND coord_y >= ";
								char* query4 = " AND coord_y <= ";
								char* sql1 = concat(2, query1, x_minus_100);
								char* sql2 = concat(3, sql1, query2, x_plus_100);
								char* sql4 = concat(3, sql2, query3, y_minus_100);
								char* sql5 = concat(3, sql4, query4, y_plus_100);

								/* abre o bd*/
								sqlite3* db = openDB("estabelecimentos.db");

								/* executa a query */
								executeDB(sql5, db);
							}

							/* MENSAGEM est:vote:<est>:<rate> -> Vota um valor <rate> ao estabelecimento de id <est> */
							else if (strcmp(entry, "vote") == 0) {
								
								/* Checa validade da entrada do voto*/
								int in = 0;
								int twoPoints = 0;
								for (in=0; in<strlen(mensagemAux)-1; in++){
									if (mensagemAux[in] == ':'){
										twoPoints++;
									}
								}
								
								/* entrada errada: exibe erro */ 
								if (twoPoints != 3){
									fprintf(stderr,
											"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
								}
								
								/* entrada correta: atribui voto */
								else{
									
									if (strlen(entrada[2]) < 1 || strlen(entrada[3]) < 1){
										fprintf(stderr,
											"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
									}
									else{
										/* pega id do estabelecimento */
										char* establishment = entrada[2];
										
										/* pega nota atribuida */
										char* rate = entrada[3];
										
										/* Nota fora da escala: exibe mensagem de erro */
										if (atoi(rate) > 10 || atoi(rate) < 0) {
											fprintf(stderr,
													"\nEntrada errada: Insira um valor entre 0 e 10\n");
										} 
										
										/* Nota dentro da escala: atribui a nota ao estabelecimento */
										else {
											
											/* monta a query */
											char* sql = concat(5,
													"INSERT INTO notas VALUES (NULL, ",
													establishment, " ,", rate, " )");
													
											/* abre o bd */
											sqlite3* db = openDB("estabelecimentos.db");
											
											/* executa a query */
											executeDB(sql, db);
											
											/* exibe mensagem de sucesso */
											fprintf(stderr,
													"\nO estabelecimento %s recebeu a nota %s\n",
													establishment, rate);
										}
									}
								}
							}

							/* MENSAGEM est:getRate:<est> -> Exibe a média dos votos para o estabelecimento com id <est>*/
							else if (strcmp(entry, "getRate") == 0) {
								
								/* Checa validade da entrada do getRate*/
								int in = 0;
								int twoPoints = 0;
								for (in=0; in<strlen(mensagemAux)-1; in++){
									if (mensagemAux[in] == ':'){
										twoPoints++;
									}
								}
								
								/* entrada errada: exibe erro */ 
								if (twoPoints != 2){
									fprintf(stderr,
											"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
								}
								else{
									
									if (strlen(entrada[2]) < 1){
										fprintf(stderr,
											"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
									}
									else{
									
										/* pega id do estabelecimento */
										char* establishment = entrada[2];
										
										/* monta query */
										char* sql = concat(2,
												"SELECT AVG(rate) from notas where estabId= ",
												establishment);
												
										/* abre o bd */
										sqlite3* db = openDB("estabelecimentos.db");
										
										/* executa query */
										executeDB(sql, db);
									}
								}
							}

							/* MENSAGEM est:getInfo:<est> -> Exibe informacoes gerais sobre o estabelecimento com id <est>*/
							else if (strcmp(entry, "getInfo") == 0) {
								
								/* Checa validade da entrada do getInfo*/
								int in = 0;
								int twoPoints = 0;
								for (in=0; in<strlen(mensagemAux)-1; in++){
									if (mensagemAux[in] == ':'){
										twoPoints++;
									}
								}
								
								/* entrada errada: exibe erro */ 
								if (twoPoints != 2){
									fprintf(stderr,
											"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
								}
								else{
									/* entrada fora do padrao: exibe mensagem de erro */
									if (strlen(entrada[2]) > 3 || strlen(entrada[2]) < 1) {
									
										fprintf(stderr,
												"Entrada errada: Insira um ID de no maximo 3 digitos ou a key 'all'");
									} 
									
									/* entrada correta: exibe informacoes */
									else {
										
										/* abre bd */
										sqlite3* db = openDB("estabelecimentos.db");

										/* MENSAGEM est:getInfo:all -> exibe informacoes de todos estabelecimentos cadastrados*/
										if (strcmp(entrada[2], "all") == 0) {

											/* monta query */
											char* sql = "SELECT * from estabelecimentos";

											/* executa query */
											executeDB(sql, db);
										}
										
										/* MENSAGEM est:getInfo:<id>  -> exibe informacoes do estabelecimento cujo id é <id>*/
										else {

											/* monta query */
											char* query =
													"SELECT * from estabelecimentos WHERE ID = ";
											char* sql = concat(2, query, entrada[2]);

											/* executa query */
											executeDB(sql, db);
										}
									}
								}
							} 
							
							/* entrada fora do padrao: exibe mensagem de ajuda */
							else {
								fprintf(stderr,
										"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
							}
						}
					} 
					
					/* MENSAGEM HELP -> Exibe ajuda */
					else if (strcmp(entrada[0], "HELP") == 0) {
						
						fprintf(stderr,
								"\nposicao:set:<x>,<y>   ->  Seta posicao do usuario\n");
						fprintf(stderr,
								"posicao:get     (*)     ->  Exibe posicao do usuario \n");
						fprintf(stderr,
								"est:cat:<cat>   (*)     ->  Lista estabelecimento em um raio de 100 m cuja categoria é <cat>\n");
						fprintf(stderr,
								"est:around      (*)     ->  Lista todos estabelecimentos cadastrados num raio de 100 m do usuario \n");
						fprintf(stderr,
								"est:vote:<id>:<rate>    ->  Atribui nota <rate> (0 a 10) ao estabelecimento cujo id é <id>\n");
						fprintf(stderr,
								"est:getRate:<id>        ->  Exibe a nota média do estabelecimento cujo id é <id> até o momento\n");
						fprintf(stderr,
								"est:getInfo:all         ->  Lista todos estabelecimentos cadastrados\n");
						fprintf(stderr,
								"est:getInfo:<id>        ->  Lista estabelecimento cujo ID é <id>\n");
						fprintf(stderr,
								"EXIT:                   ->  Sair do programa (SEMPRE USE EXIT PARA SAIR !!!!)\n");
						fprintf(stderr,
								"\n*** AS FUNCOES COM (*) SÓ SAO POSSIVEIS APÓS USUÁRIO INFORMAR LOCALIZACAO ***\n");
					}

					/* MENSAGEM EXIT -> finaliza execucao do programa */
					else if (strcmp(entrada[0], "EXIT") == 0){
								fprintf(stderr,
										"\nGOOD BYE\n");
								close(connfd);
								exit(1);
					}
					
					/* entrada fora do padrao: exibe mensagem de ajuda */
					else {
						fprintf(stderr,
								"\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
					}
				}
			}

		}
		close(connfd);
	}
}


/* funcao que abre um bd */
sqlite3* openDB(char* database) {
	sqlite3* db;
	int rc;
	rc = sqlite3_open(database, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	return db;
}


/* funcao que executa uma query no bd */
void executeDB(char* sql, sqlite3 *db) {
	char *zErrMsg = 0;
	int rc;
	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	fprintf(stderr,
			"\n**********************************************************\n");
}

/* funcao para imprimir resultados das queries */
static int callback(void *data, int argc, char **argv, char **azColName) {
	int i;
	printf("\n");
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

/* funcao que concatena duas ou mais strings */
char* concat(int count, ...) {
	va_list ap;
	int len = 1, i;

	va_start(ap, count);
	for (i = 0; i < count; i++)
		len += strlen(va_arg(ap, char*));
	va_end(ap);

	char *result = (char*) calloc(sizeof(char), len);
	int pos = 0;

	va_start(ap, count);
	for (i = 0; i < count; i++) {
		char *s = va_arg(ap, char*);
		strcpy(result + pos, s);
		pos += strlen(s);
	}
	va_end(ap);

	return result;
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

int validaString(char* str, char validator){
	int i = 0;
	int valida = 0;
	for (i=0; i < strlen(str); i++){
		if (str[i]==validator){
			valida = 1;
			break;
		}
	}
	return valida;		
}
