#undef NDEBUG

#include "minunit.h"

#include <dlfcn.h>
#include <string.h>

typedef char* (*lib_Get_Cmd_Type)(char* msg);
typedef char* (*lib_Split_Into_Words)(char* msg, char** words);

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