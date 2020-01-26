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
connect_client(const char* host, const char* port)
{
  /* Socket creation */
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd != -1, "Socket creation failed!");

  /* Specify destination address */
  struct sockaddr_in server_addr;
  bzero(&server_addr, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr(host);
  server_addr.sin_port = port;

  /* Establish connection client & server socket */
  int status = 0;
  status = connect(sockfd, (SA*)&server_addr, sizeof(server_addr));
  check(status == 0, "Connection to server failed ...");

  return sockfd;

error:
  freeaddrinfo(server_addr);
  return (-1);
}

int
main(int argc, char const* argv[])
{

  check(argc == 3, "USAGE: ./client_v1 <host> <port>");
  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int sockfd = 0;
  sockfd = connect_client(argv[1], argv[2]);
  check(sockfd >= 0, "Connection to Server %s:%s Failed", argv[1], argv[2]);

  chat(sockfd);

  close(sockfd);
  return (0);

error:
  return (-1);
}