/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "brrtools/brrtest.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlib.h"
#include "brrtools/brrnum.h"

static void BRRCALL
addtest(const char *const test_name, const char *(*const test_fn)(void), brrtest_suite_t *const suite)
{
	brrsz tnlen;
	if (brrlib_alloc((void **)&suite->tests, (suite->n_tests + 1) * sizeof(*suite->tests), 0)) {
		fprintf(stderr, "FAILED TO ALLOCATE TEST: %s", strerror(errno));
		exit(1);
	}
	tnlen = strlen(test_name);
	if (tnlen > suite->max_test_name_length)
		suite->max_test_name_length = tnlen;
	suite->tests[suite->n_tests] = (brrtest_test_t){.name = test_name, .test = test_fn};
	suite->n_tests++;
}
static void BRRCALL
runtest(brrtest_test_t *const test)
{
	test->message = test->test();
	if (!test->message) /* NULL msg means test is skipped. */
		test->skipped = brrtrue;
	else if (test->message[0]) /* Non-empty means test failed, with error being in message. */
		test->failed = brrtrue;
}

void BRRCALL
brrtest_add_tests(brrtest_suite_t *const suite, ...)
{
	va_list lptr;
	brrtest_test_t *test;
	va_start(lptr, suite);
	while ((test = va_arg(lptr, brrtest_test_t *)))
		addtest(test->name, test->test, suite);
	va_end(lptr);

}
void BRRCALL
brrtest_run_suite(brrtest_suite_t *const suite)
{
	int ndigits = brrnum_ndigits(suite->n_tests, 0, 10);
	printf("Running suite '%s' with %zu...\n", suite->name, suite->n_tests);
	for (brru8 i = 0; i < suite->n_tests; ++i) {
		brrtest_test_t *const t = &suite->tests[i];
		printf("SUITE %s Running test %*zu/%zu %-*s ... ", suite->name, ndigits, i + 1, suite->n_tests, suite->max_test_name_length, t->name);
		fflush(stdout);
		fflush(stderr);
		runtest(t);
		if (t->skipped) {
			printf("SKIPPED\n");
			suite->n_tests_skipped++;
		} else {
			suite->n_tests_run++;
			if (t->failed) {
				suite->n_tests_failed++;
				printf("FAILED : %s\n", t->message);
				if (suite->skip_on_failure) {
					suite->n_tests_skipped += suite->n_tests - (i + 1);
					break;
				}
			} else {
				suite->n_tests_passed++;
				printf("SUCCEEDED\n");
			}
		}
	}
	printf("Ran a total of %zu/%zu tests. Skipped %zu.\n", suite->n_tests_run, suite->n_tests, suite->n_tests_skipped);
	if (suite->n_tests_run)
		printf("Succeeded: %zu/%zu.\n", suite->n_tests_passed, suite->n_tests_run);
	brrlib_alloc((void **)&suite->tests, 0, 0);
}
