#include "brrtools/brrtest.h"
#include "brrtools/brrmacro.h"

#include "brrtools/brrnum.h"

BRRTEST_NEW_TEST(NUM_NDIGITS) {
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
	BRR_ITER_ARRAY(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.res = brrnum_ndigits(cse.number, cse.is_signed, cse.base);
		BRRTEST_ASSERT(cse.res == cse.correct,
			brrtools_test_message("case %zu '%s' failed. Expected %zu, got %zu",
				i, cse.name, cse.correct, cse.res));
	}

	return "";
}

BRRTEST_NEW_SUITE(num, brrfalse
	,NUM_NDIGITS
)
