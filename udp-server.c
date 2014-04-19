
/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

char** split(char* string, char delimiter);
static int callback(void *NotUsed, int argc, char **argv, char **azColName);
char* concat(int count, ...);
sqlite3* openDB(char* database);
void selectDB(char* sql, sqlite3 *db);
static int callback(void *data, int argc, char **argv, char **azColName);
char* normalizeCoordinate(char* coordinate);


int main(int argc, char**argv)
{
    int sockfd,n;   
    
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    struct sockaddr_in servaddr,cliaddr;
    socklen_t len;
    char mesg[1000];

    typedef struct{
        char *x;
        char *y;
    }Posicao; 

    

    sockfd=socket(AF_INET,SOCK_DGRAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(32000);
    bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));



    for (;;)
    {
        len = sizeof(cliaddr);
        n = recvfrom(sockfd,mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
        sendto(sockfd,mesg,n,0,(struct sockaddr *)&cliaddr,sizeof(cliaddr));

        Posicao posicao;
                        
        char** entrada = split(mesg, ':');

        if (strcmp(entrada[0], "posicao") == 0){
            char* position = strtok(entrada[1], "\n");
            
            if(strcmp(position, "get") == 0){
                printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);            
            }
            else{
                char** posicoes = split(position, ',');      
                posicao.x = (char*)malloc(strlen(posicoes[0])*sizeof(char));
                posicao.y = (char*)malloc(strlen(posicoes[1])*sizeof(char));    
                strcpy(posicao.x, posicoes[0]);
                strcpy(posicao.y, posicoes[1]);
                printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);
            }
        }

        else if (strcmp(entrada[0], "est") == 0){
            char* entry = strtok(entrada[1], "\n");

            if(strcmp(entry, "cat")==0){
                char* category = strtok(entrada[2], "\n");
                
                char* x_plus_100 = (char*)malloc(4*sizeof(char));
                char* x_minus_100 = (char*)malloc(4*sizeof(char));
                char* y_plus_100 = (char*)malloc(4*sizeof(char));
                char* y_minus_100 = (char*)malloc(4*sizeof(char));
                
                sprintf(x_plus_100,"%d",((atoi(posicao.x)+ 100))); 
                sprintf(x_minus_100,"%d",((atoi(posicao.x) - 100)));
                sprintf(y_plus_100,"%d",((atoi(posicao.y) + 100)));
                sprintf(y_minus_100,"%d",((atoi(posicao.y) - 100)));

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
                sqlite3* db = openDB("estabelecimentos.db");
                selectDB(sql6, db);
            }
            
            else if (strcmp(entry, "around")==0){

                char* x_plus_100 = (char*)malloc(4*sizeof(char));
                char* x_minus_100 = (char*)malloc(4*sizeof(char));
                char* y_plus_100 = (char*)malloc(4*sizeof(char));
                char* y_minus_100 = (char*)malloc(4*sizeof(char));
                
                sprintf(x_plus_100,"%d",((atoi(posicao.x)+ 100))); 
                sprintf(x_minus_100,"%d",((atoi(posicao.x) - 100)));
                sprintf(y_plus_100,"%d",((atoi(posicao.y) + 100)));
                sprintf(y_minus_100,"%d",((atoi(posicao.y) - 100)));

                char* query1 = "SELECT * from estabelecimentos WHERE coord_x >= ";
                char* query2 = " AND coord_x <= ";                                 
                char* query3 = " AND coord_y >= ";
                char* query4 = " AND coord_y <= ";
                
                char* sql1 = concat(2, query1, x_minus_100);
                char* sql2 = concat(3, sql1, query2, x_plus_100);
                char* sql4 = concat(3, sql2, query3, y_minus_100);
                char* sql5 = concat(3, sql4, query4, y_plus_100); 
                sqlite3* db = openDB("estabelecimentos.db");
                selectDB(sql5, db);
            }


            else{

                if (strlen(entry) > 4){
                    fprintf(stderr, "Entrada errada: Insira um número de 3 digitos ou 'all'.");  
                }else{
                    sqlite3* db = openDB("estabelecimentos.db");            
                    if(strcmp(entry, "all") == 0){
                        char* sql = "SELECT * from estabelecimentos";
                        selectDB(sql, db);
                    }else{
                        char* query = "SELECT * from estabelecimentos WHERE ID = ";
                        char* sql = concat(2, query, entry);
                        selectDB(sql, db);
                    }
                         
                }    
            }   
        }        
   }
}


sqlite3* openDB(char* database){
    sqlite3* db;
    int rc;

    /* Open database */
    rc = sqlite3_open(database, &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }
    return db;
}


void selectDB(char* sql, sqlite3 *db){
    char *zErrMsg = 0;  
    int rc;

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
}


static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}


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

char* normalizeCoordinate(char* coordinate){
    int c = atoi(coordinate);
    if (c<0){
        c = 1000 + c;
    }else{
        c = c;
    }
    char* resposta = (char*)malloc(3*sizeof(char));
    sprintf(resposta, "%d", (c%1000));
    return resposta;
}
    

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
