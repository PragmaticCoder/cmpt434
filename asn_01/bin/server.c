#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "dictionary.h"

#define MAXPENDING 2
#define arrayLength(x) (sizeof((x))/sizeof((x)[0])
char* 
HandleDictionary(char *arg)
{
    char *ptr;
    char *argV[10];
    int i = 0;
    for(char *token = strtok_r(arg, " \n\r", &ptr); token; token = strtok_r(NULL, " \n\r", &ptr)) {
      argV[i++] = token;
      if(i >= (arrayLength(argV) - 1)) {
        break;
      } 
    }
    argV[i] = NULL;
    Command_interface(i, argV, arg);
    log_info("%s\n", arg);
    return arg;
}


void 
HandleClient(int client)
{
    char receiveBuffer[1024];
    int  receivedLength = 0;
    int  sentLength = 0;
    while(1){
      memset(receiveBuffer, 0, sizeof(receiveBuffer));
      if((receivedLength = recv(client, receiveBuffer, sizeof(receiveBuffer) -1 , 0)) < 0) {
        log_err("Error in Receving the Character from TCP client\n"); 
        break;
      }
	
      if(receivedLength == 0) {
        log_err("client lost connection\n");
        break;
      }

      log_info("Data received : %s\nReceived Length : %d\n", receiveBuffer, receivedLength);
      HandleDictionary(receiveBuffer);
		
      if((sentLength = send(client, receiveBuffer, strlen(receiveBuffer), 0)) < strlen(receiveBuffer)) {
        log_err("sent lesser number of character %d\n", receivedLength); 
        break; 
      }
    }
}


int main(int argc, char *argv[]){
	int serverFD, clientFD;
	struct sockaddr_in serverSock, clientSock;
	
	if (argc < 2) { log_err("Too few Argument"); return 1;}

	if((serverFD = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		log_err("Error in opening the Socket connection of TCP Server\n");
		exit(1);
	}
	
	memset(&serverSock, 0, sizeof(struct sockaddr_in));
	
	serverSock.sin_family = AF_INET;	
	serverSock.sin_addr.s_addr = htonl(INADDR_ANY);
	serverSock.sin_port = htons(atoi(argv[1]));
	
	if(bind(serverFD, (struct sockaddr *)&serverSock, sizeof(struct sockaddr_in)) < 0) {	
		log_err("Error in binding the Socket connection of TCP Server\n");
		exit(1);
	}
	if(listen(serverFD, MAXPENDING) < 0) {
		log_err("Error in listing the Socket connection of TCP Server\n");
		exit(1);	
	}
	
	while(1){
		memset(&serverSock, 0, sizeof(struct sockaddr_in));
		unsigned int clientSize = sizeof(struct sockaddr_in);

		if((clientFD = accept(serverFD, (struct sockaddr *)&clientSock, &clientSize)) < 0) {
			log_err("Error in Accepting the Socket connection of TCP Server\n");
			exit(1);
		}

		log_info("Client Address : %s Port : %d\n", inet_ntoa(clientSock.sin_addr), ntohs(clientSock.sin_port));
		HandleClient(clientFD);
		close(clientFD);
	}
	close(serverFD);
}
