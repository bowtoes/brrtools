#include "tests/lib.h"

#include <stdio.h>

#include "brrtools/brrlib.h"
#include "brrtools/brrtest.h"

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

BRRTEST_TEST(LIB_PAUSE) {
	fprintf(stderr, "Pausing...\n");
	brrlib_pause();
	return "";
}
BRRTEST_TEST(LIB_CLEAR) {
	brrlib_clear();
	fprintf(stderr, "Cleared\n");
	return "";
}
BRRTEST_TEST(LIB_UTIME) {
	brru8 u = brrlib_utime();
	brru8 s = u / 1000000, U = u % 1000000;
	brru8 m = s / 60, S = s % 60;
	brru8 h = m / 60, M = m % 60;
	brru8 d = h / 24, H = h % 24;
	fprintf(stderr, "Time: u=%llu %llu, %llu:%llu:%llu.%llu\n", u, d, H, M, S, U);
	return "";
}
BRRTEST_TEST(LIB_USLEEP) {
	fprintf(stderr, "Sleeping for 333333 usecs (1/3 sec)...\n");
	brrlib_usleep(333333);
	return "";
}
BRRTEST_TEST(LIB_NDIGITS) {
	struct testcase {
		const char *name;
		brru8 number;
		int is_signed;
		brru1 base;
		brrsz correct;

		brrsz res;
	};
#define NEWCASE(_n_, _N_, _s_, _b_, _c_) {_n_, _N_, _s_, _b_, _c_, 0}
	static const struct testcase cases[] = {
		NEWCASE("0-u-0", 0, 0, 0, 0),
		NEWCASE("0-u-1", 0, 0, 1, 0),
		NEWCASE("0-u-2", 0, 0, 2, 1),
		NEWCASE("0-u-3", 0, 0, 3, 1),
		NEWCASE("1-u-3", 1, 0, 3, 1),
		NEWCASE("1-s-3", 1, 1, 3, 1),
		NEWCASE("-1-s-3", -1, 1, 3, 1),
		NEWCASE("100-u-10", 100, 0, 10, 3),
		NEWCASE("100-s-10", 100, 1, 10, 3),
		NEWCASE("-100-s-10", -100, 1, 10, 3),
		NEWCASE("100-u-2", 100, 0, 2, 7),
		NEWCASE("100-s-2", 100, 1, 2, 7),
		NEWCASE("-100-s-2", -100, 1, 2, 7),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.res = brrlib_ndigits(cse.number, cse.is_signed, cse.base);
		BRRTEST_ASSERT(cse.res == cse.correct, MSG("case %zu '%s' failed. Expected %zu, got %zu", i, cse.name, cse.correct, cse.res));
	}

	return "";
}
BRRTEST_TEST(LIB_RAND) {
	struct testcase {
		brru8 correct;

		brru8 result;
	};
#define NEWCASE(_c_) {_c_, 0}
	static const struct testcase cases[] = {
		NEWCASE(2558736989570252433ULL),
		NEWCASE(17296837127720541114ULL),
		NEWCASE(3340261415703918136ULL),
		NEWCASE(4091938460723678821ULL),
		NEWCASE(511797725500851688ULL),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrlib_rand();
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu failed. Expected %zu, got %zu", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(LIB_SRAND) {
	struct testcase {
		brru8 seed;
		brru8 correct;

		brru8 result;
	};
#define NEWCASE(_s_, _c_) {_s_, _c_, 0}
	static const struct testcase cases[] = {
		NEWCASE(0, 13959451808893450918ULL),
		NEWCASE(1, 5747011685946229465ULL),
		NEWCASE(2, 13226621903386470572ULL),
		NEWCASE(3, 17484855840781733675ULL),
		NEWCASE(4, 1329909386138386218ULL),
		NEWCASE(0, 13959451808893450918ULL),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrlib_srand(cse.seed);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu failed. Expected %zu, got %zu", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_SUITE(lib, 0
//	,lib_pause
//	,lib_clear
	,LIB_UTIME
	,LIB_USLEEP
	,LIB_NDIGITS
	,LIB_RAND
	,LIB_SRAND
)

void BRRCALL
brrtools_brrlib_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed)
{
	BRRTEST_RUN_SUITE(lib);
	if (total) {
		*total += BRRTEST_SUITE_TOTAL(lib);
	}
	if (ran) {
		*ran += BRRTEST_SUITE_RUN(lib);
	}
	if (skipped) {
		*skipped += BRRTEST_SUITE_SKIP(lib);
	}
	if (succeeded) {
		*succeeded += BRRTEST_SUITE_SUCCEED(lib);
	}
	if (failed) {
		*failed += BRRTEST_SUITE_FAIL(lib);
	}
}
