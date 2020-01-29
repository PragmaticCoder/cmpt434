#undef NDEBUG

#include "minunit.h"

#include <dlfcn.h>
#include <string.h>

typedef char* (*lib_Get_Cmd_Type)(char* msg);
typedef char** (*lib_Get_Words)(char* words);

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
test_get_words()
{
  lib_Get_Words func_get_words = dlsym(lib, "get_words");
  mu_assert(func_get_words != NULL, "Failed to find get_words function.");

  char input[50] = "add name Alvi";

  char** words = func_get_words(input);
  // mu_assert(words != NULL, "Failed: Array of Words is pointing to NULL");
  // mu_assert((sizeof(words)/sizeof(*words[0])) == 3, "Failed: length of array is not 3");
  
  // int i;
  // for (i = 0; i < 3; i++)
  debug("%p", words);
  // debug("words[0]: %s", words[0]);
  // mu_assert(strcmp(*words[0], "add"), "Failed: add - wrong word!");
  // mu_assert(strcmp(words[1], "name"), "Failed: name - wrong word!");
  // mu_assert(strcmp(words[2], "Alvi"), "Failed: value - wrong word!");

  return NULL;
}

char*
test_get_cmd_type()
{
  lib_Get_Cmd_Type func_get_cmd_type = dlsym(lib, "get_cmd_type");
  mu_assert(func_get_cmd_type != NULL, "Failed to find get_cmd_type function.");

  char* cmd = NULL;
  char input[50] = "add name Alvi";

  cmd = func_get_cmd_type(input);

  mu_assert(strcmp(cmd, "add") == 0, "Failed: extracting cmd from string");

  return NULL;
}

char*
test_get_param()
{

  return NULL;
}

char*
test_get_params()
{

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
  mu_run_test(test_get_words);
  // mu_run_test(test_get_cmd_type);
  // mu_run_test(test_get_params);
  mu_run_test(test_dlclose);

  return NULL;
}

RUN_TESTS(all_tests);