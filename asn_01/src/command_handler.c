#undef NDEBUG

#include "dbg.h"
#include <fcntl.h>
#include <stdio.h>
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
Database_put(char* key, char * value)
{
  FILE* fd = fopen("storage", "a");

  fputs(key, fd);
  fputs(" ", fd);
  fputs(value, fd);
  fputs("\n", fd);

  fclose(fd);
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

    /* First checking if the key is already present in DB */
    

    /* And then attempting to store key, value pair into database */
    Database_put(words[1], words[2]);
  }

  /* GET Command Handler */
  // if (strcmp(cmd, "get") == 0) {
  //   check(words[1] != NULL, "Argument 1 cannot be null");

  //   HashNode_t* node = Hashtable_get(words[1]);
  //   if (node == NULL) {
  //     item_not_available = 1;
  //     goto error;
  //   }

  //   return node->value;
  // }

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