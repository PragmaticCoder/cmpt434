#undef NDEBUG
#include <arpa/inet.h>
#include <dbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXPENDING 2
struct sockaddr_in clientSock;

void
HandleClient(int client, int server);

int
main(int argc, char* argv[])
{

  int clientFD, serverFD, tcpClientFD;
  int fromSize;
  struct sockaddr_in serverSock;
  struct sockaddr_in tcpClientSock;

  if (argc < 4) {
    printf("Too few Argument");
    return 1;
  }

  if ((clientFD = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Error in opening the Socket connection of TCP client\n");
    exit(1);
  }
  if ((serverFD = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Error in opening the Socket connection of TCP Server\n");
    exit(1);
  }

  memset(&clientSock, 0, sizeof(struct sockaddr_in));

  clientSock.sin_family = AF_INET;
  clientSock.sin_addr.s_addr = inet_addr(argv[1]);
  clientSock.sin_port = htons(atoi(argv[2]));

  memset(&serverSock, 0, sizeof(struct sockaddr_in));

  serverSock.sin_family = AF_INET;
  serverSock.sin_addr.s_addr = htonl(INADDR_ANY);
  serverSock.sin_port = htons(atoi(argv[3]));

  if (bind(serverFD,
           (struct sockaddr*)&serverSock,
           sizeof(struct sockaddr_in)) < 0) {
    printf("Error in binding the Socket connection of TCP Server\n");
    exit(1);
  }
  if (listen(serverFD, MAXPENDING) < 0) {
    printf("Error in listing the Socket connection of TCP Server\n");
    exit(1);
  }

  if (connect(
        clientFD, (struct sockaddr*)&clientSock, sizeof(struct sockaddr)) < 0) {
    printf("Failed to connect to the desination Address");
    exit(1);
  }

  while (1) {
    fromSize = sizeof(struct sockaddr);
    if ((tcpClientFD =
           accept(serverFD, (struct sockaddr*)&tcpClientSock, &fromSize)) < 0) {
      printf("Error in Accepting the Socket connection of TCP Server\n");
      exit(1);
    }
    printf("IP : %s Port %d\n\n",
           inet_ntoa(tcpClientSock.sin_addr),
           ntohs(tcpClientSock.sin_port));
    HandleClient(tcpClientFD, clientFD);
    close(tcpClientFD);
  }

  close(clientFD);
}

void
HandleClient(int client, int server)
{
  int RecevicedLength;
  int sentLength;
  char buffer[100];
  struct sockaddr_in fromSock;

  while (1) {

    if ((RecevicedLength = recv(client, buffer, sizeof(buffer) - 1, 0)) < 0) {
      printf("Error in Receving the Character from TCP client\n");
      break;
    }

    if (RecevicedLength == 0) {
      printf("client lost connection\n");
      break;
    }
    buffer[RecevicedLength] = 0;
    printf("Received TCP Data to Server : %s\n", buffer);

    if ((sentLength = send(server, buffer, RecevicedLength, 0)) !=
        RecevicedLength) {
      printf("Error in Sending data to TCP Server\n");
      break;
    }

    if ((RecevicedLength = recv(server, buffer, sizeof(buffer) - 1, 0)) < 0) {
      printf("Error in Receving data from TCP server\n");
      break;
    }
    buffer[RecevicedLength] = 0;
    printf("Received TCP Data from Server : %s\n", buffer);

    strcat(buffer, " - from the Proxy");
    RecevicedLength = strlen(buffer);

    if ((sentLength = send(client, buffer, RecevicedLength, 0)) !=
        RecevicedLength) {
      printf("Error in Sending data to TCP client\n");
      break;
    }
  }
}
