/**
 * Name: Alvi Akbar
 * NSID: ala273
 * Student Number: 11118887
 */

#include "minunit.h"
#include <frame.h>
#include <string.h>

char *
test_first_frame_add_success()
{
  frame_t *frame;
  frame = Frame_add("100");

  mu_assert(frame != NULL, "Failed to add a Frame.");
  mu_assert(frame->index == 0, "Frame index is not zero.");

  return NULL;
}

char *
test_frame_add_correct_sequence()
{
  frame_t *frame;
  frame = Frame_add("101");

  mu_assert(frame != NULL, "Failed to add a Frame.");
  mu_assert(strncmp(frame->message, "101", 3) == 0, "Wrong frame message returned.");
  mu_assert(frame->index == 1, "Wrong index returned.");

  return NULL;
}

char *
test_get_head()
{
  frame_t *frame;
  frame = Get_head();

  mu_assert(frame != NULL, "No frame available on head.");
  mu_assert(strncmp(frame->message, "100", 3) == 0, "Wrong frame message returned.");
  mu_assert(frame->index == 0, "Incorrect frame index.");

  return NULL;
}

char *
test_get_tail()
{
  frame_t *frame;
  frame = Get_tail();

  mu_assert(frame != NULL, "No frame available on tail.");
  mu_assert(strncmp(frame->message, "100", 3) == 0, "Wrong frame message returned.");
  mu_assert(frame->index == 0, "Incorrect frame index.");

  return NULL;
}

char *
all_tests()
{
  mu_suite_start();

  mu_run_test(test_first_frame_add_success);
  mu_run_test(test_frame_add_correct_sequence);

  mu_run_test(test_get_head);
  mu_run_test(test_get_tail);

  return NULL;
}

RUN_TESTS(all_tests);
