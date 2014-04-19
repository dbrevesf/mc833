
/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split(char* string, char delimiter);

int main(int argc, char**argv)
{
    int sockfd,n, i;
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
        }
            
   }

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
