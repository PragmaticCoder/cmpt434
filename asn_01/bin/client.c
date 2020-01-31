#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	
	int socketFD;
	struct sockaddr_in clientAddress;	

	if (argc < 3) { 
		printf("Too few Argument"); 
		exit(1);
	}

	if((socketFD = socket(PF_INET, SOCK_STREAM, 0)) < 0){ 
		printf("Failed to create Socket"); 
		exit(1); 
	}
	
	memset(&clientAddress, 0, sizeof(struct sockaddr_in));
	
	clientAddress.sin_family = AF_INET;	
	clientAddress.sin_addr.s_addr   = inet_addr(argv[1]);
	clientAddress.sin_port   = htons(atoi(argv[2]));

	if(connect(socketFD, (struct sockaddr *)&clientAddress, sizeof(struct sockaddr)) < 0) {
		printf("Failed to connect to the given Address"); 
		exit(1); 
	}
	
	int sentLength;
	char receiveBuffer[1024];
	int receivedlength = 0;
	while(1) {
		char *ptr = fgets(receiveBuffer, sizeof(receiveBuffer), stdin);
		if(!memcmp(ptr, "quit", 4)) { break; }
		if((sentLength = send(socketFD, ptr, strlen(ptr), 0)) != strlen(ptr)) {
			printf("sent lower number of send character"); 
			exit(1); 
		}
		printf("send Data : %s\nlength : %d\n", ptr, sentLength);

		printf("Received : \n");
		receivedlength = 0;
		
		if((receivedlength = recv(socketFD, ptr, sizeof(receiveBuffer) - 1, 0)) <= 0) { 
			printf("received lesser number of character\n");
			exit(1);
		}
		receiveBuffer[receivedlength] = 0;
		printf("%s", ptr);
		printf("\n");
	}
	close(socketFD);
}
