#include "dbg.h"

/* Utility Functions */
int
print_a_message(const char* msg)
{

  printf("A STRING: %s\n", msg);

  return (0);
}

void
append(char* s, char c)
{
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

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

int
fail_on_purpose(const char* msg)
{
  return (1);
}