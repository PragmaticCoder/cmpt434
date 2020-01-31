#undef NDEBUG

#include <dbg.h>
#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <netinet/in.h>

#define MAX 1024
#define MAX_CONNECTION 10

typedef char* (*lib_Command_Handler)(char* user_input);

char* lib_file = "./build/libasn_01.so";
void* lib = NULL;

int
Socket_setup(int port)
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

  log_info(
    "Attempting to connnet to %s:%d", "localhost", ntohs(serv_addr.sin_port));

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
  lib = dlopen(lib_file, RTLD_NOW);
  check(lib != NULL, "Failed to open the library to test.");

  lib_Command_Handler func_command_handler = dlsym(lib, "command_handler");
  check(func_command_handler != NULL,
        "Failed to find command_handler function.");

  check(argc == 2, "USAGE: ./socket_v1 <port>");

  int port = 0;
  port = atoi(argv[1]);

  int sockfd = 0;
  sockfd = Socket_setup(port);
  check(sockfd >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

search_for_clients:
  /* Listening to incoming connections */
  listen(sockfd, MAX_CONNECTION);
  log_info("Waiting for client to connect...");

  int cli_sockfd;
  struct sockaddr_in cli_addr;
  socklen_t clilen;

  cli_sockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &clilen);
  check(sockfd >= 0, "Server Setup using %s:%s Failed", argv[1], argv[2]);

  /* Client-Server Communication */
  char buf[MAX];

  while (1) {
    /* Receive message from client */
    bzero(buf, MAX);

    int status = 0;

    debug("START OF WHILE LOOP");

    status = recv(cli_sockfd, buf, sizeof(buf), 0);
    check(status >= 0, "Error while recv() from socket.");
    check(buf != NULL, "Cannot read from empty buffer");

    debug("CLIENT: %s", buf);

    if (strncmp("quit", buf, 4) == 0) {
      close(cli_sockfd);
      goto search_for_clients;
    }

    char response_msg[MAX];

    strcpy(response_msg, func_command_handler((char*)buf));
    debug("response_msg: %s", response_msg);

    status = send(cli_sockfd, response_msg, sizeof(response_msg), 0);
    check(status >= 0, "Error while Send() to Socket");
  }

  close(sockfd);

  return (0);

error:
  debug("Error Detected in Server Program!");

  return (-1);
}