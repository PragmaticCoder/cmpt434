#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/types.h>

#include <arpa/inet.h>

#include <errno.h>
#include <poll.h>
#include <time.h>
#include <unistd.h>

#include <dbg.h>
#include <router.h>

#define MAX_CLIENT 32UL
#define OPENED 1
#define CLOSED 0

struct connections
{
  int fd;
  uint16_t flag;
  uint16_t port;
};

/* List of clients */
struct pollfd listFD[MAX_CLIENT];

/* Keeping track of open and closed connections */
struct connections links[MAX_CLIENT];

int
close_connections(int fd);

/*
 *   Function Called at the end of the program execution to clear any resources
 * allocated while execution no input parameter , no return arguments
 */
void
closeFD()
{
  log_info("Program Closing, Clearing Resources Allocation . . .\n");

  for (int i = 0; i < MAX_CLIENT; i++) {
    if (listFD[i].fd == -1) {
      continue;
    }
    close(listFD[i].fd); // close any opened client sockets
  }
  remove_table(); // remove route table
}

int
main(int argc, char* argv[])
{

  // check atleast Server name and Server Port is given in the command line
  // argument
  check((argc >= 3),
        "Too few argument\nUsage : ./server <name> <port> <dest-port> "
        "[[opt-port]]\n");

  // store the list of port number for the server given in the command line
  // argument and to keep track which one is opened or not
  for (int i = 3; i < argc; i++) {
    links[i - 3].port = atoi(argv[i]);
    links[i - 3].flag = CLOSED;
  }

  int serverFD; // server file descriptor
  struct sockaddr_in server;
  struct sockaddr_in client;

  struct timespec last_time;

  unsigned char buffer[1024];

  unsigned int clientStructSize = sizeof(struct sockaddr_in);
  int clientFD;

  check(((serverFD = socket(PF_INET, SOCK_STREAM, 0)) >= 0),
        "Failed to Allocate the socket file descriptor for the server\n");

  atexit(closeFD); // register the callback function for the exit event
  memset(&server, 0, sizeof(struct sockaddr_in)); // clear the structure

  listFD[0].fd = serverFD; // list of file descriptor to poll for IO activity
                           // first is server port itself
  listFD[0].events = POLLIN;
  for (int i = 1; i < MAX_CLIENT; i++) {
    listFD[i].fd = -1; // clear the rest of the file descriptor
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[2])); // fill server socket structure

  check(
    (bind(serverFD, (struct sockaddr*)&server, sizeof(struct sockaddr)) >= 0),
    "Failed to bind to the given Port\n");

  check((listen(serverFD, MAX_CLIENT) >= 0),
        "Failed to put the Server in Listen Mode\n");

  add_router_entry(
    add_gateway(atoi(argv[2]), argv[1])); // add the first entry of the router
                                          // table with its own port address

  int clientNum = 1;
  int rc;

  clock_gettime(CLOCK_MONOTONIC, &last_time);

  while (1) {

    for (int i = 0; i < argc - 3; i++) {
      if (links[i].flag == CLOSED) {

        check(
          ((links[i].fd = socket(PF_INET, SOCK_STREAM, 0)) >= 0),
          "Failed to Allocate the socket file descriptor for the Clients\n");

        memset(&client, 0, sizeof(struct sockaddr_in));

        client.sin_family = AF_INET;

        client.sin_addr.s_addr = inet_addr("127.0.0.1");
        client.sin_port = htons(links[i].port); // client address and port

        debug("Connecting Server %d ...", links[i].port);

        if (connect(links[i].fd,
                    (struct sockaddr*)&client,
                    sizeof(struct sockaddr_in)) < 0) {

          debug("Failed\n");
          continue;
        }

        printf("Connected\n");
        links[i].flag = OPENED; // mark this descriptor as opened

        if (clientNum < MAX_CLIENT) { // add the descritor for polling

          listFD[clientNum].fd = links[i].fd;
          listFD[clientNum].events = POLLIN;
          clientNum++;
        } else {
          close(links[i].fd); // if the array overflows close the file
                              // descriptor (connection)
        }
      }
    }

    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time); // get the current time

    if ((current_time.tv_sec - last_time.tv_sec) >=
        2) { // check if elapsed time is greater then 2 secs
      last_time.tv_sec = current_time.tv_sec; // update the previous time value
      print_Table();                          // print the router table
      for (int i = 1; i < MAX_CLIENT; i++) {
        if (listFD[i].fd == -1) {
          continue;
        }

        int length = form_packet(
          buffer); // serialise the router table for sending over TCP link
        if (send(listFD[i].fd, buffer, length, 0) != length) {
          debug("Failed to send to the Server\n");
        }
      }
    }

    check(((rc = poll(listFD, MAX_CLIENT, 2000)) >= 0),
          "Error in Polling the file descriptor\n");
    if (rc == 0) {
      debug("Timeout");
    } else {
      if ((listFD[0].revents & POLLIN) ==
          POLLIN) { // check if there is any incoming request in Server port
        clientStructSize = sizeof(struct sockaddr_in);
        check(((clientFD = accept(listFD[0].fd,
                                  (struct sockaddr*)&client,
                                  &clientStructSize)) >= 0),
              "Failed to connect to the Client\n");

        debug("Client Connected from the IP Address : %s  Port : %d\n\r",
              inet_ntoa(client.sin_addr),
              ntohs(client.sin_port));

        if (clientNum < MAX_CLIENT) { // add the descriptor for polling
          listFD[clientNum].fd = clientFD;
          listFD[clientNum].events = POLLIN;
          clientNum++;
        } else {

          close(clientFD); // if the array overflows close the file descriptor
                           // (connection)
        }
      }

      for (int i = 1; i < MAX_CLIENT; i++) { // loop rest of the descriptor list
        // to check if any have IO ready
        if (listFD[i].fd == -1) {
          continue; // skip the unimplemented sockets
        }

        int readLength;

        if ((listFD[i].revents & POLLHUP) ==
            POLLHUP) { // check if the Connection Hangup
          listFD[i].fd = close_connections(listFD[i].fd);
          debug("Remote connection Closed\n");
        }

        if ((listFD[i].revents & POLLERR) ==
            POLLERR) { // check if the connection have error in Connection
          listFD[i].fd = close_connections(listFD[i].fd);
          debug("Pipe connection Closed\n");
        } else if ((listFD[i].revents & POLLIN) ==
                   POLLIN) { // read the incoming data
          check(((readLength =
                    recv(listFD[i].fd, buffer, sizeof(buffer) - 1, 0)) >= 0),
                "Failed to read incoming data\n");

          if (readLength == 0) { // TCP connection closed
            listFD[i].fd = close_connections(listFD[i].fd);
          }

          else {
            // printf("Message Received : ");
            // for(int i = 0; i < readLength; i += 2) {
            //    printf("0x%X%X ", buffer[i], buffer[i + 1]);
            //}
            // printf("\n");
            parse_data(
              buffer,
              readLength,
              listFD[i].fd); // parse the received data and update the table
          }
        }
      }
    }
  }
error : {
  exit(EXIT_FAILURE);
}
}

/*
 *   Function used to close the file descriptor and mark the port as CLOSED if
 * it is a command line argument and updates the table input parament - file
 * descriptor to be closed, return -1
 */
int
close_connections(int fd)
{
  // clientStructSize = sizeof(struct sockaddr_in);
  // clientFD = getpeername(listFD[0].fd, (struct sockaddr *)&client,
  // &clientStructSize); printf("Connection Closed : IP %s\n",
  // inet_ntoa(client.sin_addr));
  close(fd); // close the file
  for (int j = 0; j < MAX_CLIENT;
       j++) { // find the port  in the list and mark it as closed
    if (links[j].fd == fd) {
      links[j].flag = CLOSED;
      break;
    }
  }
  route_disconnected(fd); // update the router table
  debug("Connection Closed\n");
  return -1;
}
