#undef NDEBUG

#include "minunit.h"
#include "hashtable.h"

#include <dlfcn.h>
#include <string.h>


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
  mu_assert(func_Hashtable_put != NULL, "Failed to find Hashtable_put function.");

  char* key = "name";
  char * value = "Alvi";

  HashNode_t* node = func_Hashtable_put("name", "Alvi");
  mu_assert(node != NULL, "Failed to put (key, value) pair into hashtable");
  
  mu_assert(strcmp(node->name, key) == 0, "Failed: HashNode key mismatch");
  mu_assert(strcmp(node->value, value) == 0, "Failed: HashNode value mismatch");

  return NULL;
}

char*
test_get_success()
{

  lib_Hashtable_get func_Hashtable_get = dlsym(lib, "Hashtable_get");
  mu_assert(func_Hashtable_get != NULL, "Failed to find Hashtable_get function.");

  char* key = "name";
  char * value = "Alvi";

  HashNode_t* node = func_Hashtable_get("name");
  mu_assert(node != NULL, "Failed to get key from hashtable");
  
  mu_assert(strcmp(node->name, key) == 0, "Failed: HashNode key mismatch");
  mu_assert(strcmp(node->value, value) == 0, "Failed: HashNode value mismatch");
  
  return NULL;
}

char *test_dlclose()
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
  mu_run_test(test_put_success);
  mu_run_test(test_get_success);
  mu_run_test(test_dlclose);

  return NULL;
}

RUN_TESTS(all_tests);