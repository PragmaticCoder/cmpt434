#undef NDEBUG

#include "dbg.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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
Database_put(char* key, char* value)
{
  FILE* fd = fopen("storage", "a");

  fputs(key, fd);
  fputs(" ", fd);
  fputs(value, fd);
  fputs("\n", fd);

  fclose(fd);
}

int
Database_getval(char key[], char* value)
{
  char line[256];
  FILE* fd = fopen("storage", "r");

  while (fgets(line, sizeof(line), fd)) {
    debug("LINE: %s", line);

    if (strlen(line) <= 2) /* will skip if empty line*/
      continue;

    char* words[] = { NULL, NULL, NULL };
    split_into_words(line, words);
    debug("Inside Database --- KEY: %s, VALUE: %s", words[0], words[1]);

    if (strcmp(key, words[0]) == 0) {
      debug("FOUND!!!: %s", words[1]);
      value = words[1];
      return 0;
    }
  }

  return -1;
}

char*
command_handler(char* user_input)
{
  debug("Inside command_handler library function");
  debug("User Input: %s", user_input);

  int error_status = 0;
  int already_present = 0;
  int item_not_available = 0;

  check(user_input != NULL, "");
  check(strlen(user_input) > 0, "");

  char* words[] = { NULL, NULL, NULL };
  split_into_words(user_input, words);

  char* cmd = words[0];

  debug("COMMAND: %s", cmd);
  debug("Command Argument 1: %s", words[1]);
  debug("Command Argument 2: %s", words[2]);

  /* PUT Command Handler */
  if (strcmp(cmd, "put") == 0) {
    check(words[1] != NULL, "Argument 1 cannot be null");
    check(words[2] != NULL, "Argument 2 cannot be null");

    char value[50];
    if (Database_getval(words[1], value) == 1) {
      debug("Already in Database!: %s: %s", words[1], value);
      return "Key already exists in DB.";
    }

    /* And then attempting to store key, value pair into database */
    Database_put(words[1], words[2]);
  }

  /* GET Command Handler */
  char value[50];
  if (strcmp(cmd, "get") == 0) {
    check(words[1] != NULL, "Argument 1 cannot be null");

    if (Database_getval(words[1], value) == -1) {
      item_not_available = 1;
      goto error;
    }

    char* result = (char*)malloc(50 * sizeof(char));
    check_mem(result);

    strcpy(result, value);

    return result;
  }

  /*All error handlers */
error:
  if (error_status == 1)
    return "Error detected while interacting with database";

  if (already_present == 1)
    return "Item already present in database";

  if (item_not_available == 1)
    return "Item not available";

  if (user_input == NULL)
    return "User input cannot be NULL";

  if (strlen(user_input) <= 0)
    return "User input cannot be empty string";

  if (words[1] == NULL)
    return "First command param missing!";

  if (words[2] == NULL)
    return "Second command param missing!";

  return "Success";
}