
// Server side implementation of UDP client-server model
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024
#define MAX 1024

void
udp_server_parse(char buffer[], char* command, uint16_t size, uint16_t* offset)
{
  int i = 0;

  for (i = 0; i < size; i++) {
    if (buffer[i] != ' ') {
      command[i] = buffer[i];
    } else {
      command[i + 1] = '\0';
      *offset += i + 1;
      i = size;
    }
  }
}

void
udp_server_add(char* buffer)
{
  int fd = open("storage", O_WRONLY | O_CREAT | O_APPEND, 0x777);

  write(fd, buffer, strlen(buffer));
  write(fd, "\n", strlen("\n"));

  close(fd);
}

int
udp_server_getval(char key[], char* value)
{
  char line[256];
  int fd;
  int ndx = 0;
  char keyLine[40];
  int lung;
  uint16_t offset = 0;
  int flag = 0;

  fd = open("storage", O_RDONLY);

  while ((lung = read(fd, line + ndx, 1) > 0) && (flag == 0)) {
    ndx++;

    if (line[ndx - 1] == ' ') {
      line[ndx - 1] = '\0';

      if (strcmp(line, key) == 0) {
        line[ndx - 1] = ' ';

        while (line[ndx - 1] != '\n') {
          read(fd, line + ndx, 1);

          ndx++;
        }

        line[ndx - 1] = '\0';

        udp_server_parse(line, keyLine, strlen(line), &offset);
        udp_server_parse(line + offset, value, strlen(line), &offset);
        flag = 1;
      } else {
        line[ndx - 1] = ' ';

        while (line[ndx - 1] != '\n') {
          read(fd, line + ndx, 1);

          ndx++;
        }
      }

      memset(line, 0, strlen(line));

      ndx = 0;
      offset = 0;
    }
  }

  close(fd);

  return flag;
}

int
udp_find_line(char key[])
{
  char line[256];
  int fd;
  int ndx = 0;
  int lung;
  int cnt = 0;
  int flag = 0;
  int position = 0xffffffffu;

  fd = open("storage", O_RDONLY);

  while ((lung = read(fd, line + ndx, 1) > 0) && (flag == 0)) {
    ndx++;

    if (line[ndx - 1] == ' ') {
      line[ndx - 1] = '\0';

      if (strcmp(line, key) == 0) {
        flag = 1;
        position = cnt;
      } else {
        line[ndx - 1] = ' ';

        while (line[ndx - 1] != '\n') {
          read(fd, line + ndx, 1);

          ndx++;
        }
      }

      memset(line, 0, strlen(line));

      ndx = 0;
      cnt++;
    }
  }

  close(fd);

  return position;
}

void
udp_server_getall(int sockfd, struct sockaddr_in* cliaddr, socklen_t len)
{
  char line[256];
  int fd = open("storage", O_RDONLY);
  int ndx = 0;

  while (read(fd, line + ndx, 1) > 0) {
    ndx++;

    if (line[ndx - 1] == '\n') {
      line[ndx - 1] = '\0';

      sendto(sockfd,
             (const char*)line,
             strlen(line),
             MSG_CONFIRM,
             (const struct sockaddr*)cliaddr,
             len);

      ndx = 0;

      memset(line, 0, strlen(line));
    }
  }
}

void
udp_server_remove(int lno)
{
  int ctr = 0;
  char ch;
  FILE *fptr1, *fptr2;
  char fname[] = "storage";
  char str[MAX], temp[] = "temp.txt";

  fptr1 = fopen(fname, "r");
  if (!fptr1) {
    printf(" File not found or unable to open the input file!!\n");
  }
  fptr2 = fopen(temp, "w"); // open the temporary file in write mode
  if (!fptr2) {
    printf("Unable to open a temporary file to write!!\n");
    fclose(fptr1);
  }

  lno++;
  // copy all contents to the temporary file except the specific line
  while (!feof(fptr1)) {
    strcpy(str, "\0");
    fgets(str, MAX, fptr1);
    if (!feof(fptr1)) {
      ctr++;
      /* skip the line at given line number */
      if (ctr != lno) {
        fprintf(fptr2, "%s", str);
      }
    }
  }
  fclose(fptr1);
  fclose(fptr2);
  remove(fname);
  rename(temp, fname);

  fptr1 = fopen(fname, "r");
  ch = fgetc(fptr1);
  while (ch != EOF) {
    ch = fgetc(fptr1);
  }
  fclose(fptr1);
}

// Driver code
int
main()
{
  int sockfd;
  char buffer[MAXLINE];
  char cmd[40];
  char key[40];
  char val[40];
  int pos = 0;
  socklen_t len;
  uint16_t n;
  uint16_t offset = 0;
  struct sockaddr_in servaddr, cliaddr;

  // Creating socket file descriptor
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket creation failed");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  // Filling server information
  servaddr.sin_family = AF_INET; // IPv4
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // Bind the socket with the server address
  if (bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  len = sizeof(cliaddr); // len is value/resuslt

  while (1) {
    n = recvfrom(sockfd,
                 (char*)buffer,
                 MAXLINE,
                 MSG_WAITALL,
                 (struct sockaddr*)&cliaddr,
                 &len);

    buffer[n] = '\0';

    memset(cmd, 0, sizeof(cmd));

    udp_server_parse(buffer, cmd, n, &offset);

    memset(key, 0, sizeof(key));
    memset(val, 0, sizeof(val));

    if (strcmp(cmd, "add") == 0u) {
      udp_server_add(buffer + offset);
    } else if (strcmp(cmd, "getv") == 0u) {
      udp_server_parse(buffer + offset, key, n, &offset);

      if (udp_server_getval(key, val) == 1) {
        sendto(sockfd,
               (const char*)val,
               strlen(val),
               MSG_CONFIRM,
               (const struct sockaddr*)&cliaddr,
               len);
      } else {
        sendto(sockfd,
               (const char*)"none",
               strlen("none"),
               MSG_CONFIRM,
               (const struct sockaddr*)&cliaddr,
               len);
      }
    } else if (strcmp(cmd, "geta") == 0u) {
      udp_server_getall(sockfd, &cliaddr, len);
    } else if (strcmp(cmd, "rev") == 0u) {
      udp_server_parse(buffer + offset, key, n, &offset);

      if ((pos = udp_find_line(key)) != 0xFFFFFFFFu) {
        udp_server_remove(pos);
      }
    }
    offset = 0;
    memset(buffer, 0, sizeof(buffer));
  }

  return 0;
}
