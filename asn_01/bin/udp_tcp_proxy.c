#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXPENDING 2
struct sockaddr_in clientSock;

void HandleClient(int client, int server);

int main(int argc, char* argv[]){

	int clientFD, serverFD, tcpClientFD, fromSize = sizeof(struct sockaddr_in);	
	
	struct sockaddr_in serverSock;
	struct sockaddr_in tcpClientSock;

	
	if(argc < 4) { printf("Too few Argument"); return 1;}

	if((clientFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Error in opening the Socket connection of UDP client\n");
		exit(1);	
	}
	if((serverFD = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error in opening the Socket connection of TCP Server\n");
		exit(1);
	}

	memset (&clientSock, 0, sizeof(struct sockaddr_in ));
	
	clientSock.sin_family = AF_INET;
	clientSock.sin_addr.s_addr = inet_addr(argv[1]);
	clientSock.sin_port = htons(atoi(argv[2]));
	
	memset (&serverSock, 0, sizeof(struct sockaddr_in));

	serverSock.sin_family = AF_INET;
	serverSock.sin_addr.s_addr = htonl(INADDR_ANY);
	serverSock.sin_port = htons(atoi(argv[3]));
	
	if(bind(serverFD, (struct sockaddr *)&serverSock, sizeof(struct sockaddr_in)) < 0) {	
		printf("Error in binding the Socket connection of TCP Server\n");
		exit(1);
	}
	if(listen(serverFD, MAXPENDING) < 0) {
		printf("Error in listing the Socket connection of TCP Server\n");
		exit(1);	
	}

	while(1) {
	
	if((tcpClientFD = accept(serverFD, (struct sockaddr *)&tcpClientSock, &fromSize)) < 0) {
		printf("Error in Accepting the Socket connection of TCP Server\n");
		exit(1);	
	}
	printf("IP : %s Port %d\n\n", inet_ntoa(tcpClientSock.sin_addr), ntohs(tcpClientSock.sin_port));
	HandleClient(tcpClientFD, clientFD);
	}

	close(clientFD);
}

void HandleClient(int client, int server){
	int fromSize;
	int RecevicedLength;
	int sentLength;
	char buffer[100];
	struct sockaddr_in fromSock;

	while(1) {
	
		if((RecevicedLength = recv(client, buffer, sizeof(buffer) -1, 0)) < 0) {
			printf("Error in Receving the Character from TCP client\n"); 
			break;
		}
		
		if(RecevicedLength == 0) {
			printf("client lost connection\n");
			break;
		}
		buffer[RecevicedLength] = 0;
		printf("Received TCP Data : %s\n", buffer);

		if((sentLength = sendto(server, buffer, RecevicedLength, 0, (struct sockaddr *)&clientSock, sizeof(struct sockaddr))) != RecevicedLength){
			printf("Error in Sending data to UDP client\n");
			break;
		}
		
		fromSize = sizeof(struct sockaddr);
		if((RecevicedLength = recvfrom(server, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&fromSock, &fromSize)) < 0) {
			printf("Error in Receving the Character from UDP server\n"); 
			break;	
		}
		
		buffer[RecevicedLength] = 0;
		printf("Received UPD Data : %s\n", buffer);
		
		printf("Received TCP Data from Server : %s\n", buffer);

		strcat(buffer, " - from the Proxy");
		RecevicedLength = strlen(buffer);				
		
		if((sentLength = send(client, buffer, RecevicedLength, 0)) != RecevicedLength) { 
			printf("Error in Sending data to TCP client\n");
			break;
		}
	}
	close (client);
}
