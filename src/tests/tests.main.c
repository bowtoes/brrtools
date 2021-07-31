#include <stdio.h>

#include "tests/log.h"
#include "tests/lib.h"
#include "tests/mem.h"
#include "tests/stg.h"
#include "tests/path.h"

int main(void)
{
	brrsz total = 0;
	brrsz run = 0;
	brrsz skip = 0;
	brrsz succ = 0;
	brrsz fail = 0;
	brrtools_brrlog_test(&total, &run, &skip, &succ, &fail);
	brrtools_brrmem_test(&total, &run, &skip, &succ, &fail);
	brrtools_brrlib_test(&total, &run, &skip, &succ, &fail);
	brrtools_brrstg_test(&total, &run, &skip, &succ, &fail);
	brrtools_brrpath_test(&total, &run, &skip, &succ, &fail);

	fprintf(stderr, "\nRan a total of %zu/%zu tests. %zu succeeded, %zu failed, %zu skipped.\n", run, total, succ, fail, skip);
}
