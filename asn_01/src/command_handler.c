#undef NDEBUG
#include "dbg.h"
#include <stdio.h>
#include <string.h>

void
split_into_words(char* str, char** words)
{
  char* ptr = strtok(str, " ");

  int i = 0;
  words[i] = ptr;

  while (ptr != NULL) {
    ptr = strtok(NULL, " ");
    words[++i] = ptr;
  }
}

void
command_handler(char* str, char** words)
{
  
}

int
fail_function(const char* msg)
{
  return (1);
}
