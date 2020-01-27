#undef NDEBUG

#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define MAX 1024
#define MAX_CONNECTION 10

int
setup_server(int port)
{

  /* create the server socket */
  int sockfd = 0;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd >= 0, "Socket creation failed");

  /* configure server address */
  struct sockaddr_in serv_addr;
  bzero((char*)&serv_addr, sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  log_info("Attempting to connnet to %s:%d",
           serv_addr.sin_addr.s_addr,
           ntohs(serv_addr.sin_port));

  /* bind the socket to specified port */
  check(bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) >= 0,
        "Error detected during binding");
  return sockfd;

error:
  return (-1);
}

int
main(int argc, char const* argv[])
{

  check(argc == 2, "USAGE: ./socket_v1 <port>");

  int port = 30000; /* default port number */
  port = atoi(argv[1]);

  int sockfd = 0;
  sockfd = setup_server(port);
  check(sockfd >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

  /* Listening to incoming connections */
  listen(sockfd, MAX_CONNECTION);
  log_info("Connection Successful!");

  int cli_sockfd;
  struct cli_addr;
  socklen_t clilen;

  cli_sockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
  check(sockfd >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

  /* Client-Server Communication */
  char buf[MAX] = NULL;

  while (1) {
    /* Receive message from client */
    bzero(buf, MAX);

    int status = 0;
    status = read(cli_sockfd, buf, sizeof(buf));
    check(status >= 0, "Error received while receiving message from client.");

    printf("CLIENT: %s\n", buf);

    /* Reply to client */
    bzero(buf, MAX);
    fgets(buf, sizeof(buf), stdin);

    n = write(cli_sockfd, buf, strlen(buf));
    check(n >= 0, "Error while writing to Socket");

    if (strncmp("quit", buf, 4) == 0)
      break;
  }

  /* close all socket */
  close(cli_sockfd);
  close(sockfd);

  return (0);

error:
  debug("Error Detected in Server Program!");
  return (-1);
}