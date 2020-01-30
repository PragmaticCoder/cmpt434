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

  /* Listening to incoming connections */
  listen(sockfd, MAX_CONNECTION);
  log_info("Connection Successful!");
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

    status = recv(cli_sockfd, buf, sizeof(buf), 0);
    check(status >= 0, "Error while recv() from socket.");
    check(buf != NULL, "Cannot read from empty buffer");

    debug("CLIENT: %s", buf);

    /**
     * TODO:
     * 3. print the response message using debug statement
     * 4. store the char* type response to stream
     */

    // char* user_input = malloc(sizeof(buf)* strlen(buf));

    // while (fgets((char*)user_input, strlen(buf), (FILE*)buf) != NULL)
    // puts(user_input);

    // debug("user_input: %s", user_input);

    // char* response_msg = func_command_handler((char*)user_input);
    char* user_input = "put name Alvi";
    char* response_msg = func_command_handler((char*)user_input);
    // char* response_msg = func_command_handler((char*)buf);
    debug("response_msg: %s", response_msg);

    bzero(buf, MAX);                /* clearing buffer */
    fgets(buf, sizeof(buf), stdin); /* reading from stdin */

    // status = send(cli_sockfd, sizeof(char), MAX, 0);
    // check(status >= 0, "Error while Send() to Socket");

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