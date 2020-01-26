#include <dbg.h>

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 1024
#define MAXLINE 100
#define SA struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

struct hostent* server;

int
connect_server(const char* host, const char* port)
{
  /* Socket creation */
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd != -1, "Socket creation failed!");

  /* Specify destination address */
  struct sockaddr_in serv_addr;
  bzero((char*)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);

  server = gethostbyname(host);
  check(server != NULL, "ERROR, no such host");

  bcopy(
    (char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);

  /* Establish connection client & server socket */
  check(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != -1,
        "Connection to server failed ...");

  return sockfd;

error:
  return (-1);
}

void
error(const char* msg)
{
  perror(msg);
  exit(0);
}

int
main(int argc, char const* argv[])
{

  check(argc == 3, "USAGE: ./client_v1 <host> <port>");
  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int port = 0;
  port = atoi(argv[2]);

  char buffer[MAX];

  int sockfd = 0;
  sockfd = connect_server(argv[1], port);

  printf("Client: ");

  while (1) {
    bzero(buffer, MAX);
    fgets(buffer, MAX - 1, stdin);

    int n = 0;
    n = write(sockfd, buffer, strlen(buffer));

    check(n >= 0, "Error while writing to Socket");

    bzero(buffer, MAX);
    n = read(sockfd, buffer, MAX - 1);

    check(n >= 0, "Error while reading from Socket");

    printf("Server : %s\n", buffer);

    int i = 0;
    i = strncmp("Exit", buffer, 4);

    if (i == 0)
      break;
  }

  close(sockfd);

  return 0;

error:
  debug("ERROR DETECTED!");
  return (-1);
}