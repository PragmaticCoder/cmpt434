#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char* argv[]){

	int clientFD, fromSize = sizeof(struct sockaddr_in);

	struct sockaddr_in clientSock;
	struct sockaddr_in fromSock;

	char buffer[100];
	if(argc < 3) { printf("Too few Argument"); return 1;}

	if((clientFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {}
	
	memset (&clientSock, 0, sizeof(struct sockaddr_in ));
	
	clientSock.sin_family = AF_INET;
	clientSock.sin_addr.s_addr = inet_addr(argv[1]);
	clientSock.sin_port = htons(atoi(argv[2]));

	while(1) {
	scanf("%s", buffer);

	if(sendto(clientFD, buffer, strlen(buffer), 0, (struct sockaddr *)&clientSock, sizeof(struct sockaddr)) != strlen(buffer)){}
	memset(buffer, 0, sizeof(buffer));
	memset(&fromSock, 0, sizeof(struct sockaddr_in ));

	recvfrom(clientFD, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&fromSock, &fromSize);
	printf("Received Data : %s\n", buffer);
	}

	close(clientFD);
}
