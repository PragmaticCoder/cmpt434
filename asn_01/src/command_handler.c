#undef NDEBUG

#include "dbg.h"
#include "libasn_01.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

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

    if (strlen(line) <= 2) /* will skip if empty line*/
      continue;

    char* words[] = { NULL, NULL, NULL };
    split_into_words(line, words);
    int len = strlen(words[1]);

    if (strncmp(key, words[0], len - 2) == 0) {
      debug("FOUND!!!: %s", words[1]);
      strcpy(value, words[1]);
      
      fclose(fd);
      return 0;
    }
  }

  fclose(fd);
  return -1;
}

int
Database_remove(char key[])
{
  char line[256];
  FILE* fd = fopen("storage", "r");

  while (fgets(line, sizeof(line), fd)) {

    if (strlen(line) <= 2) /* will skip if empty line*/
      continue;

    char* words[] = { NULL, NULL, NULL };
    split_into_words(line, words);
    int len = strlen(words[1]);

    if (strncmp(key, words[0], len - 2) == 0) {
      debug("FOUND!!!: %s", words[1]);
      /* TODO: Remove line here */

      fclose(fd);
      return 0;
    }
  }

  fclose(fd);
  return -1;
}

void
Database_getall(char* value)
{
  char ch;
  FILE* fd = fopen("storage", "r");
  ch = getc(fd);

  int i = 0;
  while (ch != EOF) {
    printf("%c", ch);

    ch = getc(fd);
    debug("ch: %c", ch);
    append(value, (char)ch);
  }
  
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

    char value[50];
    if (Database_getval(words[1], value) == 0) {
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

    debug("result: %s", result);
    debug("value: %s", value);

    return result;
  }

  /* Get All */
  char all_items[1000];
  if (strcmp(cmd, "getall") == 0) {

    Database_getall(all_items);

    char* result = (char*)malloc(1000 * sizeof(char));
    check_mem(result);

    strcpy(result, value);

    debug("result: %s", result);
    debug("value: %s", value);
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
    return "Wrong command! Please enter a valid command";

  if (words[2] == NULL)
    return "Second command param missing!";

  return "Success";
}