/**
 * Name: Alvi Akbar
 * NSID: ala273
 * 11118887
 */

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
#include <unistd.h>

int server;
frame_t* current_frame;
uint16_t N;

struct sockaddr_in receiver;
uint32_t timeout_in_seconds;

volatile uint16_t last_frame_received = 0;
volatile uint16_t last_frame_sent = 0;

/*
 * Signal Handler for SIGIO to read when IO device ready for read/write function
 */

void
data_Received(int mask)
{

  frame_t* local_frame;
  struct sockaddr_in client;
  unsigned int client_size = sizeof(struct sockaddr);

  int receivedLength;
  char buffer[1024];

  /* loop till all the received UDP packets are copied in storage buffer */
  do {
    receivedLength = recvfrom(server,
                              buffer,
                              sizeof(buffer),
                              0,
                              (struct sockaddr*)&client,
                              &client_size);

    /* when there is no bytes in the internal buffer */
    if ((receivedLength < 0) && ((errno == EWOULDBLOCK) || (errno = EAGAIN)))
      break;

    if (receivedLength < 0)
      log_err("Failed to Receive message from the server");

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
        Set_head(NULL);
        Set_tail(NULL);
      }
    }
  }

  while (receivedLength != 0);
  alarm(timeout_in_seconds); // restart the timeout alarm

  if (last_frame_received ==
      (last_frame_sent +
       1)) { // if the last received and last sent frame are in sync

    alarm(0); // reset the timer
    raise(SIGALRM);
  }
}

/*
 * function to send the data to UDP server
 */

void
go_back_n_sliding()
{

  int i;
  frame_t* local_frame = current_frame;
  char buffer[1024];
  sigset_t set, oset;

  sigemptyset(&set);
  sigaddset(&set, SIGIO); // mask for SIGIO to disable the interrupt while in
                          // the middle of sending the frame window
  sigprocmask(SIG_BLOCK, &set, &oset);

  for (i = 0; (i < N) && (local_frame != NULL);
       i++) { // loop till the window size or last frame
    int length = Frame_serialize(
      buffer,
      local_frame); // convert the frame to linear buffer for sending via UDP

    if (length != 0) {
      if (sendto(server,
                 buffer,
                 length,
                 0,
                 (struct sockaddr*)&receiver,
                 sizeof(struct sockaddr)) != length) {
        log_err("Failed to send message to the server");
      }
      last_frame_sent = local_frame->index;
      local_frame = local_frame->next;
      debug("Last Frame sent : %d", last_frame_sent);
    }
  }
  alarm(timeout_in_seconds);             // start the timeout alarm
  sigprocmask(SIG_UNBLOCK, &set, &oset); // unmask the SIGIO interrupt
}

/*
 * Signal Handler for the timeout alarm
 */
void
timeout(int mask)
{
  go_back_n_sliding(); // unblocks start the write function
  debug("Timeout");
}

/*
 *	Signal Handler to terminate the program
 */
void
terminate_program(int mask)
{
  Frame_delete_all(); // delete all the frame in the queue
  close(server);      // close the socket
  debug("Program Exited");
  exit(0); // exit the program
}

int
main(int argc, char* argv[])
{

  check(
    (argc >= 5),
    "Usage ./client <Hostname/IP Address> <Port> <Window Size> <Timeout(S)>");

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

  N = atoi(argv[3]);                  // Window Size
  timeout_in_seconds = atoi(argv[4]); // timeout

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

  handler.sa_handler = &data_Received; // Signal Handler for SIGIO signal used
                                       // for nonBlocking UDP socket
  if (sigfillset(&handler.sa_mask) < 0)
    log_err("Failed to set the signal mask");

  handler.sa_flags = SA_RESTART;

  if (sigaction(SIGIO, &handler, 0) < 0)
    log_err("Failed to set the Signal Handler function of SIGIO");

  if (fcntl(server, F_SETOWN, getpid()) < 0)
    log_err("Failed to set the Owner of the Socket");

  if (fcntl(server, F_SETFL, O_NONBLOCK | FASYNC) < 0)
    log_err("Failed to set the Socket in Async Mode");

  handler.sa_handler =
    &timeout; // signal handler for SIGALRM signal used for timeout purpose
  if (sigfillset(&handler.sa_mask) < 0)
    log_err("Failed to set the signal mask");

  handler.sa_flags = SA_RESTART;

  if (sigaction(SIGALRM, &handler, 0) < 0)
    log_err("Failed to set the Signal Handler function of SIGALRM");

  alarm(timeout_in_seconds);

  char ch;
  char buffer[1024];
  int num = 0;
  int size;
  int inputFD = fileno(stdin);

  sprintf(buffer, "[000] ");
  if (write(fileno(stdout), buffer, strlen(buffer)) != strlen(buffer))
    log_err("Failed to write to the output stream");

  while (1) {

    size = read(inputFD, &ch, 1); // read single character from the STDIN buffer
    if (size > 0) {
      if (ch != '\n') { // copy the value into the buffer till newline character
                        // is pressed
        buffer[num++] = ch;
      } else {
        buffer[num] = '\0';                 // null terminate the buffer
        num = 0;                            // reset the index
        frame_t* frame = Frame_add(buffer); // add to the buffer
        sprintf(buffer, "[%03d] ", frame->index + 1); // sequence string

        if (write(fileno(stdout), buffer, strlen(buffer)) != strlen(buffer))
          log_err("Failed to write to the output stream");

        if (current_frame == NULL)
          current_frame = Get_head(); // get the lastest additon in the queue
      }
    } else if ((size < 0) && (errno != EWOULDBLOCK) && (errno != EAGAIN))
      log_err("Failed to read from the user");
  }

  frame_t *head = Get_head(), *tail = Get_tail();
  if ((head != NULL) && (tail != NULL) && ((tail->index - head->index) < N))
    raise(SIGALRM);

/* Error condition exit the program */
error : {
  terminate_program(0);
  while (1)
    ;
}
}
