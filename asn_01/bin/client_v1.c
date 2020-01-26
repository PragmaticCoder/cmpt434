#include <dbg.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include <unistd.h>

#define MAX 1024
#define PORT 61187
#define SA struct sockaddr

void
chat(int sockfd)
{
  char buff[MAX];
  int n;
  for (;;) {
    bzero(buff, sizeof(buff));
    printf("Enter the string : ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
      ;
    // write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    if ((strncmp(buff, "exit", 4)) == 0) {
      printf("Client Exit...\n");
      break;
    }
  }
}

int
main(int argc, char const* argv[])
{

  check(argc == 3, "USAGE: ./client_v1 <host> <port>");
  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int sockfd = 0;
  struct sockaddr_in servaddr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd != -1, "Socket creation failed!");

  bzero(&servaddr, sizeof(servaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr(argv[1]);
  servaddr.sin_port = argv[2];

  /* connect client socket to server socket */
  int status = 0;
  status = connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
  check(status == 0, "Connection to server failed ...");

  chat(sockfd);

  close(sockfd);
  return (0);

error:
  return (-1);
}