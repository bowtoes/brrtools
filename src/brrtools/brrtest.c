/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

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

static void BRRCALL
addtest(const char *const test_name, const char *(*const test_fn)(void), brrtest_suiteT *const suite)
{
	if (!brrlib_alloc((void **)&suite->tests, (suite->ntests + 1) * sizeof(*suite->tests), 0)) {
		fprintf(stderr, "FAILED TO ALLOCATE TEST: %s", strerror(errno));
		exit(1);
	}
	suite->tests[suite->ntests] = (brrtest_testT){.name = test_name, .test = test_fn};
	suite->ntests++;
}
static void BRRCALL
runtest(brrtest_testT *const test)
{
	test->message = test->test();
	if (!test->message) /* NULL msg means test is skipped. */
		test->skip = 1;
	else if (test->message[0]) /* Non-empty means test failed, with error being in message. */
		test->fail = 1;
}

void BRRCALL
(BRRTEST_ADD_TESTS)(brrtest_suiteT *const suite, ...)
{
	va_list lptr;
	brrtest_testT *test;
	va_start(lptr, suite);
	while ((test = va_arg(lptr, brrtest_testT *))) {
		addtest(test->name, test->test, suite);
	}
	va_end(lptr);

}
void BRRCALL
(BRRTEST_RUN_SUITE)(brrtest_suiteT *const suite)
{
	printf("Running suite '%s' with %zu...\n", suite->name, suite->ntests);
	for (brru8 i = 0; i < suite->ntests; ++i) {
		brrtest_testT *const t = &suite->tests[i];
		runtest(t);
		if (t->skip) {
			printf("SUITE %s %zu/%zu Skipped: %s\n", suite->name, i + 1, suite->ntests, t->name);
			suite->nskip++;
		} else {
			suite->nrun++;
			if (t->fail) {
				printf("SUITE %s %zu/%zu Failed: %s: %s\n", suite->name, i + 1, suite->ntests, t->name, t->message);
				suite->nfail++;
				if (suite->skip_on_fail) {
					suite->nskip += suite->ntests - (i + 1);
					break;
				}
			} else {
				printf("SUITE %s %zu/%zu Succeeded: %s\n", suite->name, i + 1, suite->ntests, t->name);
				suite->nsucc++;
			}
		}
	}
	printf("Ran a total of %zu/%zu tests. Skipped %zu.\n", suite->nrun, suite->ntests, suite->nskip);
	if (suite->nrun)
		printf("Succeeded: %zu/%zu.\n", suite->nsucc, suite->nrun);
	brrlib_alloc((void **)&suite->tests, 0, 0);
}
