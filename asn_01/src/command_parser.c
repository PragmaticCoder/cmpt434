#include "dbg.h"
#include <regex.h>
#include <stdio.h>

/*
  COMMANDS:
• add key value : add (key, value) pair, if no existing pair with same key value
• getvalue key : return value from matching (key, value) pair, if any
• getall : return all (key, value) pairs
• remove key : remove matching (key, value) pair, if any
• quit : terminate client
*/

/* Utility Functions */
int
dummy_print(const char* msg)
{

  printf("A STRING: %s\n", msg);
  return (0);
}

int
fail_function(const char* msg)
{
  return (1);
}
