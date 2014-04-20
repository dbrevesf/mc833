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
void selectDB(char* sql, sqlite3 *db);


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

    /* cria socket */
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));


    for (;;)
    {
        /* recebe mensagem */
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

        /* cria struct de posicao */
        Posicao posicao;              
        
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
            
            /* posicao:X,Y -> armazena X e Y na posicao do usuario */
            else{
                
                /* split pra pegar X e Y */
                char** posicoes = split(position, ',');      
                
                /* aloca memoria pras posicoes */
                posicao.x = (char*)malloc(strlen(posicoes[0])*sizeof(char));
                posicao.y = (char*)malloc(strlen(posicoes[1])*sizeof(char));    
                
                /* seta posicoes */
                strcpy(posicao.x, posicoes[0]);
                strcpy(posicao.y, posicoes[1]);
                
                printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);
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
                selectDB(sql6, db);
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
                selectDB(sql5, db);
            }

            /* est: */
            else{

                /* if para evitar sql injections */
                if (strlen(entry) > 4){
                    fprintf(stderr, "Entrada errada: Insira um número de 3 digitos ou 'all'.");  
                }
                
                /* est:<id> -> exibe informacoes do estabelecimento com id <id>*/
                else{
                    /* abre bd */
                    sqlite3* db = openDB("estabelecimentos.db");            
                    
                    /* est:all -> lista todos estabelecimentos cadastrados */
                    if(strcmp(entry, "all") == 0){
                        
                        /* monta query */
                        char* sql = "SELECT * from estabelecimentos";
                        
                        /* executa query */
                        selectDB(sql, db);
                    }
                    /* est:<id> -> lista informacoes do estabelecimento com id <id>*/
                    else{
                        
                        /* monta query */
                        char* query = "SELECT * from estabelecimentos WHERE ID = ";
                        char* sql = concat(2, query, entry);
                        
                        /* executa query */
                        selectDB(sql, db);
                    }
                }    
            }   
        }
        else if(strcmp(entrada[0], "HELP") == 0){
            fprintf(stderr, "posicao:<x>,<y>   ->  Seta posicao do usuario\n");
            fprintf(stderr, "posicao:get       ->  Exibe posicao do usuario\n");
            fprintf(stderr, "est:all           ->  Lista todos estabelecimentos cadastrados\n");
            fprintf(stderr, "est:around           ->  Lista todos estabelecimentos cadastrados num raio de 100 m do usuario\n");
            fprintf(stderr, "est:<id>          ->  Lista estabelecimento cujo ID é <id>\n");
            fprintf(stderr, "est:cat:<cat>     ->  Lista estabelecimento em um raio de 100 m cuja categoria é <cat>\n");
        }
            
        else{
            fprintf(stderr, "Entrada errada. Digite HELP: para exibir as possiveis entradas");
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


/* funcao que executa um select no bd */
void selectDB(char* sql, sqlite3 *db){
    char *zErrMsg = 0;  
    int rc;
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}


/* funcao para imprimir resultados das queries */
static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
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
