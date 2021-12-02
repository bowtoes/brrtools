#include "brrtools/brrtest.h"
#include "brrtools/brrrand.h"

BRRTEST_NEW_TEST(RAND_RAND) {
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
	BRR_ITER_ARRAY(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrrand_rand();
		BRRTEST_ASSERT(cse.result == cse.correct,
			brrtools_test_message("case %zu failed. Expected %zu, got %zu", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_NEW_TEST(RAND_SRAND) {
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
	BRR_ITER_ARRAY(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrrand_srand(cse.seed, 0);
		BRRTEST_ASSERT(cse.result == cse.correct,
			brrtools_test_message("case %zu failed. Expected %zu, got %zu", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_NEW_SUITE(rand, brrfalse
	,RAND_RAND
	,RAND_SRAND
)
