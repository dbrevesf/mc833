
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

    Posicao posicao;
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

                
        char** entrada = split(mesg, ':');
        if (strcmp(entrada[0], "posicao") == 0){
            char** posicoes = split(entrada[1], ',');      
            posicao.x = (char*)malloc(strlen(posicoes[0])*sizeof(char));
            posicao.y = (char*)malloc(strlen(posicoes[1])*sizeof(char));    
            strcpy(posicao.x, posicoes[0]);
            strcpy(posicao.y, posicoes[1]);
            printf("\nPosicao do usuario-> X: %s Y: %s \n", posicao.x, posicao.y);
        }

        else if (strcmp(entrada[0], "est") == 0){
            sqlite3* db = openDB("estabelecimentos.db");
            char* query = "SELECT * from estabelecimentos WHERE ID = ";
            char* filter = concat(2, query, entrada[1]);
            char* sql = strtok(filter, "\n");
            printf("SQL %s", sql);           

            //selectDB(sql, db);            
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
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    return db;
}


void selectDB(char* sql, sqlite3 *db){
    char *zErrMsg = 0;  
    int rc;
    const char* data = "Callback function called";

    /* Execute SQL statement */
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
}


static int callback(void *data, int argc, char **argv, char **azColName){
    int i;
    fprintf(stderr, "%s: ", (const char*)data);
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
