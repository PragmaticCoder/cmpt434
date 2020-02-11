/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */

#include "minunit.h"
#include <libasn_02.h>

char*
test_dummy()
{
  return NULL;
}


char *test_print_a_message()
{
    int rc = print_a_message("Printing Tests");
    mu_assert(rc != 0, "Failed to execute print_a_message.");
    return NULL;
}

char*
all_tests()
{
  mu_suite_start();

  mu_run_test(test_print_a_message);

  return NULL;
}

RUN_TESTS(all_tests);
