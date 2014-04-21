#include <netinet/in.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

char* concat(int count, ...);
char* normalizeCoordinate(char* coordinate);
char** split(char* string, char delimiter);
sqlite3* openDB(char* database);
static int callback(void *data, int argc, char **argv, char **azColName);
void executeDB(char* sql, sqlite3 *db);


int main(int argc, char**argv)
{
    /* define variaveis */
    int rc;
    int sockfd,n;   
    char mesg[1000];
    char *zErrMsg = 0;
    sqlite3 *db;
    socklen_t len;
    struct sockaddr_in servaddr,cliaddr;
    typedef struct{
        char *x;
        char *y;
    }Posicao; 
    
    if (argc != 2)
    {    
        printf("Formato:  udpserver <PORTA>\n");
        exit(1);
    }


    /* cria socket */
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(atoi(argv[1]));
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


    for (;;)
    {
        /* recebe mensagem */
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

        /* cria struct de posicao */
        Posicao posicao;            
        
        int correctEntry = 0; 
        int index = 0;
        
        for(index=0; index < strlen(mesg); index++){
            if(mesg[index] == ':')
                correctEntry = 1;
        }
        
        if(correctEntry == 0){
            fprintf(stderr, "\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
        }
        else{
            /* Split da mensagem */          
            char** entrada = split(strtok(mesg,"\n"), ':');

            /* posicao: */
            if (strcmp(entrada[0], "posicao") == 0){
                
                /* remove \n da entrada */
                char* position = strtok(entrada[1], "\n");
                
                /* posicao:get -> imprime posicao do usuario na tela */
                if(strcmp(position, "get") == 0){
                
                    printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);            
                }
                
                /* posicao:<x>,<y> -> armazena x e y na posicao do usuario */
                else if(strcmp(position, "set") == 0){
                    
                    /* split pra pegar x e y */
                    char** posicoes = split(entrada[2], ',');      
                    int X = atoi(posicoes[0]);
                    int Y = atoi(posicoes[1]);
                    
                    if(X > 1000 || X < 0 || Y > 1000 || Y < 0){
                        
                        fprintf(stderr, "\nERRO: fora da area de cobertura do servico \n");
                    }
                    else{
                        
                        /* aloca memoria pras posicoes */
                        posicao.x = (char*)malloc(strlen(posicoes[0])*sizeof(char));
                        posicao.y = (char*)malloc(strlen(posicoes[1])*sizeof(char));    
                        /* seta posicoes */
                        strcpy(posicao.x, posicoes[0]);
                        strcpy(posicao.y, posicoes[1]);
                        fprintf(stderr, "\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);
                    }
                }
                else{
                    fprintf(stderr, "\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
                }
            }
                
            /* est: */
            else if (strcmp(entrada[0], "est") == 0){
                
                /* remove \n da entrada */
                char* entry = strtok(entrada[1], "\n");
                
                /* est:cat:<categoria> -> lista estabelecimentos da categoria <categoria> 
                 * num raio de 100 m do usuario */
                if(strcmp(entry, "cat")==0){
                    
                    /* remove \n da entrada */
                    char* category = strtok(entrada[2], "\n");
                    
                    /* aloca memoria pras posicoes do raio de 100 metro do usuario */
                    char* x_plus_100 = (char*)malloc(4*sizeof(char));
                    char* x_minus_100 = (char*)malloc(4*sizeof(char));
                    char* y_plus_100 = (char*)malloc(4*sizeof(char));
                    char* y_minus_100 = (char*)malloc(4*sizeof(char));
                    
                    /* seta posicoes */
                    sprintf(x_plus_100,"%d",((atoi(posicao.x)+ 100))); 
                    sprintf(x_minus_100,"%d",((atoi(posicao.x) - 100)));
                    sprintf(y_plus_100,"%d",((atoi(posicao.y) + 100)));
                    sprintf(y_minus_100,"%d",((atoi(posicao.y) - 100)));

                    /* monta query */
                    char* query1 = "SELECT * from estabelecimentos WHERE coord_x >= ";
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
                
                /* est:around -> lista todos estabelecimentos num raio de 100 m do usuario */
                else if (strcmp(entry, "around")==0){

                    /* aloca memoria pras posicoes do raio de 100 metro do usuario */
                    char* x_plus_100 = (char*)malloc(4*sizeof(char));
                    char* x_minus_100 = (char*)malloc(4*sizeof(char));
                    char* y_plus_100 = (char*)malloc(4*sizeof(char));
                    char* y_minus_100 = (char*)malloc(4*sizeof(char));
                    
                    /* seta posicoes */
                    sprintf(x_plus_100,"%d",((atoi(posicao.x)+ 100))); 
                    sprintf(x_minus_100,"%d",((atoi(posicao.x) - 100)));
                    sprintf(y_plus_100,"%d",((atoi(posicao.y) + 100)));
                    sprintf(y_minus_100,"%d",((atoi(posicao.y) - 100)));

                    /* monta query */
                    char* query1 = "SELECT * from estabelecimentos WHERE coord_x >= ";
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
                
                /* est:vote:<est>:<rate> -> Vota um valor <rate> ao estabelecimento de id <est> */
                else if(strcmp(entry, "vote") == 0){
                    
                    char* establishment = entrada[2];
                    char* rate = entrada[3];
                    if(atoi(rate) > 10 || atoi(rate) < 0){
                        fprintf(stderr, "\nEntrada errada: Insira um valor entre 0 e 10\n");
                    }
                    else{
                        sqlite3* db = openDB("estabelecimentos.db");            
                        char* sql = concat(5, "INSERT INTO notas VALUES (NULL, ", establishment, " ,", rate, " )");
                        executeDB(sql, db);
                        fprintf(stderr, "\nO estabelecimento %s recebeu a nota %s\n", establishment, rate);
                    }
                }
                
                /* est:getRate:<est> -> Exibe a média dos votos para o estabelecimento com id <est>*/
                else if(strcmp(entry, "getRate") == 0){
                    sqlite3* db = openDB("estabelecimentos.db");            
                    char* establishment = entrada[2];
                    char* sql = concat(2, "SELECT AVG(rate) from notas where estabId= ", establishment);
                    executeDB(sql, db);
                }

                /* est:getInfo:<est> -> Exibe informacoes gerais sobre o estabelecimento com id <est>*/
                else if (strcmp(entry, "getInfo") == 0){
                    
                    if(strlen(entrada[2]) > 3){
                        fprintf(stderr, "Entrada errada: Insira um ID de no maximo 3 digitos ou a key 'all'");
                    }
                    else{
                        /* abre bd */
                        sqlite3* db = openDB("estabelecimentos.db");            
                        
                        /* est:getInfo:all */
                        if(strcmp(entrada[2], "all") == 0){
                            
                            /* monta query */
                            char* sql = "SELECT * from estabelecimentos";
                            
                            /* executa query */
                            executeDB(sql, db);
                        }
                        /* est:getInfo:<id> */
                        else{
                            
                            /* monta query */
                            char* query = "SELECT * from estabelecimentos WHERE ID = ";
                            char* sql = concat(2, query, entrada[2]);
                            
                            /* executa query */
                            executeDB(sql, db);
                        }                        
                    }
                }
                else{
                    fprintf(stderr, "\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
                }   
            }
            else if(strcmp(entrada[0], "HELP") == 0){
                fprintf(stderr, "\nposicao:set:<x>,<y>   ->  Seta posicao do usuario\n");
                fprintf(stderr, "posicao:get             ->  Exibe posicao do usuario\n");
                fprintf(stderr, "est:cat:<cat>           ->  Lista estabelecimento em um raio de 100 m cuja categoria é <cat>\n");
                fprintf(stderr, "est:around              ->  Lista todos estabelecimentos cadastrados num raio de 100 m do usuario\n");
                fprintf(stderr, "est:vote:<id>:<rate>    ->  Atribui nota <rate> (0 a 10) ao estabelecimento cujo id é <id>\n");
                fprintf(stderr, "est:getRate:<id>        ->  Exibe a nota média do estabelecimento cujo id é <id> até o momento\n");
                fprintf(stderr, "est:getInfo:all         ->  Lista todos estabelecimentos cadastrados\n");
                fprintf(stderr, "est:getInfo:<id>        ->  Lista estabelecimento cujo ID é <id>\n");
            }
                
            else{
                fprintf(stderr, "\nEntrada errada. Digite HELP: para exibir as possiveis entradas\n");
            }
        }
   }
}

/* funcao que abre um bd */
sqlite3* openDB(char* database){    
    sqlite3* db;
    int rc;
    rc = sqlite3_open(database, &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    return db;
}


/* funcao que executa uma query no bd */
void executeDB(char* sql, sqlite3 *db){
    char *zErrMsg = 0;  
    int rc;
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    fprintf(stderr, "\n**********************************************************\n");
}


/* funcao para imprimir resultados das queries */
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    printf("\n");
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


/* funcao que concatena duas ou mais strings */
char* concat(int count, ...)
{
    va_list ap;
    int len = 1, i;

    va_start(ap, count);
    for(i=0 ; i<count ; i++)
        len += strlen(va_arg(ap, char*));
    va_end(ap);

    char *result = (char*) calloc(sizeof(char),len);
    int pos = 0;

    va_start(ap, count);
    for(i=0 ; i<count ; i++)
    {
        char *s = va_arg(ap, char*);
        strcpy(result+pos, s);
        pos += strlen(s);
    }
    va_end(ap);

    return result;
}
    
    
/* funcao para dar split em uma string */
char **split(char frase[], char separador)
{
    int i, j, k, contsep = 0;
 
    for(i=0,contsep=0;i<strlen(frase);i++)
    if(frase[i] == separador)
        contsep++;
 
    char  aux[contsep][20];
    char **result = (char**)malloc(contsep*sizeof(char));
     
    if(contsep)
    {
        for(i=0; i<=contsep; i++ )
            *(result + i) = (char*)malloc(40*sizeof(char));
         
        for(i=0,k=0,j=0; i < strlen(frase); i++)
        if(frase[i] != separador)
        {
            aux[k][j] = frase[i];
            j++;
        }
        else
        {
            aux[k][j] = 0;
            k++;
            j=0;
        }
        aux[k][j] = 0;
         
        for(i=0;i<=contsep;i++)
            *(result+i) = strcpy(*(result+i), aux[i]);
     
        return result;
    }
    else
        printf("Nenhum Separador Encontrado");
}
