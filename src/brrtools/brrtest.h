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

#ifndef BRRTEST_H
#define BRRTEST_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrtil.h>
#include <brrtools/brrmap.h>

BRRCPPSTART

typedef struct brrtest_suite brrtest_suiteT;
typedef struct brrtest_test {
	const char *name;
	const char *(*test)(void);
	const char *message;
	int skip;
	int fail;
} brrtest_testT;
struct brrtest_suite {
	const char *name;
	brrtest_testT *tests;
	brru8 ntests;
	brru8 nrun;
	brru8 nskip;
	brru8 nsucc;
	brru8 nfail;
	int skip_on_fail;
};

BRRAPI void BRRCALL
BRRTEST_ADD_TESTS(brrtest_suiteT *const suite, ...);
BRRAPI void BRRCALL
BRRTEST_RUN_SUITE(brrtest_suiteT *const suite);

#define _brrsuitename(_suite_) BRRTIL_PASTE(_brrtest_suiteT_,_suite_)
#define _brrtestname(_test_) BRRTIL_PASTE(_brrtest_testT_,_test_)
#define _brrtestfn(_name_) BRRTIL_PASTE(_brrtestname(_name_),_fn)
#define _brraddname(_suite_) BRRTIL_PASTE(BRRTIL_PASTE(_brrtest_suiteT_,_suite_),_add_tests)
#define _brrtestref(_test_) &_brrtestname(_test_)

#define BRRTEST_TEST(_test_) \
    static const char *BRRCALL _brrtestfn(_test_)(void); \
    static brrtest_testT _brrtestname(_test_) = {.name=#_test_,.test=_brrtestfn(_test_)}; \
    static const char *BRRCALL _brrtestfn(_test_)(void)
#define BRRTEST_SUITE(_suite_, _skip_on_fail_, ...) \
	static brrtest_suiteT _brrsuitename(_suite_) = {.name=#_suite_,.skip_on_fail=_skip_on_fail_}; \
	static int BRRTIL_PASTE(_brraddname(_suite_),_ran) = 0; \
	static void BRRCALL _brraddname(_suite_)(void) { \
		if (!BRRTIL_PASTE(_brraddname(_suite_),_ran)) { \
			BRRTEST_ADD_TESTS(&_brrsuitename(_suite_), BRRMAP_LIST(_brrtestref, __VA_ARGS__), NULL); \
			BRRTIL_PASTE(_brraddname(_suite_),_ran) = 1; \
		} \
	}
#define BRRTEST_RUN_SUITE(_suite_) do { _brraddname(_suite_)(); BRRTEST_RUN_SUITE(&_brrsuitename(_suite_)); } while (0)

#define BRRTEST_ASSERT(_cond_, _msg_) do { if (!(_cond_)) return _msg_; } while (0)

BRRCPPEND

#endif /* BRRTEST_H */
