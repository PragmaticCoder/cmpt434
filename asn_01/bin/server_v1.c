#undef NDEBUG

#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define BUFFSIZE 12005
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

  int conn = 0;
  conn = setup_server(port);
  check(conn >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

  /* TODO: Configuring Client stuff here */
  // struct sockaddr_in cli_addr;
  // socklen_t clilen;

  /* Listening to incoming connections */
  listen(conn, MAX_CONNECTION);

  log_info("Connection Successful!");

  int client_socket;
  client_socket = accept(conn, NULL, NULL);

  while (1) {
    /* send the message */
    char message[BUFFSIZE] = "You have reached the server!\0";

    int status = 0;
    status = send(client_socket, message, sizeof(message), 0);
    check(status != -1, "Error received while sending.");

    char response[BUFFSIZE];
    recv(conn, &response, sizeof(response), 0);

    /* printing the data received from client */
    log_info("Response data from server: %s", response);
    sleep(1);
  }
  /* close the socket */
  close(conn);

  return (0);

error:
  debug("Error Detected in Server Program!");
  return (-1);
}