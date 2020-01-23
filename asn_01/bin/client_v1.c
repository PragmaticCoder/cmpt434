#undef NDEBUG

#include <dbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int connect_client(char *host, char *port) {
  int status = 0;
  struct addrinfo *addr = NULL;

  status = getaddrinfo(host, port, NULL, &addr);
  check(status == 0, "Failed to lookup %s:%s", host, port);

  int conn = socket(AF_INET, SOCK_STREAM, 0);
  check(conn > 0, "Socket Creation Failed");

  return conn;

error:
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
