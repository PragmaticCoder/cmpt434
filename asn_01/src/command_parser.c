#undef NDEBUG
#include "dbg.h"
#include <stdio.h>
#include <string.h>

void
get_word(char* str, char** words)
{
  char* pch = strtok(str, " ");

  int i = 0;
  words[i] = pch;

  while (pch != NULL) {
    i++;
    pch = strtok(NULL, " ");
    words[i] = pch;
  }
}

int
fail_function(const char* msg)
{
  return (1);
}
