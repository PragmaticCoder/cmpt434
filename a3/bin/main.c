/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */

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

struct pollfd listFD[MAX_CLIENT];
struct connections links[MAX_CLIENT];

/* Function Prototypes */

void
closeFD();

int
close_connections(int fd);

/**
 * @brief Called at the end of the program execution to clear any resources
 * allocated while execution
 * @note
 * @retval
 */
void
closeFD()
{
  log_info("Program Closing, Clearing Resources Allocation . . .\n");

  for (int i = 0; i < MAX_CLIENT; i++) {
    if (listFD[i].fd == -1)
      continue;

    close(listFD[i].fd); /* close any opened client sockets */
  }

  remove_table(); /* remove route table */
}

/**
 * @brief Closes the file descriptor and mark the port as CLOSED if
 * it is a command line argument and updates the table input parament
 * @note
 * @param  fd: file descriptor to be closed
 * @retval -1
 */

int
close_connections(int fd)
{
  close(fd);

  /* find the port  in the list and mark it as closed */
  for (int j = 0; j < MAX_CLIENT; j++) {
    if (links[j].fd == fd) {
      links[j].flag = CLOSED;
      break;
    }
  }

  route_disconnected(fd); /* update the router table */
  debug("Connection Closed");

  return -1;
}

int
main(int argc, char* argv[])
{

  /* check atleast Server name and Server Port is given in the command line */
  /* argument */
  check((argc >= 3),
        "Too few argument\nUsage : ./server <name> <port> <dest-port> "
        "[[opt-port]]");

  /* store the list of port number for the server given in the command line */
  /* argument and to keep track of port state */
  for (int i = 3; i < argc; i++) {
    links[i - 3].port = atoi(argv[i]);
    links[i - 3].flag = CLOSED;
  }

  int clientFD;
  int serverFD;

  struct sockaddr_in client;
  struct sockaddr_in server;

  unsigned char buffer[1024];
  struct timespec last_time;

  unsigned int clientStructSize = sizeof(struct sockaddr_in);

  check(((serverFD = socket(PF_INET, SOCK_STREAM, 0)) >= 0),
        "Failed to Allocate the socket file descriptor for the server\n");

  atexit(closeFD); /* register the callback function for the EXIT event */
  memset(&server, 0, sizeof(struct sockaddr_in)); /* clear the structure */

  /* list of file descriptor to poll for I/O activity */
  /* where the first item is the server port itself */
  listFD[0].fd = serverFD;
  listFD[0].events = POLLIN;

  /* clearing rest of the file descriptor */
  for (int i = 1; i < MAX_CLIENT; i++)
    listFD[i].fd = -1;

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[2]));

  check(
    (bind(serverFD, (struct sockaddr*)&server, sizeof(struct sockaddr)) >= 0),
    "Failed to bind to the given Port\n");

  check((listen(serverFD, MAX_CLIENT) >= 0),
        "Failed to put the Server in Listen Mode\n");

  /* add the first entry of the router */
  /* table with its own port address */
  add_router_entry(add_gateway(atoi(argv[2]), argv[1]));

  int rc = 0;
  int clientNum = 1;

  clock_gettime(CLOCK_MONOTONIC, &last_time);

  while (1) {

    for (int i = 0; i < argc - 3; i++) {

      if (links[i].flag == CLOSED) {

        links[i].fd = socket(PF_INET, SOCK_STREAM, 0);

        check(links[i].fd >= 0,
              "Failed to allocate the socket file descriptor");

        memset(&client, 0, sizeof(struct sockaddr_in));

        client.sin_family = AF_INET;
        client.sin_addr.s_addr = inet_addr("127.0.0.1");
        client.sin_port = htons(links[i].port);

        debug("Connecting Server %d ...", links[i].port);

        int status = connect(
          links[i].fd, (struct sockaddr*)&client, sizeof(struct sockaddr_in));

        if (status < 0) {
          debug("Failed to connect");
          continue;
        }

        log_info("Connected");

        /* mark this descriptor as opened */
        links[i].flag = OPENED;

        /* add the descriptor for polling */
        if (clientNum < MAX_CLIENT) {
          listFD[clientNum].fd = links[i].fd;
          listFD[clientNum].events = POLLIN;
          clientNum++;

        } else /* overflow closes the file descriptor connection*/
          close(links[i].fd);
      }
    }

    struct timespec current_time;
    clock_gettime(CLOCK_MONOTONIC, &current_time); /* get the current time */

    /* check if elapsed time is greater then 2 secs */
    /* if yes, update the previous time value */

    if ((current_time.tv_sec - last_time.tv_sec) >= 2) {

      last_time.tv_sec = current_time.tv_sec;
      print_Table();

      for (int i = 1; i < MAX_CLIENT; i++) {

        if (listFD[i].fd == -1)
          continue;

        /* serialize the router table for sending over TCP link */
        int length = form_packet(buffer);

        if (send(listFD[i].fd, buffer, length, 0) != length) {
          debug("Failed to send to the Server");
        }
      }
    }

    check(((rc = poll(listFD, MAX_CLIENT, 2000)) >= 0),
          "Error in Polling the file descriptor\n");

    if (rc == 0)
      debug("Connection: Timeout");
    else {
      if ((listFD[0].revents & POLLIN) == POLLIN) {

        /* check if there is any incoming request in server port */
        clientStructSize = sizeof(struct sockaddr_in);
        check(((clientFD = accept(listFD[0].fd,
                                  (struct sockaddr*)&client,
                                  &clientStructSize)) >= 0),
              "Failed to connect to the Client\n");

        debug("Client Connected from the IP Address : %s  Port : %d\n\r",
              inet_ntoa(client.sin_addr),
              ntohs(client.sin_port));

        if (clientNum < MAX_CLIENT) { /* add the descriptor for polling */

          listFD[clientNum].fd = clientFD;
          listFD[clientNum].events = POLLIN;
          clientNum++;

        } else /* if the array overflows close the file descriptor */
          close(clientFD);
      }

      /* loop rest of the descriptor list */
      /* to check if any have IO ready */
      for (int i = 1; i < MAX_CLIENT; i++) {

        if (listFD[i].fd == -1)
          continue; /* skip the unimplemented sockets */

        int readLength;
        size_t len;

        /* check if the Connection hang up */
        if ((listFD[i].revents & POLLHUP) == POLLHUP) {
          listFD[i].fd = close_connections(listFD[i].fd);
          debug("Remote connection Closed\n");
        }

        /* check if the connection have error in Connection */
        if ((listFD[i].revents & POLLERR) == POLLERR) {

          listFD[i].fd = close_connections(listFD[i].fd);
          debug("Pipe connection Closed\n");

        } else if ((listFD[i].revents & POLLIN) == POLLIN) {
          /* read the incoming data */
          check(((readLength =
                    recv(listFD[i].fd, buffer, sizeof(buffer) - 1, 0)) >= 0),
                "Failed to read incoming data\n");

          if (readLength == 0) /* TCP connection closed */
            listFD[i].fd = close_connections(listFD[i].fd);
          else /* parse the received data and update the table */
            parse_data(buffer, readLength, listFD[i].fd);
        }
      }
    }
  }
error : {
  exit(EXIT_FAILURE);
}
}
