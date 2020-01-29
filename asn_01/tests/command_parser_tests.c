#undef NDEBUG

#include "minunit.h"

#include <dlfcn.h>
#include <string.h>

typedef int (*lib_function)(const char* data);

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
test_get_command()
{

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
  mu_run_test(test_get_command);
  mu_run_test(test_get_param);
  mu_run_test(test_get_params);
  mu_run_test(test_dlclose);

  return NULL;
}

RUN_TESTS(all_tests);