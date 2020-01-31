#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	
	int serverFD; 
	char receiveBuffer[100];
	struct sockaddr_in serverSock;
	struct sockaddr_in clientSock;
	
	if (argc < 2) { 
		printf("Too few Argument"); 
		exit(1);
	}
	
	if((serverFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0){ 
		printf("Error in opening the Socket connection of UDP Server\n");
		exit(1);
	}
	
	memset(&serverSock, 0, sizeof(struct sockaddr));
	
	serverSock.sin_family = AF_INET;
	serverSock.sin_addr.s_addr= htonl(INADDR_ANY);
	serverSock.sin_port = htons(atoi(argv[1]));
	

	memset(&clientSock, 0, sizeof(struct sockaddr));
	if(bind(serverFD, (struct sockaddr *)&serverSock, sizeof(struct sockaddr)) < 0) { 
		printf("Error in binding the Socket connection of UDP Server\n"); 
		exit(1); 
	}
	
	while(1){
		int receivedLength = 0;
		int sentLength = 0;
		int clientSize = sizeof(struct sockaddr_in);

		memset(receiveBuffer, 0, sizeof(receiveBuffer));
		if((receivedLength = recvfrom(serverFD, receiveBuffer, sizeof(receiveBuffer)-1, 0, (struct sockaddr *)&clientSock, &clientSize)) < 0){
			printf("Error in Receving the Character from UDP client\n"); 
			exit(1);		
		}
		printf("host : %s Port : %d\n", inet_ntoa(clientSock.sin_addr), ntohs(clientSock.sin_port));
		printf("Received Data : %s\n", receiveBuffer);
		
		if((sentLength = sendto(serverFD,  receiveBuffer, receivedLength, 0, (struct sockaddr *)&clientSock, sizeof(struct sockaddr))) < 0){
			printf("Error in Sending data to UDP client\n");
			break;
		}
	}
}

char* HandleClient(char *data){
	return data;
}