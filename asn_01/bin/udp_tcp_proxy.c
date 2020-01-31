#include <dbg.h>

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXPENDING 2
struct sockaddr_in clientSock;

void
Handle_client(int client, int server)
{
  int fromSize;
  int recvLength;
  int sentLength;
  char buffer[100];
  struct sockaddr_in fromSock;

  while (1) {

    if ((recvLength = recv(client, buffer, sizeof(buffer) - 1, 0)) < 0) {
      printf("Error in Receving the Character from TCP client\n");
      break;
    }

    if (recvLength == 0) {
      printf("client lost connection\n");
      break;
    }
    buffer[recvLength] = 0;
    printf("Received TCP Data : %s\n", buffer);

    if ((sentLength = sendto(server,
                             buffer,
                             recvLength,
                             0,
                             (struct sockaddr*)&clientSock,
                             sizeof(struct sockaddr))) != recvLength) {
      printf("Error in Sending data to UDP client\n");
      break;
    }

    fromSize = sizeof(struct sockaddr);
    if ((recvLength = recvfrom(server,
                                    buffer,
                                    sizeof(buffer) - 1,
                                    0,
                                    (struct sockaddr*)&fromSock,
                                    &fromSize)) < 0) {
      printf("Error in Receving the Character from UDP server\n");
      break;
    }

    buffer[recvLength] = 0;
    printf("Received UPD Data : %s\n", buffer);

    printf("Received TCP Data from Server : %s\n", buffer);

    strcat(buffer, " - from the Proxy");
    recvLength = strlen(buffer);

    if ((sentLength = send(client, buffer, recvLength, 0)) !=
        recvLength) {
      printf("Error in Sending data to TCP client\n");
      break;
    }
  }
  close(client);
}

int
main(int argc, char* argv[])
{

  int clientFD, serverFD, tcpClientFD, fromSize = sizeof(struct sockaddr_in);

  struct sockaddr_in serverSock;
  struct sockaddr_in tcpClientSock;

  check(argc == 3, "Please provide three arguments.");

  check((clientFD = socket(PF_INET, SOCK_DGRAM, 0)) >= 0,
        "Error opening UDP client socket connection");

  check((serverFD = socket(PF_INET, SOCK_STREAM, 0)) >= 0,
        "Error opening UDP server socket connection");

  memset(&clientSock, 0, sizeof(struct sockaddr_in));

  clientSock.sin_family = AF_INET;
  clientSock.sin_addr.s_addr = inet_addr(argv[1]);
  clientSock.sin_port = htons(atoi(argv[2]));

  memset(&serverSock, 0, sizeof(struct sockaddr_in));

  serverSock.sin_family = AF_INET;
  serverSock.sin_addr.s_addr = htonl(INADDR_ANY);
  serverSock.sin_port = htons(atoi(argv[3]));

  check((bind(serverFD,
              (struct sockaddr*)&serverSock,
              sizeof(struct sockaddr_in)) >= 0),
        "Error in binding the Socket connection of TCP Server");

  check(listen(serverFD, MAXPENDING) >= 0,
        "Error in listing the Socket connection of TCP Server");

  while (1) {

    check(((tcpClientFD = accept(
              serverFD, (struct sockaddr*)&tcpClientSock, &fromSize)) >= 0),
          "Error in Accepting the Socket connection of TCP Server");

    log_info("IP : %s Port %d\n\n",
             inet_ntoa(tcpClientSock.sin_addr),
             ntohs(tcpClientSock.sin_port));

    Handle_client(tcpClientFD, clientFD);
  }

  close(clientFD);

  return 0;

error:
  return (-1);
}
