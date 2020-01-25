#undef NDEBUG

#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define BUFFSIZE 1024
#define MAX_CONNECTION 5

int
setup_server(const char* host, const char* port)
{

  int sockfd;
  struct sockaddr_in server_address;

  /* create the server socket */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  /* configure server address */
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(30000);
  server_address.sin_addr.s_addr = inet_addr(host);

  /* bind the socket to our specified IP and port */
  bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address));

  return sockfd;

error:
  freeaddrinfo(server_address);
  return (-1);
}

int
main(int argc, char const* argv[])
{

  check(argc == 3, "USAGE: ./socket_v1 <host> <port>");
  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int conn = 0;
  conn = setup_server(argv[1], argv[2]);
  check(conn >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

  /* Listening to incoming connections */
  listen(conn, MAX_CONNECTION);

  int client_socket;
  client_socket = accept(conn, NULL, NULL);

  /* send the message */
  char message[BUFFSIZE] = "You have reached the server!\0";

  int status = 0;
  status = send(client_socket, message, sizeof(message), 0);
  check(status != -1, "Error received while sending.");

  /* close the socket */
  close(conn);

  return (0);

error:
  return (-1);
}