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