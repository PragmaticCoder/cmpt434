#include "minunit.h"
#include <dlfcn.h>

typedef int (*lib_function)(const char *data);
char *lib_file = "build/libbase.so";
void *lib = NULL;

char *test_dummy() {
    return NULL;
}

char *all_tests() {
  mu_suite_start();

  mu_run_test(test_dummy);

  return NULL;
}

RUN_TESTS(all_tests);
