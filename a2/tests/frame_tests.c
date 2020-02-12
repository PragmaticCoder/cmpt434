/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */
#undef NDEBUG
#include "minunit.h"
#include <frame.h>
#include <string.h>


char*
test_first_frame_add_success()
{   
    frame_t *frame;
    frame = Frame_add("101");

    mu_assert(frame != NULL, "Failed to add a Frame.");
    mu_assert(frame->index == 0, "Frame index is not zero.");

    debug("%s\n", frame->message);
    return NULL;
}

char*
test_frame_add_correct_sequence()
{
    frame_t *frame;
    frame = Frame_add("110");

    mu_assert(frame != NULL, "Failed to add a Frame.");
    mu_assert(strncmp(frame->message, "110", 3) == 0, "Wrong frame message returned.")
    mu_assert(frame->index == 1, "Wrong index returned.");

    return NULL;
}

char*
test_frame_add_success()
{
    frame_t *frame;
    frame = Frame_add("101");

    mu_assert(frame != NULL, "Failed to create frame.");
    mu_assert(frame->index == 1, "Wrong index returned.");

    return NULL;
}


char*
all_tests()
{
  mu_suite_start();

  mu_run_test(test_first_frame_add_success);
  mu_run_test(test_frame_add_correct_sequence);

  return NULL;
}

RUN_TESTS(all_tests);
