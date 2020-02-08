#include <arpa/inet.h>
#include <client.h>
#include <dbg.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

typedef struct _frame
{
  uint16_t index;
  char* message;
  struct _frame* next;
} frame_t;

frame_t *head = NULL, *tail = NULL;

char*
strdup(const char* str)
{
  int length = strlen(str);
  char* copy = (char*)malloc(length + 1);
  memcpy(copy, str, length);
  copy[length] = '\0';
  return copy;
}

frame_t*
Frame_add(const char* message)
{
  static uint32_t index = 0;
  frame_t* frame = (frame_t*)malloc(sizeof(frame_t));

  frame->index = index++;
  frame->message = strdup(message);
  frame->next = NULL;

  if (tail != NULL)
    tail->next = frame;

  tail = frame;

  if (head == NULL)
    head = frame;

  return frame;
}

void
Frame_delete(frame_t* frame)
{
  if (frame == NULL)
    return;

  if (head == frame)
    head = frame->next;

  free(frame->message);
  free(frame);
}

int
Frame_serialize(char* buffer, frame_t* frame)
{
  if (frame == NULL)
    return 0;

  uint16_t length = strlen(frame->message);
  *(uint16_t*)&buffer[2] = length;

  memcpy(buffer, (const void*)&frame->index, sizeof(frame->index));
  memcpy(&buffer[4], frame->message, length);

  return length + sizeof(frame->index) + 2; //+ 2;
}

int server;

frame_t* current_frame;
uint32_t timeout_in_Secs;

volatile uint16_t last_frame_received = 0, last_frame_sent = 0;
volatile int wait_for_reply = 0;

void
timeout(int mask)
{
  wait_for_reply = 0;
  log_info("Timeout");
}

void
data_Received(int mask)
{

  struct sockaddr_in client;
  unsigned int clientSize = sizeof(struct sockaddr);
  int receivedLength;
  char buffer[1024];
  frame_t* local_frame;
  do {
    if ((receivedLength = recvfrom(server,
                                   buffer,
                                   sizeof(buffer),
                                   0,
                                   (struct sockaddr*)&client,
                                   &clientSize)) < 0) {

      if ((errno == EWOULDBLOCK) || (errno = EAGAIN))
        break;

      log_err("Failed to Receive message from the server");
    }

    local_frame = current_frame;
    uint16_t Request_Number = *(uint16_t*)buffer;

    debug("Last Frame Ackd : %d", Request_Number - 1);

    if (Request_Number == (last_frame_received + 1)) {
      current_frame = current_frame->next;

      Frame_delete(local_frame);
      last_frame_received++;

      if (current_frame == NULL) {
        head = NULL;
        tail = NULL;
      }
    }

  } while (receivedLength != 0);

  alarm(timeout_in_Secs);

  if (last_frame_received == (last_frame_sent + 1)) {
    alarm(0);
    wait_for_reply = 0;
  }
}

void
go_back_n_sliding(struct sockaddr_in* client, char* buffer, int window_size)
{

  int i;
  sigset_t set, oset;

  frame_t* local_frame = current_frame;

  sigemptyset(&set);
  sigaddset(&set, SIGIO);
  sigprocmask(SIG_BLOCK, &set, &oset);

  for (i = 0; (i < window_size) && (local_frame != NULL); i++) {
    int length = Frame_serialize(buffer, local_frame);

    if (length != 0) {
      if (sendto(server,
                 buffer,
                 length,
                 0,
                 (struct sockaddr*)client,
                 sizeof(struct sockaddr)) != length)
        log_err("Failed to send message to the server");

      last_frame_sent = local_frame->index;
      local_frame = local_frame->next;
      debug("Last Frame sent : %d", last_frame_sent);
    }
  }

  wait_for_reply = 1;

  alarm(timeout_in_Secs);
  sigprocmask(SIG_UNBLOCK, &set, &oset);
}

void
terminate_program(int mask)
{
  while (head != NULL)
    Frame_delete(head);

  close(server);
  debug("Program Exited");

  exit(0);
}

int probability;
int socket_ID;

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

int
user_input()
{
  printf("Incoming Frame, Accept (Y/else No) : \n");

  int c;
  int ch = fgetc(stdin);

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
  if ((ip = inet_addr(argv[1])) == (uint32_t)-1) {
    if ((hostIP = gethostbyname(argv[1])) != NULL) {
      ip = *(uint32_t*)(hostIP->h_addr_list[0]);
    } else {
      log_err("Unable to resolve the IP address");
    }
  }

  struct in_addr addr = { .s_addr = ip };
  debug("IP Address resolved : %s", inet_ntoa(addr));
  debug("Port : %d", port);

  uint16_t N = atoi(argv[3]);
  timeout_in_Secs = atoi(argv[4]);

  struct sockaddr_in receiver;

  if ((server = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    log_err("Failed to open socket");
  }

  memset(&receiver, 0, sizeof(struct sockaddr));

  receiver.sin_family = AF_INET;
  receiver.sin_addr.s_addr = ip;
  receiver.sin_port = htons(port);

  struct sigaction handler;
  handler.sa_handler = &terminate_program;

  if (sigfillset(&handler.sa_mask) < 0)
    log_err("Failed to set the signal mask");

  handler.sa_flags = 0;

  if (sigaction(SIGINT, &handler, 0) < 0)
    log_err("Failed to set the Signal Handler function of SIGINT");

  handler.sa_handler = &data_Received;

  if (sigfillset(&handler.sa_mask) < 0)
    log_err("Failed to set the signal mask");

  handler.sa_flags = SA_RESTART;

  if (sigaction(SIGIO, &handler, 0) < 0) {
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

  if (sigaction(SIGALRM, &handler, 0) < 0) {
    log_err("Failed to set the Signal Handler function of SIGALRM");
  }
  alarm(0);

  unsigned int clientSize;
  int receivedLength;
  struct sockaddr_in server;
  struct sockaddr_in client;

  if ((socket_ID = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
    log_err("Failed to open socket");
  }

  handler.sa_handler = &terminate_program;
  if (sigfillset(&handler.sa_mask) < 0) {
    log_err("Failed to set the signal mask");
  }
  handler.sa_flags = 0;

  if (sigaction(SIGINT, &handler, 0) < 0) {
    log_err("Failed to set the Signal Handler function of SIGINT");
  }

  srand(time(0));

  memset(&server, 0, sizeof(struct sockaddr_in));
  memset(&client, 0, sizeof(struct sockaddr_in));

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(atoi(argv[6]));
  probability = atoi(argv[7]);

  if (bind(socket_ID, (struct sockaddr*)&server, sizeof(struct sockaddr)) < 0)
    log_err("Failed to bind the socket");

  char buffer[1024];
  uint16_t expecting_frame = 0;
  uint16_t last_frame_received;

  while (1) {

    clientSize = sizeof(struct sockaddr);

    if ((receivedLength = recvfrom(socket_ID,
                                   buffer,
                                   (sizeof(buffer) / sizeof(buffer[0])) - 1,
                                   0,
                                   (struct sockaddr*)&client,
                                   &clientSize)) <= 0)
      log_err("Failed to Receive message from the server");

    last_frame_received = *(uint16_t*)buffer;
    uint16_t message_length = *(uint16_t*)&buffer[2];

    buffer[4 + message_length] = '\0';

    if (expecting_frame == last_frame_received) {

      /*user input*/
      if (probability_Calculate()) {
        expecting_frame++;
        printf("[Proper Frame  ][%03d]  %s\n", last_frame_received, &buffer[4]);
        Frame_add(&buffer[4]);
      } else
        printf("[Corrupted     ][%03d]  %s\n", last_frame_received, &buffer[4]);

    } else {

      if (expecting_frame > last_frame_received) {
        printf("[Retransmission][%03d]  %s\n", last_frame_received, &buffer[4]);
      } else {
        printf("[Out of Order  ][%03d]  %s\n", last_frame_received, &buffer[4]);
      }
    }

    *(uint16_t*)buffer = expecting_frame;

    if ((sendto(socket_ID,
                buffer,
                sizeof(expecting_frame),
                0,
                (struct sockaddr*)&client,
                sizeof(struct sockaddr))) != sizeof(expecting_frame)) {
      log_err("Failed to send message to the server");
    }

    if (head != NULL)
      current_frame = head;

    if ((current_frame != NULL) && !wait_for_reply)
      go_back_n_sliding(&receiver, buffer, N);
  }

error : {
  terminate_program(0);
  while (1)
    ;
}
}
