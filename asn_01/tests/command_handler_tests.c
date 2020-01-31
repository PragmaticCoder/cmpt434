#undef NDEBUG

#include "minunit.h"

#include <dlfcn.h>
#include <string.h>

typedef char* (*lib_Split_Into_Words)(char* msg, char** words);
typedef char* (*lib_Command_Handler)(char* user_input);

char* lib_file = "./build/libasn_01.so";
void* lib = NULL;

char*
test_dlopen()
{
  lib = dlopen(lib_file, RTLD_NOW);
  mu_assert(lib != NULL, "Failed to open the library to test.");

  return NULL;
}

char*
test_split_into_words()
{
  lib_Split_Into_Words func_split_into_words = dlsym(lib, "split_into_words");
  mu_assert(func_split_into_words != NULL, "Failed to find get_word function.");

  char input[50] = "put name Alvi";

  char* words[] = { NULL, NULL, NULL };

  func_split_into_words(input, words);

  mu_assert(strcmp(words[0], "put") == 0, "Failed: put - wrong word!");
  mu_assert(strcmp(words[1], "name") == 0, "Failed: name - wrong key!");
  mu_assert(strcmp(words[2], "Alvi") == 0, "Failed: name - wrong key!");

  return NULL;
}

char*
test_command_handler_put_success()
{
  lib_Command_Handler func_command_handler = dlsym(lib, "command_handler");
  mu_assert(func_command_handler != NULL,
            "Failed to find command_handler function.");

  char input[50] = "put name Alvi";

  char* response_msg;

  response_msg = func_command_handler(input);
  mu_assert(strcmp(response_msg, "Success") == 0, "Command Failure!");

  strcpy(input, "put age 26");
  response_msg = func_command_handler(input);
  mu_assert(strcmp(response_msg, "Success") == 0, "Command Failure!");

  strcpy(input, "put gender male");
  response_msg = func_command_handler(input);
  mu_assert(strcmp(response_msg, "Success") == 0, "Command Failure!");

  return NULL;
}

char*
test_command_handler_get_success()
{
  lib_Command_Handler func_command_handler = dlsym(lib, "command_handler");
  mu_assert(func_command_handler != NULL,
            "Failed to find command_handler function.");

  char input[50] = "get name";
  char* response_msg;

  response_msg = func_command_handler(input);
  mu_assert(strncmp(response_msg, "Alvi", 4) == 0,
            "Error getting value during testing!");

  strcpy(input, "get age");
  response_msg = func_command_handler(input);
  mu_assert(strncmp(response_msg, "26", 2) == 0,
            "Error getting value during testing!");

  strcpy(input, "get gender");
  response_msg = func_command_handler(input);
  mu_assert(strncmp(response_msg, "male", 4) == 0,
            "Error getting value during testing!");

  return NULL;
}

char*
test_command_handler_get_all()
{
  lib_Command_Handler func_command_handler = dlsym(lib, "command_handler");
  mu_assert(func_command_handler != NULL,
            "Failed to find command_handler function.");

  char input[50] = "getall";

  char* response_msg;
  response_msg = func_command_handler(input);
  debug("Response Msg: %s", (char*)response_msg);
  // mu_assert(strcmp(response_msg, "Success") == 0, "Command Failure!");

  return NULL;
}

char*
test_dlclose()
{
  int rc = dlclose(lib);
  mu_assert(rc == 0, "Failed to close Hashtable lib.");

  return NULL;
}

char*
all_tests()
{
  mu_suite_start();

  mu_run_test(test_dlopen);
  mu_run_test(test_split_into_words);
  mu_run_test(test_command_handler_put_success);
  mu_run_test(test_command_handler_get_success);
  // mu_run_test(test_command_handler_get_all);
  mu_run_test(test_dlclose);

  return NULL;
}

RUN_TESTS(all_tests);