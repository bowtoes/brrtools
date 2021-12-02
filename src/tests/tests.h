#ifndef _brrtools_brrcon_test_h
#define _brrtools_brrcon_test_h

#include "brrtools/brrapi.h"
#include "brrtools/brrtypes.h"
#include "brrtools/brrtest.h"

#include "tests/con.c"
#include "tests/log.c"
#include "tests/num.c"
#include "tests/path.c"
#include "tests/rand.c"
//#include "tests/stringr.c"
#include "tests/time.c"

_brrcppstart

#define brrtools_test_run_suite(_name_, _global_total_, _global_run_,\
    _global_skipped_, _global_passed_, _global_failed_)\
do {\
	BRRTEST_RUN_SUITE(_name_);\
	if (_global_total_)\
		*_global_total_ += BRRTEST_SUITE_TOTAL(_name_);\
	if (_global_run_)\
		*_global_run_ += BRRTEST_SUITE_RUN(_name_);\
	if (_global_skipped_)\
		*_global_skipped_ += BRRTEST_SUITE_SKIP(_name_);\
	if (_global_passed_)\
		*_global_passed_ += BRRTEST_SUITE_PASSED(_name_);\
	if (_global_failed_)\
		*_global_failed_ += BRRTEST_SUITE_FAIL(_name_);\
} while (0)

#define brrtools_test_run_suites(_global_total_, _global_run_, _global_skipped_,\
    _global_passed_, _global_failed_)\
do {\
	brrtools_test_run_suite(con,     _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
	brrtools_test_run_suite(log,     _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
	brrtools_test_run_suite(num,     _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
	brrtools_test_run_suite(path,    _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
	brrtools_test_run_suite(rand,    _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
/*	brrtools_test_run_suite(stringr, _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
*/	brrtools_test_run_suite(time,    _global_total_, _global_run_, _global_skipped_, _global_passed_, _global_failed_);\
} while(0)

_brrcppend

#endif /* _brrtools_brrtime_test_h */
