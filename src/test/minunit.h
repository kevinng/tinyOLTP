/*
Unit testing macros from Zed Shaw (c.learncodethehardway.org).

(Maybe minor modifications.)
 */

#undef NDEBUG
#ifndef __MINUNIT_HDR__
#define __MINUNIT_HDR__

#include <stdio.h>
#include "dbg.h"
#include <stdlib.h>

#define mu_suite_start() char *message = NULL;

#define mu_assert(test, message) if (!(test)) { log_err(message); return message; }
#define mu_assert_with_clean_up(test, message, clean_up) if(!(test)) { log_err(message); clean_up; return message; }
#define mu_run_test(test) debug("\n-----%s", " " #test);\
	message = test(); tests_run++; if (message) return message;

#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; /* Just to stop the compiler from warning. */\
	debug("----- RUNNING: %s", argv[0]);\
	printf("----\nRUNNING: %s\n", argv[0]);\
	char *result = name();\
	if (result != NULL) {\
		printf("FAILED: %s\n", result);\
	}\
	else {\
		printf("ALL TESTS PASSED\n");\
	}\
	printf("Tests run: %d\n", tests_run);\
	exit(result != 0);\
}

int tests_run;

#endif


/*
EXAMPLE USAGE
-------------

#include "minunit.h"
#include <dlfcn.h>

typedef int (*lib_function)(const char *data);
char *lib_file = "build/libYOUR_LIBRARY.so";
void *lib = NULL;

int check_function(const char *func_to_run, const char *data, int expected)
{
    lib_function func = dlsym(lib, func_to_run);
    check(func != NULL, "Did not find %s function in the library %s: %s", func_to_run, lib_file, dlerror());

    int rc = func(data);
    check(rc == expected, "Function %s return %d for data: %s", func_to_run, rc, data);

    return 1;
error:
    return 0;
}

char *test_dlopen()
{
    lib = dlopen(lib_file, RTLD_NOW);
    mu_assert(lib != NULL, "Failed to open the library to test.");

    return NULL;
}

char *test_functions()
{
    mu_assert(check_function("print_a_message", "Hello", 0), "print_a_message failed.");
    mu_assert(check_function("uppercase", "Hello", 0), "uppercase failed.");
    mu_assert(check_function("lowercase", "Hello", 0), "lowercase failed.");

    return NULL;
}

char *test_failures()
{
    mu_assert(check_function("fail_on_purpose", "Hello", 1), "fail_on_purpose should fail.");

    return NULL;
}

char *test_dlclose()
{
    int rc = dlclose(lib);
    mu_assert(rc == 0, "Failed to close lib.");

    return NULL;
}

char *all_tests() {
    mu_suite_start();

    mu_run_test(test_dlopen);
    mu_run_test(test_functions);
    mu_run_test(test_failures);
    mu_run_test(test_dlclose);

    return NULL;
}

RUN_TESTS(all_tests);
*/