#undef NDEBUG

#include "minunit.h"
#include "hashtable.h"

#include <dlfcn.h>
#include <string.h>

typedef struct HashNode HashNode_t;

typedef HashNode_t* (*lib_Hashtable_get)(const char* data);
typedef HashNode_t* (*lib_Hashtable_put)(const char* key, const char* value);

char* lib_file = "./build/libasn_01.so";
void* lib = NULL;

HashNode_t* node;

char*
test_dlopen()
{
  lib = dlopen(lib_file, RTLD_NOW);
  mu_assert(lib != NULL, "Failed to open the library to test.");

  return NULL;
}

char*
test_put_success()
{

  lib_Hashtable_put func_Hashtable_put = dlsym(lib, "Hashtable_put");
  mu_assert(func_Hashtable_put != NULL, "Failed to create Hash Node.");

  char* key = "name";
  char * value = "Alvi";

  HashNode_t* node = func_Hashtable_put("name", "Alvi");
  mu_assert(node != NULL, "Failed to put (key, value) pair into hashtable");
  
  mu_assert(strcmp(node->name, key) == 0, "Failed: HashNode key mismatch");
  mu_assert(strcmp(node->value, value) == 0, "Failed: HashNode value mismatch");

  return NULL;
}

char*
all_tests()
{
  mu_suite_start();

  mu_run_test(test_dlopen);
  mu_run_test(test_put_success);

  return NULL;
}

RUN_TESTS(all_tests);