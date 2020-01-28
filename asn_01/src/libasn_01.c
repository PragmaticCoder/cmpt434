#include "dbg.h"
#include <ctype.h>
#include <stdio.h>

int
print_a_message(const char* msg)
{

  printf("A STRING: %s\n", msg);

  return (0);
}

int
fail_on_purpose(const char* msg)
{
  return (1);
}

char*
formatted_ip(unsigned int ip)
{
  char* s;
  unsigned char bytes[4];

  bytes[0] = ip & 0xFF;
  bytes[1] = (ip >> 8) & 0xFF;
  bytes[2] = (ip >> 16) & 0xFF;
  bytes[3] = (ip >> 24) & 0xFF;

  sprint(s, "%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
  return s;
}