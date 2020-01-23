#undef NDEBUG

#include <dbg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int client_connect(char *host, char *port) {
  debug("Inside client_connection()");

  int conn = socket(AF_INET, SOCK_STREAM, 0);
  check(conn > 0, "Create Connection: Failed");

  return conn;

error:
  return -1;
}

int main(int argc, char const *argv[]) {

  check(argc == 3, "USAGE: netclient <host> <port>");

  int conn = 0;
  conn = client_connect(argv[1], argv[2]);
  check(conn >= 0, "Connection to %s:%s failed", argv[1], argv[2]);

  debug("Reached main()");

error:
  return -1;

  return 0;
}
