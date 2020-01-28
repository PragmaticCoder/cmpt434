#include "minunit.h"
#include <dlfcn.h>

typedef int (*Hashtable_get)(char* data);
typedef int (*Hashtable_put)(char* name, char* value);
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
all_tests()
{
  mu_suite_start();

  mu_run_test(test_dlopen);

  return NULL;
}

RUN_TESTS(all_tests);