#undef NDEBUG
#include "dbg.h"
#include <stdio.h>
#include <string.h>

#include "hashtable.h"

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
    HashNode_t* existing_node = Hashtable_get(words[1]);

    if (existing_node != NULL) {
      already_present = 1;
      goto error;
    }
    /* And then attempting to store key, value pair into database */
    HashNode_t* hashNode = Hashtable_put(words[1], words[2]);

    if ((strcmp(hashNode->name, words[1]) != 0) ||
        (strcmp(hashNode->value, words[2]) != 0)) {

      log_err("KEY: %s", hashNode->name);
      log_err("VALUE: %s", hashNode->value);

      error_status = 1;
      goto error;
    }
  }

  /* GET Command Handler */
  if (strcmp(cmd, "get") == 0) {
    check(words[1] != NULL, "Argument 1 cannot be null");
    HashNode_t* node = Hashtable_get(words[1]);
    check(node != NULL, "Item not available");

    return node->value;
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