#undef NDEBUG

#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

int connect_client(const char *host, const char *port) {
  int status = 0;

  /* socket creation */
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd >= 0, "Failed to created socket.");

  /* specifying an destination address for the socket*/
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;

  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  status = connect(sockfd, (struct sockaddr *)&server_addr,
                   sizeof(struct sockaddr_in));
  check(status == -1, "Failed to create connection");

  return sockfd;

error:
  freeaddrinfo(server_addr);
  return -1;
}

int main(int argc, char const *argv[]) {

  check(argc == 3, "USAGE: ./client_v1 <host> <port>");

  log_info("HOST: %s \tPORT: %s", argv[1], argv[2]);

  int conn = 0;
  conn = connect_client(argv[1], argv[2]);
  check(conn >= 0, "Connection to %s:%s Failed", argv[1], argv[2]);

error:
  return -1;

  return 0;
}
