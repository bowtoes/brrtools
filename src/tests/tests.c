#include "tests/tests.h"

int main(void)
{
	brrsz total = 0;
	brrsz run = 0;
	brrsz skip = 0;
	brrsz pass = 0;
	brrsz fail = 0;

	brrtools_test_run_suites(&total, &run, &skip, &pass, &fail);

	fprintf(stderr, "\nRan a total of %zu/%zu tests. %zu succeeded, %zu failed, %zu skipped.\n", run, total, pass, fail, skip);
}
