#include "tests/mem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrmem.h"
#include "brrtools/brrtest.h"

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

#define DUMM(...) {__VA_ARGS__}
BRRTEST_TEST(MEM_SWAP) {
	struct testcase {
		brrby data0[10];
		brrby data1[10];

		brrby correct0[10];
		brrby correct1[10];
	};
#define NEWCASE(_d0_, _d1_) {_d0_, _d1_, _d1_, _d0_}
	static const struct testcase cases[] = {
		NEWCASE(DUMM('1','2','3','4','5','6','7','8','9',0), DUMM('9','8','7','6','5','4','3','2','1',0)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		brrmem_swap(cse.data0, cse.data1, 10);
		BRRTEST_ASSERT(memcmp(cse.data0, cse.correct0, 10) == 0, MSG("case %zu data0 incorrect. Expected '%s', got '%s'", i, cse.correct0, cse.data0));
		BRRTEST_ASSERT(memcmp(cse.data1, cse.correct1, 10) == 0, MSG("case %zu data1 incorrect. Expected '%s', got '%s'", i, cse.correct1, cse.data1));
	}
	return "";
}
BRRTEST_TEST(MEM_NEXT) {
	struct testcase {
		const char *name;
		brru1 data[16];
		brrby key;
		brrsz offset;
		brrsz correct;

		brrsz result;
	};
#define NEWCASE(_name_, _data_, _key_, _ofs_, _cor_) {_name_, _data_, _key_, _ofs_, _cor_, 0}
	static const struct testcase cases[] = {
		NEWCASE("zero-l", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00,  0,  0),
		NEWCASE("zero-r", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00, 15, 15),
		NEWCASE("zero-m", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00,  5,  5),
		NEWCASE("zero-o", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00, 16, 16),

		NEWCASE("one-l",  DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  0, 16),
		NEWCASE("one-r",  DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01, 15, 16),
		NEWCASE("one-m",  DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  5, 16),
		NEWCASE("one-o",  DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01, 16, 16),

		NEWCASE("find-b", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  0,  5),
		NEWCASE("find-a", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  6, 16),
		NEWCASE("find-@", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  5,  5),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrmem_next(cse.data, 16, cse.key, cse.offset);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' had incorrect return: got %zu, expected %zu", i, cse.name, cse.result, cse.correct));
	}
	return "";
}
BRRTEST_TEST(MEM_PREVIOUS) {
	struct testcase {
		const char *name;
		brru1 data[16];
		brrby key;
		brrsz offset;
		brrsz correct;

		brrsz result;
	};
#define NEWCASE(_name_, _data_, _key_, _ofs_, _cor_) {_name_, _data_, _key_, _ofs_, _cor_, 0}
	static const struct testcase cases[] = {
		NEWCASE("zero-l", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00,  0,  0),
		NEWCASE("zero-r", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00, 15, 15),
		NEWCASE("zero-m", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00,  5,  5),
		NEWCASE("zero-o", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x00, 16, 16),

		NEWCASE("one-l", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  0, 16),
		NEWCASE("one-r", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01, 15, 16),
		NEWCASE("one-m", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  5, 16),
		NEWCASE("one-o", DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01, 16, 16),

		NEWCASE("find-b", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  0, 16),
		NEWCASE("find-a", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  6,  5),
		NEWCASE("find-@", DUMM(0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00), 0x01,  5,  5),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrmem_previous(cse.data, 16, cse.key, cse.offset);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' had incorrect return. Expected %zu, got %zu", i, cse.name, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(MEM_DUPLICATE) {
	struct testcase {
		const char *const data;
		brrsz length;

		char *result;
		int cmp;
	};
#define NEWCASE(_d_, _l_) {_d_, _l_, NULL, 0}
	static const struct testcase cases[] = {
		NEWCASE(NULL, 0),
		NEWCASE(NULL, 100),
		NEWCASE("0123456789", 0),
		NEWCASE("0123456789", 11),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrmem_duplicate(cse.data, cse.length);
		if (cse.data && cse.length)
			cse.cmp = memcmp(cse.result, cse.data, cse.length);
		else
			cse.cmp = cse.result != NULL;
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu failed. Expected '%s', got '%s'", i, BRRTIL_NULSTR(cse.data), BRRTIL_NULSTR(cse.result)));
		if (cse.result) {
			free(cse.result);
			cse.result = NULL;
		}
	}
	return "";
}
BRRTEST_TEST(MEM_COPY) {
	struct testcase {
		brrby data[16];
		brrsz length;
		brrby correct[16];

		brrby res[16];
		brrsz cmp;
	};
#define NEWCASE(_d_, _l_) {_d_, _l_, _d_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9), 10),
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9),  5),
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9),  0),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.cmp = brrmem_copy(cse.data, 10, cse.res, cse.length);
		BRRTEST_ASSERT(cse.cmp == cse.length, MSG("case %zu failed. Expected '%zu', got '%zu'", i, cse.length, cse.cmp));
		BRRTEST_ASSERT((memcmp(cse.res, cse.correct, cse.cmp) == 0), MSG("case %zu did not match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_SEARCH) {
	struct testcase {
		const char *const name;
		brrby data[24];
		brrby key[8];
		brrsz key_size;
		brrsz offset;
		brrsz correct;

		brrsz result;
	};
#define DATA DUMM( \
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
		0x01,0xEF,0x00,0xF2,0x3D,0xAA,0x90,0xFF, \
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 \
	)
#define KEY0 DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00)
#define KEY1 DUMM(0x02,0xFF,0xB2,0xF2,0x9D,0x31,0x00,0xFD)
#define KEY2 DUMM(0x01,0xEF,0x00,0xF2,0x3D,0xAA,0x90,0xFF)
#define NEWCASE(_name_, _data_, _key_, _sz_, _ofs_, _cor_) {_name_, _data_, _key_, _sz_, _ofs_, _cor_, 0}
	static const struct testcase cases[] = {
		NEWCASE("search-0-8-l", DATA, KEY0, 8,  0,  0),
		NEWCASE("search-0-8-r", DATA, KEY0, 8, 16, 16),
		NEWCASE("search-0-8-m", DATA, KEY0, 8,  5, 16),
		NEWCASE("search-0-8-o", DATA, KEY0, 8, 24, 24),

		NEWCASE("search-1-8-l", DATA, KEY1, 8,  0, 24),
		NEWCASE("search-1-8-r", DATA, KEY1, 8, 16, 24),
		NEWCASE("search-1-8-m", DATA, KEY1, 8,  5, 24),
		NEWCASE("search-1-8-o", DATA, KEY1, 8, 24, 24),

		NEWCASE("search-2-8-b", DATA, KEY2, 8,  0,  8),
		NEWCASE("search-2-8-a", DATA, KEY2, 8,  9, 24),
		NEWCASE("search-2-8-@", DATA, KEY2, 8,  8,  8),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrmem_search(cse.data, 24, cse.key, cse.key_size, cse.offset);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' had incorrect return. Expected %zu, got %zu", i, cse.name, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(MEM_SEARCH_PREVIOUS) {
	struct testcase {
		const char *const name;
		brrby data[24];
		brrby key[8];
		brrsz key_size;
		brrsz offset;
		brrsz correct;

		brrsz result;
	};
#define DATA DUMM( \
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, \
		0x01,0xEF,0x00,0xF2,0x3D,0xAA,0x90,0xFF, \
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 \
	)
#define KEY0 DUMM(0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00)
#define KEY1 DUMM(0x02,0xFF,0xB2,0xF2,0x9D,0x31,0x00,0xFD)
#define KEY2 DUMM(0x01,0xEF,0x00,0xF2,0x3D,0xAA,0x90,0xFF)
#define NEWCASE(_name_, _data_, _key_, _sz_, _ofs_, _cor_) {_name_, _data_, _key_, _sz_, _ofs_, _cor_, 0}
	static const struct testcase cases[] = {
		NEWCASE("search-0-8-l", DATA, KEY0, 8,  0,  0),
		NEWCASE("search-0-8-r", DATA, KEY0, 8, 16, 16),
		NEWCASE("search-0-8-m", DATA, KEY0, 8,  5,  0),
		NEWCASE("search-0-8-o", DATA, KEY0, 8, 24, 24),

		NEWCASE("search-1-8-l", DATA, KEY1, 8,  0, 24),
		NEWCASE("search-1-8-r", DATA, KEY1, 8, 16, 24),
		NEWCASE("search-1-8-m", DATA, KEY1, 8,  5, 24),
		NEWCASE("search-1-8-o", DATA, KEY1, 8, 24, 24),

		NEWCASE("search-2-8-b", DATA, KEY2, 8,  0, 24),
		NEWCASE("search-2-8-a", DATA, KEY2, 8,  9,  8),
		NEWCASE("search-2-8-@", DATA, KEY2, 8,  8,  8),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrmem_search_reverse(cse.data, 24, cse.key, cse.key_size, cse.offset);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' had incorrect return. Expected %zu, got %zu", i, cse.name, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(MEM_STATIC_REVERSE) {
	struct testcase {
		brrby data[10];
		brrby correct[10];
	};
#define NEWCASE(_d_, _c_) {_d_, _c_}
	struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9), DUMM(9,8,7,6,5,4,3,2,1,0)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		brrmem_static_reverse(cse.data, 10);
		BRRTEST_ASSERT(memcmp(cse.data, cse.correct, 10) == 0, MSG("case %zu did not match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_REVERSE) {
	struct testcase {
		brrby data[10];
		brrby correct[10];

		brrby *res;
		int cmp;
	};
#define NEWCASE(_d_, _c_) {_d_, _c_, NULL, 0}
	struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9), DUMM(9,8,7,6,5,4,3,2,1,0)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.res = brrmem_reverse(cse.data, 10);
		cse.cmp = memcmp(cse.res, cse.correct, 10);
		if (cse.res) {
			free(cse.res);
			cse.res = NULL;
		}
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu did not match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_STATIC_REVERSE_BLOCKS) {
	struct testcase {
		brrby data[12];
		brrby correct[12];
	};
#define NEWCASE(_d_, _c_) {_d_, _c_}
	struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9,10,11), DUMM(9,10,11,6,7,8,3,4,5,0,1,2)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		brrmem_static_reverse_blocks(cse.data, 3, 4);
		BRRTEST_ASSERT(memcmp(cse.data, cse.correct, 10) == 0, MSG("case %zu did not match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_REVERSE_BLOCKS) {
	struct testcase {
		brrby data[12];
		brrby correct[12];

		brrby *res;
		int cmp;
	};
#define NEWCASE(_d_, _c_) {_d_, _c_, NULL, 0}
	struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9,10,11), DUMM(9,10,11,6,7,8,3,4,5,0,1,2)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.res = brrmem_reverse_blocks(cse.data, 3, 4);
		cse.cmp = memcmp(cse.res, cse.correct, 10);
		if (cse.res) {
			free(cse.res);
			cse.res = NULL;
		}
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu did not match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_JOIN) {
	struct testcase {
		brrby data0[10];
		brrby data1[6];
		brrsz res_size;
		brrby correct[16];

		brrby *res;
		int cmp;
	};
#define NEWCASE(_d0_, _d1_, _c_) {_d0_, _d1_, 0, _c_, NULL, 0}
	struct testcase cases[] = {
		NEWCASE(DUMM(0,1,2,3,4,5,6,7,8,9), DUMM(0,1,2,3,4,5), DUMM(0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.res = brrmem_join(cse.data0, 10, cse.data1, 6, &cse.res_size);
		cse.cmp = memcmp(cse.res, cse.correct, 16);
		if (cse.res) {
			free(cse.res);
			cse.res = NULL;
		}
		BRRTEST_ASSERT(cse.res_size == 16, MSG("case %zu had incorrect size. Expected %zu, got %zu", i, (brrsz)16, cse.res_size));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu didn't match", i));
	}
	return "";
}
BRRTEST_TEST(MEM_SLICE) {
	struct testcase {
		const char *const name;
		brrby data[10];
		brrby result[10];
		brrsz start;
		brrsz end;

		brrsz slice_len;
		brrsz correct_len;
		brrby correct[10];
	};
#define NEWCASE(_n_, _d_, _s_, _e_, _cl_, _c_) {_n_, _d_, {0}, _s_, _e_, 0, _cl_, _c_}
	struct testcase cases[] = {
		NEWCASE("zero-slice",      DUMM(0,1,2,3,4,5,6,7,8,9), 0,  0, 0, DUMM(0)),
		NEWCASE("same-slice",      DUMM(0,1,2,3,4,5,6,7,8,9), 3,  3, 0, DUMM(0)),
		NEWCASE("good-slice",      DUMM(0,1,2,3,4,5,6,7,8,9), 3,  7, 4, DUMM(3,4,5,6)),
		NEWCASE("one-slice",       DUMM(0,1,2,3,4,5,6,7,8,9), 0,  1, 1, DUMM(0)),
		NEWCASE("over-slice",      DUMM(0,1,2,3,4,5,6,7,8,9), 3, 11, 7, DUMM(3,4,5,6,7,8,9)),
		NEWCASE("back-slice",      DUMM(0,1,2,3,4,5,6,7,8,9), 7,  3, 4, DUMM(6,5,4,3)),
		NEWCASE("back-one-slice",  DUMM(0,1,2,3,4,5,6,7,8,9), 1,  0, 1, DUMM(0)),
		NEWCASE("over-back-slice", DUMM(0,1,2,3,4,5,6,7,8,9), 11,  3, 7, DUMM(9,8,7,6,5,4,3)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.slice_len = brrmem_slice(cse.data, 10, cse.result, cse.start, cse.end);
		BRRTEST_ASSERT(cse.slice_len == cse.correct_len, MSG("case '%s' %zu incorrect slice length. Expected %zu, got %zu", cse.name, i, cse.correct_len, cse.slice_len));
		BRRTEST_ASSERT(memcmp(cse.result, cse.correct, cse.slice_len) == 0, MSG("case '%s' %zu incorrect slice", cse.name, i));
	}
	return "";
}
BRRTEST_SUITE(mem, 0
	,MEM_SWAP
	,MEM_NEXT
	,MEM_PREVIOUS
	,MEM_SEARCH
	,MEM_SEARCH_PREVIOUS
	,MEM_DUPLICATE
	,MEM_COPY
	,MEM_STATIC_REVERSE
	,MEM_REVERSE
	,MEM_STATIC_REVERSE_BLOCKS
	,MEM_REVERSE_BLOCKS
	,MEM_JOIN
	,MEM_SLICE
)

void BRRCALL
brrtools_brrmem_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed)
{
	BRRTEST_RUN_SUITE(mem);
	if (total) {
		*total += BRRTEST_SUITE_TOTAL(mem);
	}
	if (ran) {
		*ran += BRRTEST_SUITE_RUN(mem);
	}
	if (skipped) {
		*skipped += BRRTEST_SUITE_SKIP(mem);
	}
	if (succeeded) {
		*succeeded += BRRTEST_SUITE_SUCCEED(mem);
	}
	if (failed) {
		*failed += BRRTEST_SUITE_FAIL(mem);
	}
}
