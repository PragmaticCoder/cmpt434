#undef NDEBUG
#include <dbg.h>

#include <netdb.h>
#include <netinet/in.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 1024
#define SOCKET_ADDR struct sockaddr
#define h_addr h_addr_list[0] /* for backward compatibility */

struct hostent* server;

int
connect_server(const char* host, uint32_t port)
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
  check(connect(sockfd, (SOCKET_ADDR*)&serv_addr, sizeof(serv_addr)) != -1,
        "Connection to server failed ...");

  return sockfd;

error:
  return (-1);
}

int
main(int argc, char const* argv[])
{

  check(argc == 3, "USAGE: ./client_v1 <host> <port>");
  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int port = 0;
  port = atoi(argv[2]);

  int sockfd = 0;
  sockfd = connect_server(argv[1], port);

  char buf[MAX];
  int n = 0;

  while (1) {
    bzero(buf, MAX);

    /* getting user input */
    printf("CLIENT: ");
    fgets(buf, MAX - 1, stdin);

    n = write(sockfd, buf, strlen(buf));
    check(n >= 0, "Error while writing to Socket");

    /* clear buffer and write response */
    /* message from server to buffer */
    bzero(buf, MAX);

    n = read(sockfd, buf, MAX);
    check(n >= 0, "Error while reading from Socket");

    printf("SERVER: %s\n", buf);

    if (strncmp("quit", buf, 4) == 0)
      break;
  }

  log_info("Connection terminated");
  close(sockfd);

  return (0);

error:
  debug("Error Detected in Client Program!");
  return (-1);
}