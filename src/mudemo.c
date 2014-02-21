#include "minunit.h"

char *test_should_fail()
{
	mu_assert(0 == 1, "Zero is not equal to 1 (duh).");

	return NULL;
}

char *test_should_succeed()
{
	mu_assert(1 == 1, "One is not equal to one.");

	return NULL;
}

char *all_tests()
{
	mu_suite_start();

	mu_run_test(test_should_fail);
	mu_run_test(test_should_succeed);

	return NULL;
}

RUN_TESTS(all_tests);