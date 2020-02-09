#include <arpa/inet.h>
#include <dbg.h>
#include <errno.h>
#include <fcntl.h>
#include <frame.h>
#include <netdb.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

frame_t* current_frame;
int server;
uint32_t timeout_in_Secs;
volatile uint16_t last_frame_received = 0, last_frame_sent = 0;
volatile int wait_for_reply = 0;
int probability;
int socket_ID;

/*
 * Signal Handler for the timeout alarm
 */
void
timeout(int mask)
{
  wait_for_reply = 0; // unblocks start the write function
  debug("Timeout");
}

/*
 * Signal Handler for SIGIO to read when IO device ready for read/write function
 */
void
data_Received(int mask)
{

  struct sockaddr_in client;
  unsigned int clientSize = sizeof(struct sockaddr);
  int receivedLength;
  char buffer[1024];
  frame_t* local_frame;
  // loop till all the received UDP packets are copied in storage buffer
  do {
    if ((receivedLength = recvfrom(server,
                                   buffer,
                                   sizeof(buffer),
                                   0,
                                   (struct sockaddr*)&client,
                                   &clientSize)) < 0) {
      if ((errno == EWOULDBLOCK) ||
          (errno =
             EAGAIN)) { // indicates there is no bytes in the internal buffer
        break;
      }
      log_err("Failed to Receive message from the server");
    }

    local_frame = current_frame;
    uint16_t Request_Number =
      *(uint16_t*)
        buffer; // get the request number of the last received index number

    debug("Last Frame Ackd : %d",
          Request_Number -
            1); // received index always point to next frame in the queue
    if (Request_Number ==
        (last_frame_received +
         1)) { // compare the received index number with last sent index number
      current_frame = current_frame->next; // point to next frame in the queue
      Frame_delete(local_frame); // free the memory allocated to this frame
      last_frame_received++;     // increment the last acknowledged frame index
      if (current_frame == NULL) { // if this is the frame in the queue reset
                                   // the head and tail of the queue
        set_Head(NULL);
        set_Tail(NULL);
      }
    }

  } while (receivedLength != 0);
  alarm(timeout_in_Secs); // restart the timeout alarm
  if (last_frame_received ==
      (last_frame_sent +
       1)) {  // if the last received and last sent frame are in sync
    alarm(0); // reset the timer
    wait_for_reply = 0;
  }
}

/*
 * function to send the data to UDP server
 */
void
go_back_n_sliding(struct sockaddr_in* client, char* buffer, int window_size)
{

  int i;
  frame_t* local_frame = current_frame;
  sigset_t set, oset;
  sigemptyset(&set);
  sigaddset(&set, SIGIO); // mask for SIGIO to disable the interrupt while in
                          // the middle of sending the frame window
  sigprocmask(SIG_BLOCK, &set, &oset);
  for (i = 0; (i < window_size) && (local_frame != NULL);
       i++) { // loop till the window size or last frame
    int length = Frame_serialize(
      buffer,
      local_frame); // convert the frame to linear buffer for sending via UDP
    if (length != 0) {
      if (sendto(server,
                 buffer,
                 length,
                 0,
                 (struct sockaddr*)client,
                 sizeof(struct sockaddr)) != length) {
        log_err("Failed to send message to the server");
      }
      last_frame_sent = local_frame->index;
      local_frame = local_frame->next;
      debug("Last Frame sent : %d", last_frame_sent);
    }
  }
  wait_for_reply = 1;
  alarm(timeout_in_Secs);                // start the timeout alarm
  sigprocmask(SIG_UNBLOCK, &set, &oset); // unmask the SIGIO interrupt
}

/*
 *	Signal Handler to terminate the program
 */
void
terminate_program(int mask)
{
  Frame_delete_all(); // delete all the frame in the queue
  close(server);      // close the socket
  close(socket_ID);
  debug("Program Exited");
  exit(0); // exit the program
}

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
  if (rand() > 16384) {
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

int
main(int argc, char* argv[])
{

  check((argc >= 7),
        "Usage ./client <Hostname/IP Address> <DPort> <Window Size> "
        "<Timeout(S)> <RPort> <Probability(%%)>");

  uint32_t ip;
  struct hostent* hostIP;
  uint16_t port = atoi(argv[2]);

  debug("Resolving IP Addres ...");
  if ((ip = inet_addr(argv[1])) == (uint32_t)-1) {   // if IP Address
    if ((hostIP = gethostbyname(argv[1])) != NULL) { // if HostName
      ip = *(uint32_t*)(hostIP->h_addr_list[0]);
    } else {
      log_err("Unable to resolve the IP address");
    }
  }

  struct in_addr addr = { .s_addr = ip };
  debug("IP Address resolved : %s", inet_ntoa(addr));
  debug("Port : %d", port);

  uint16_t N = atoi(argv[3]);      // Window Size
  timeout_in_Secs = atoi(argv[4]); // timeout

  struct sockaddr_in receiver;

  if ((server = socket(PF_INET, SOCK_DGRAM, 0)) < 0) { // open the socket
    log_err("Failed to open socket");
  }

  memset(&receiver, 0, sizeof(struct sockaddr));
  receiver.sin_family = AF_INET;
  receiver.sin_addr.s_addr = ip;
  receiver.sin_port =
    htons(port); // fill the socket structure for the server to connect

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

  handler.sa_handler = &data_Received;
  if (sigfillset(&handler.sa_mask) < 0) {
    log_err("Failed to set the signal mask");
  }
  handler.sa_flags = SA_RESTART;

  if (sigaction(SIGIO, &handler, 0) <
      0) { // Signal Handler for SIGIO signal used for nonBlocking UDP socket
    log_err("Failed to set the Signal Handler function of SIGIO");
  }

  if (fcntl(server, F_SETOWN, getpid()) < 0) {
    log_err("Failed to set the Owner of the Socket");
  }

  if (fcntl(server, F_SETFL, O_NONBLOCK | FASYNC) < 0) {
    log_err("Failed to set the Socket in Async Mode");
  }

  handler.sa_handler = &timeout;
  if (sigfillset(&handler.sa_mask) < 0) {
    log_err("Failed to set the signal mask");
  }
  handler.sa_flags = SA_RESTART;

  if (sigaction(SIGALRM, &handler, 0) <
      0) { // signal handler for SIGALRM signal used for timeout purpose
    log_err("Failed to set the Signal Handler function of SIGALRM");
  }
  alarm(0);

  unsigned int clientSize;
  int receivedLength;
  struct sockaddr_in server;
  struct sockaddr_in client;

  if ((socket_ID = socket(PF_INET, SOCK_DGRAM, 0)) <
      0) { // open the socket for the server
    log_err("Failed to open socket");
  }

  srand(time(0)); // seed the rand function

  memset(&server, 0, sizeof(struct sockaddr_in));
  memset(&client, 0, sizeof(struct sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[5]));
  probability = atoi(argv[6]); // server IP details

  if (bind(socket_ID, (struct sockaddr*)&server, sizeof(struct sockaddr)) <
      0) { // bind the server to the socket
    log_err("Failed to bind the socket");
  }

  if (fcntl(socket_ID, F_SETFL, O_NONBLOCK) <
      0) { // set receiver in non-blocking mode
    log_err("Failed to set the Socket in Async Mode");
  }

  char buffer[1024];
  uint16_t expecting_frame = 0;
  uint16_t last_frame_received = 0;

  while (1) {

    clientSize = sizeof(struct sockaddr);
    // check if any data received from the client
    if ((receivedLength = recvfrom(socket_ID,
                                   buffer,
                                   (sizeof(buffer) / sizeof(buffer[0])) - 1,
                                   0,
                                   (struct sockaddr*)&client,
                                   &clientSize)) <= 0) {
      if ((errno != EWOULDBLOCK) &&
          (errno != EAGAIN)) { // if no data byte found in the buffer
        log_err("Failed to Receive message from the server");
      }
    } else {
      last_frame_received = *(uint16_t*)buffer; // index of the message received
      uint16_t message_length =
        *(uint16_t*)&buffer[2]; // length of the message received
      uint16_t Acutual_crc = *(uint16_t*)&buffer[4 + message_length];
      uint16_t calculated_crc =
        CRC16(buffer,
              2 + sizeof(expecting_frame) +
                message_length); // check for corruption in the communication

      buffer[4 + message_length] = '\0';

      debug("Ac CRC : 0x%04X Cal CRC : 0x%04X", Acutual_crc, calculated_crc);

      if (expecting_frame == last_frame_received) { // check if received message
                                                    // is in proper sequence
        if ((Acutual_crc == calculated_crc) &&
            probability_Calculate() /*user input*/) { // simulate
                                                      // corruption/loss
          expecting_frame++;
          printf(
            "[Proper Frame  ][%03d]  %s\n", last_frame_received, &buffer[4]);
          Frame_add(&buffer[4]);
        } else {
          printf(
            "[Corrupted     ][%03d]  %s\n", last_frame_received, &buffer[4]);
        }
      } else {
        if (expecting_frame > last_frame_received) {
          printf(
            "[Retransmission][%03d]  %s\n", last_frame_received, &buffer[4]);
        } else {
          printf(
            "[Out of Order  ][%03d]  %s\n", last_frame_received, &buffer[4]);
        }
      }
    }
    // if the data have been received from the client , send acknowledgement
    // back to the server
    if (receivedLength > 0) {
      *(uint16_t*)buffer = expecting_frame;
      if ((sendto(socket_ID,
                  buffer,
                  sizeof(expecting_frame),
                  0,
                  (struct sockaddr*)&client,
                  sizeof(struct sockaddr))) != sizeof(expecting_frame)) {
        if ((errno != EWOULDBLOCK) && (errno != EAGAIN)) {
          log_err("Failed to send message to the server");
        }
      }
    }

    if (get_Head() != NULL) {
      current_frame = get_Head(); // get the latest addition in the queue
    }

    if ((current_frame != NULL) &&
        !wait_for_reply) { // if queue is not empty and not waiting for reply ,
                           // send the message
      go_back_n_sliding(&receiver, buffer, N);
    }
  }

/* Error condition exit the program */
error : {
  terminate_program(0);
  while (1)
    ;
}
}
