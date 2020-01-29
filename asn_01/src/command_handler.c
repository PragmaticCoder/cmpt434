#undef NDEBUG
#include "dbg.h"
#include <stdio.h>
#include <string.h>

void
split_into_words(char* str, char** words)
{
  char* pch = strtok(str, " ");

  int i = 0;
  words[i] = pch;

  while (pch != NULL) {
    pch = strtok(NULL, " ");
    words[++i] = pch;
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
