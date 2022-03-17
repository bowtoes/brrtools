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

#ifndef BRRTEST_H
#define BRRTEST_H

#include <stdio.h>

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrmacro.h>
#include <brrtools/brrmacro_map.h>

_brrcppstart

#define BRRTOOLS_TEST_MESSAGE_LENGTH 2048
extern char brrtools_test_message_string[BRRTOOLS_TEST_MESSAGE_LENGTH];
#define brrtools_test_message(...)\
(\
	snprintf(brrtools_test_message_string, BRRTOOLS_TEST_MESSAGE_LENGTH, \
		__VA_ARGS__), brrtools_test_message_string\
)

typedef struct brrtest_test {
	const char *name;
	const char *(*test)(void);
	const char *message;
	brrbl skipped:1;
	brrbl failed:1;
} brrtest_test_t;
typedef struct brrtest_suite {
	const char *name;
	brrtest_test_t *tests;
	brru8 n_tests;
	brru8 n_tests_run;
	brru8 n_tests_skipped;
	brru8 n_tests_passed;
	brru8 n_tests_failed;
	int max_test_name_length;
	brrbl skip_on_failure;
} brrtest_suite_t;

BRRAPI void BRRCALL brrtest_add_tests(brrtest_suite_t *const suite, ...);
BRRAPI void BRRCALL brrtest_run_suite(brrtest_suite_t *const suite);

#define _brrtest_get_suite_name(_suite_) BRRMACRO_PASTE(_brrtest_suite_t_,_suite_)
#define _brrtest_get_test_name(_test_) BRRMACRO_PASTE(_brrtest_test_t_,_test_)
#define _brrtest_test_function(_name_) BRRMACRO_PASTE(_brrtest_get_test_name(_name_),_fn)
#define _brrtest_suite_add_function(_suite_) BRRMACRO_PASTE(_brrtest_get_suite_name(_suite_),_add_tests)
#define _brrtest_get_test_ref(_test_) &_brrtest_get_test_name(_test_)

#define BRRTEST_NEW_TEST(_test_name_) \
    static const char *BRRCALL _brrtest_test_function(_test_name_)(void); \
    static brrtest_test_t _brrtest_get_test_name(_test_name_) = {.name=#_test_name_,.test=_brrtest_test_function(_test_name_)}; \
    static const char *BRRCALL _brrtest_test_function(_test_name_)(void)

#define BRRTEST_NEW_SUITE(_suite_, _skip_on_failure_, ...) \
	static brrtest_suite_t _brrtest_get_suite_name(_suite_) = {.name=#_suite_,.skip_on_failure=_skip_on_failure_}; \
	static int BRRMACRO_PASTE(_brrtest_suite_add_function(_suite_),_ran) = 0; \
	static void BRRCALL _brrtest_suite_add_function(_suite_)(void) { \
		if (!BRRMACRO_PASTE(_brrtest_suite_add_function(_suite_),_ran)) { \
			brrtest_add_tests(&_brrtest_get_suite_name(_suite_), BRRMACRO_MAP_LIST(_brrtest_get_test_ref, __VA_ARGS__), NULL); \
			BRRMACRO_PASTE(_brrtest_suite_add_function(_suite_),_ran) = 1; \
		} \
	}

#define BRRTEST_RUN_SUITE(_suite_) do { \
	_brrtest_suite_add_function(_suite_)(); \
	brrtest_run_suite(&_brrtest_get_suite_name(_suite_)); \
} while (0)


#define BRRTEST_SUITE_TOTAL(_suite_)           ((_brrtest_get_suite_name(_suite_)).n_tests)
#define BRRTEST_SUITE_RUN(_suite_)             ((_brrtest_get_suite_name(_suite_)).n_tests_run)
#define BRRTEST_SUITE_SKIP(_suite_)            ((_brrtest_get_suite_name(_suite_)).n_tests_skipped)
#define BRRTEST_SUITE_PASSED(_suite_)          ((_brrtest_get_suite_name(_suite_)).n_tests_passed)
#define BRRTEST_SUITE_FAIL(_suite_)            ((_brrtest_get_suite_name(_suite_)).n_tests_failed)
#define BRRTEST_SUITE_SKIP_ON_FAILURE(_suite_) ((_brrtest_get_suite_name(_suite_)).skip_on_failure)

#define BRRTEST_ASSERT(_cond_, _msg_) do { if (!(_cond_)) return (_msg_); } while (0)

_brrcppend

#endif /* BRRTEST_H */
