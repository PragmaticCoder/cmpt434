#undef NDEBUG
#include "dbg.h"
#include <stdio.h>
#include <string.h>

/*
  COMMANDS:
• add key value : add (key, value) pair, if no existing pair with same key value
• getvalue key : return value from matching (key, value) pair, if any
• getall : return all (key, value) pairs
• remove key : remove matching (key, value) pair, if any
• quit : terminate client
*/

char**
get_words(char* msg)
{
  char* words[3];

  char* pch = strtok(msg, " ");
  check(pch != NULL, "Incorrect string format. No string detected");

  int i = 0;
  while (pch != NULL && i < 3) {
    words[i] = pch;
    pch = strtok(NULL, " ");
  }

  return words;

error:
  return words;
}

char*
get_cmd_type(char* msg)
{
  char** inputs = get_words(msg);

  int i;
  for (i = 0; i < 3; i++)
    printf("%s", inputs[i]);

  return "add";
}

int
fail_function(const char* msg)
{
  return (1);
}
