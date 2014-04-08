
/* Sample UDP server */

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split(char* string, char delimiter);

int main(int argc, char**argv)
{
   int sockfd,n;
   struct sockaddr_in servaddr,cliaddr;
   socklen_t len;
   char mesg[1000];

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
      

      printf("TAMNHO DA STRING: %ld", sizeof(mesg));


      printf("Received the following:\n");
      char** strings = split(mesg, ',');
      printf("-------------------------------------------------------\n");
      
   
   }

}

char** split(char* message, char delimiter){

   int i = 0, numeroStrings = 0;
   char* auxString = NULL;
   char** response = NULL;
   
   for(i=0; i<sizeof(message); i++){
     
      printf("\n%d %c\n", i, message[i]);
     /* if (string[i] != delimiter || string[i] != '\n'){
         auxString = (char*)realloc(auxString, (i+1)*sizeof(char));         
         auxString[i] = string[i];
      }
      else{
         numeroStrings++;
         i++;
         int j = 0;
         response = (char**)realloc(response, numeroStrings*sizeof(char*));
         response[numeroStrings] = (char*)malloc(sizeof(auxString));
         strcpy(response[numeroStrings], auxString);
      }*/
   }

   printf("SAI FORA");
   return response;

}

