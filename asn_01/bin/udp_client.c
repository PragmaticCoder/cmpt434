#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 30000
#define MAXLINE 1024

int
main()
{
  int sockfd;
  char buffer[MAXLINE];
  char* hello = "getv ana";
  struct sockaddr_in servaddr;

  /* Creating socket file descriptor */
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));

  /* Filling server information */
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  int n, len;

  sendto(sockfd,
         (const char*)hello,
         strlen(hello),
         MSG_CONFIRM,
         (const struct sockaddr*)&servaddr,
         sizeof(servaddr));
  printf("message sent: %s\n", hello);

  while (1) {
    n = recvfrom(sockfd,
                 (char*)buffer,
                 MAXLINE,
                 MSG_WAITALL,
                 (struct sockaddr*)&servaddr,
                 (uint32_t*)&len);
    buffer[n] = '\0';
    printf("Server : %s\n", buffer);
  }

  close(sockfd);
  return 0;
}
