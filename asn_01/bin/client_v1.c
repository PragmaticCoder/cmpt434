#undef NDEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

int client_connect(char *host, char *port) {

  int network_socket = socket(AF_INET, SOCK_STREAM, 0);

  return network_socket;
}

int main(int argc, char const *argv[]) {

  int socket = 0;
  // socket = client_connect();

  printf("Reached main()\n");
  return 0;
}
