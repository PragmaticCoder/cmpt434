#include <arpa/inet.h>
#include <dbg.h>
#include <fcntl.h>
#include <frame.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int probability;
int socket_ID;

/*
 * Function corruption/loss probability if n probability for every 10 frames,
 * if n = 1, every 10 messages 1 will be corrupted, acknowledgement will not be
 * sent
 */
int
probability_Calculate()
{
  static int loss = 0;
  static int total_count = 0;

  if (total_count++ >= 10) {
    loss = 0;
    total_count = 0;
  }

  if (rand() > 28878) {
    if (loss < probability) {
      loss++;
      return 0;
    }
  }
  return 1;
}

/*
 * Function Asks User whether to Accept the Frame or not
 */
int
user_input()
{
  printf("Incoming Frame, Accept (Y/else No) : \n");
  int ch = fgetc(stdin);
  int c;
  while (((c = fgetc(stdin)) != '\n') && (c != EOF))
    ;
  if ((ch == 'Y') || (ch == 'y'))
    return 1;
  return 0;
}

/*
 * Signal handler for the closing the program safely
 */

void
terminate_program(int mask)
{
  close(socket_ID);
  debug("Program Exited");
  exit(0);
}

int
main(int argc, char* argv[])
{

  check((argc >= 3), "Usage ./server <Port> <Probability(%%)>");

  unsigned int clientSize;
  int receivedLength;
  struct sockaddr_in server;
  struct sockaddr_in client;

  if ((socket_ID = socket(PF_INET, SOCK_DGRAM, 0)) <
      0) { // open the socket for the server
    log_err("Failed to open socket");
  }

  struct sigaction handler;

  handler.sa_handler =
    &terminate_program; // Signal handler for SIGINT signal used to clear memory
                        // and close sockets
  if (sigfillset(&handler.sa_mask) < 0) {
    log_err("Failed to set the signal mask");
  }
  handler.sa_flags = 0;

  if (sigaction(SIGINT, &handler, 0) < 0) {
    log_err("Failed to set the Signal Handler function of SIGINT");
  }

  srand(time(NULL)); // seed the rand function
  rand();

  memset(&server, 0, sizeof(struct sockaddr_in));
  memset(&client, 0, sizeof(struct sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[1]));
  probability = atoi(argv[2]); // server IP details

  if (bind(socket_ID, (struct sockaddr*)&server, sizeof(struct sockaddr)) <
      0) { // bind the server to the socket
    log_err("Failed to bind the socket");
  }

  char buffer[1024];
  uint16_t expecting_frame = 0;
  uint16_t last_frame_received;
  while (1) {

    clientSize = sizeof(struct sockaddr);
    // since only one frame is received at any time it uses blocking function to
    // receive from the client
    if ((receivedLength = recvfrom(socket_ID,
                                   buffer,
                                   (sizeof(buffer) / sizeof(buffer[0])) - 1,
                                   0,
                                   (struct sockaddr*)&client,
                                   &clientSize)) <= 0) {
      log_err("Failed to Receive message from the server");
    }

    last_frame_received = *(uint16_t*)buffer;
    uint16_t message_length = *(uint16_t*)&buffer[2];
    uint16_t Acutual_crc = *(uint16_t*)&buffer[4 + message_length];
    uint16_t calculated_crc =
      CRC16(buffer,
            2 + sizeof(expecting_frame) +
              message_length); // check for corruption in the communication

    buffer[4 + message_length] = '\0';

    // check the frame Id from the received message
    if (expecting_frame == last_frame_received) {
      if ((Acutual_crc == calculated_crc) &&
          probability_Calculate() /*user input*/) {
        expecting_frame++;
        printf("[Proper Frame  ][%03d]  %s\n", last_frame_received, &buffer[4]);
      } else {
        printf("[Corrupted     ][%03d]  %s\n", last_frame_received, &buffer[4]);
      }
    } else {
      if (expecting_frame > last_frame_received) {
        printf("[Retransmission][%03d]  %s\n", last_frame_received, &buffer[4]);
      } else {
        printf("[Out of Order  ][%03d]  %s\n", last_frame_received, &buffer[4]);
      }
    }
    //	send the acknowledgement to the client with next expecting frame Number
    *(uint16_t*)buffer = expecting_frame;

    if ((sendto(socket_ID,
                buffer,
                sizeof(expecting_frame),
                0,
                (struct sockaddr*)&client,
                sizeof(struct sockaddr))) != sizeof(expecting_frame)) {
      log_err("Failed to send message to the server");
    }
  }
/* Error condition exit the program */
error : {
  terminate_program(0);
  while (1)
    ;
}
}
