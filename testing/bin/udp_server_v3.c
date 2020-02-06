/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */

#include "commandLine.h"
#include <arpa/inet.h>
#include <dbg.h>
#include <dictionary.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

char*
HandleClient(char* arg);
int
main(int argc, char* argv[])
{

  int serverFD;
  char receiveBuffer[1024];
  struct sockaddr_in serverSock;
  struct sockaddr_in clientSock;

  check(argc >= 2, "Too few Argument");

  check((serverFD = socket(PF_INET, SOCK_DGRAM, 0)) >= 0,
        "Error in opening the Socket connection of UDP Server");

  memset(&serverSock, 0, sizeof(struct sockaddr));

  serverSock.sin_family = AF_INET;
  serverSock.sin_addr.s_addr = htonl(INADDR_ANY);
  serverSock.sin_port = htons(atoi(argv[1]));

  memset(&clientSock, 0, sizeof(struct sockaddr));

  check(
    bind(serverFD, (struct sockaddr*)&serverSock, sizeof(struct sockaddr)) >= 0,
    "Error in binding the Socket connection of UDP Server");

  while (1) {
    int receivedLength = 0;
    int sentLength = 0;
    unsigned int clientSize = sizeof(struct sockaddr_in);

    memset(receiveBuffer, 0, sizeof(receiveBuffer));

    check((receivedLength = recvfrom(serverFD,
                                     receiveBuffer,
                                     sizeof(receiveBuffer) - 1,
                                     0,
                                     (struct sockaddr*)&clientSock,
                                     &clientSize)) >= 0,
          "Error in Receivng the Character from UDP client");

    log_info("host : %s Port : %d",
             inet_ntoa(clientSock.sin_addr),
             ntohs(clientSock.sin_port));

    log_info("Received Data : %s", receiveBuffer);

    HandleClient(receiveBuffer);

    if ((sentLength = sendto(serverFD,
                             receiveBuffer,
                             strlen(receiveBuffer),
                             0,
                             (struct sockaddr*)&clientSock,
                             sizeof(struct sockaddr))) < 0) {
      printf("Error in Sending data to UDP client\n");
      break;
    }
  }
  return (0);
error:
  return (-1);
}

char*
HandleClient(char* arg)
{
  char* argV[10];
  int i = 0;
  for (char* token = strtok(arg, " \n\r"); token;
       token = strtok(NULL, " \n\r")) {
    argV[i++] = token;
    if (i >= ((sizeof(argV) / sizeof(argV[0])) - 1)) {
      break;
    }
  }
  argV[i] = NULL;
  Command_interface(i, argV, arg);
  printf("%s\n", arg);

  return arg;
}
