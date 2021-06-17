#include "brrtools/brrapi.h"
#include "brrtools/brrstr.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <brrtools/brrplatform.h>
#include <brrtools/brrtest.h>
#include <brrtools/brrlib.h>
#include <brrtools/brrlog.h>
#include <brrtools/brrmem.h>

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

BRRTEST_TEST(LOG_INIT) {
	BRRTEST_ASSERT(brrlog_init(), MSG("Failed to initialize log : %s", strerror(errno)));
	return "";
}
BRRTEST_TEST(LOG_MESSAGE) {
	static const char fmt[] = "%s wow\n";
	static const char msg[] = "wow";
	static const brrsz len = sizeof(msg) + sizeof(fmt) - 2 - 2;
	static brrsz logged = 0;
	brrlogctl.style_enabled = 0;
	BRRTEST_ASSERT(brrlog_init(), "could not init log");
	BRRTEST_ASSERT(len == (logged = brrlog_text(-1, -1, NULL, -1, -1, -1, -1, NULL, 0, 0, __FILE__, __func__, 0, fmt, msg)), MSG("logged incorrect amount of bytes: %zu != %zu", len, logged));
	return "";
}
BRRTEST_TEST(LOG_PRIORITIES) {
	static const char fmt[] = "wow nice styles dude\n";
	brrlogctl.style_enabled = 1;
	BRRTEST_ASSERT(brrlog_init(), "could not init log");
	brrlog_setminpriority(brrlog_priority_none);
	brrlog_setmaxpriority(brrlog_priority_count);
	brrlogctl.debug_enabled = 1;
	BRRTEST_ASSERT(0 != BRRLOG_CRIN(fmt), "did not log critical with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_ERRN(fmt), "did not log error with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_NORN(fmt), "did not log normal with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_WARN(fmt), "did not log warning with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_DEBN(fmt), "did not log debug while enabled");
	brrlog_setminpriority(brrlog_priority_error);
	brrlog_setmaxpriority(brrlog_priority_normal);
	brrlogctl.debug_enabled = 0;
	BRRTEST_ASSERT(0 == BRRLOG_CRIN(fmt), "logged critical with max priority at normal, min at error");
	BRRTEST_ASSERT(0 != BRRLOG_ERRN(fmt), "did not log error with max priority at normal, min at error");
	BRRTEST_ASSERT(0 != BRRLOG_NORN(fmt), "did not log normal with max priority at normal, min at error");
	BRRTEST_ASSERT(0 == BRRLOG_WARN(fmt), "logged warning with max priority at normal, min at error");
	BRRTEST_ASSERT(0 == BRRLOG_DEBN(fmt), "logged debug while disabled");
	return "";
}
BRRTEST_TEST(LOG_DIGITS) {
	struct testcase {
		brru8 number;
		int base;
		int is_signed;
		char separator;
		brrsz spacing;
		char correct[129];

		char result[129];
		brru8 correct_length;
		brru8 res_length;
		brru8 res_print;
		brru1 cmp;
	};
#define NEWCASE(_num_, _bse_, _sgn_, _sep_, _spc_, _cor_) {_num_, _bse_, _sgn_, _sep_, _spc_, _cor_, {0}, sizeof(_cor_) - 1, 0, 0, 0}
	static const struct testcase cases[] = {
		/* TODO what are the edgecases? */
		NEWCASE( 99194853094755497, 10, 0,   0, -1, "99194853094755497"),
		NEWCASE( 99194853094755497,  2, 0,   0, -1, "101100000011010010011000101111110010000101000110010101001"),
		NEWCASE( 99194853094755497, 16, 0,   0, -1, "16069317E428CA9"),
		NEWCASE(-99194853094755497, 10, 1,   0, -1, "-99194853094755497"),
		NEWCASE(-99194853094755497,  2, 1,   0, -1, "-101100000011010010011000101111110010000101000110010101001"),
		NEWCASE(-99194853094755497, 16, 1,   0, -1, "-16069317E428CA9"),
		NEWCASE( 99194853094755497, 10, 0, ',',  3, "99,194,853,094,755,497"),
		NEWCASE( 99194853094755497,  2, 0, ':',  4, "1:0110:0000:0110:1001:0011:0001:0111:1110:0100:0010:1000:1100:1010:1001"),
		NEWCASE( 99194853094755497, 16, 0, '-',  2, "1-60-69-31-7E-42-8C-A9"),
		NEWCASE(-99194853094755497, 10, 1, ',',  3, "-99,194,853,094,755,497"),
		NEWCASE(-99194853094755497,  2, 1, ':',  4, "-1:0110:0000:0110:1001:0011:0001:0111:1110:0100:0010:1000:1100:1010:1001"),
		NEWCASE(-99194853094755497, 16, 1, '-',  2, "-1-60-69-31-7E-42-8C-A9"),
	};
#undef NEWCASE
	brrlogctl.style_enabled = 0;
	for (brrsz i = 0; i < (BRRTIL_ARRLEN(cases)); ++i) {
		struct testcase cse = cases[i];
		cse.res_print = BRRLOGD_NORBNP(cse.result, cse.number, cse.base, cse.is_signed, cse.separator, cse.spacing);
		cse.res_length = strlen(cse.result);
		BRRTEST_ASSERT(cse.res_print == cse.res_length, MSG("case %zu bytes printed does not match measured length : %zu != %zu ('%s' , expected '%s')", i, cse.res_print, cse.res_length, cse.result, cse.correct));
		BRRTEST_ASSERT(cse.res_length == cse.correct_length, MSG("case %zu res length does not match correct length : %zu != %zu ('%s' , expected '%s')", i, cse.res_length, cse.correct_length, cse.result, cse.correct));
		cse.cmp = strcmp(cse.correct, cse.result);
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu string does not match expected : '%s' != '%s'", i, cse.result, cse.correct));
	}

	return "";
}
BRRTEST_TEST(LOG_BITS) {
	struct testcase {
		brru1 data[8];
		brru1 reverse_bytes;
		brru1 reverse_bits;
		brru8 bits_to_print;
		char separator;
		brru8 spacing;
		char correct[128];

		char result[128];
		brru8 correct_length;
		brru8 res_length;
		brru8 res_print;
		brru1 cmp;
	};
#define NEWCASE(_data_, _rvsB_, _rvsb_, _nbits_, _sep_, _spc_, _cor_) {_data_, _rvsB_, _rvsb_, _nbits_, _sep_, _spc_, _cor_, {0}, sizeof(_cor_)-1, 0, 0, 0}
#define DUMM(...) {__VA_ARGS__}
	static const struct testcase cases[] = {
		NEWCASE(DUMM(0xA0,0,0x05), 0, 0, 64,   0, 0, "1010000000000000000001010000000000000000000000000000000000000000"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 1, 64,   0, 0, "0000010100000000101000000000000000000000000000000000000000000000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 0, 64,   0, 0, "0000000000000000000000000000000000000000000001010000000010100000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 1, 64,   0, 0, "0000000000000000000000000000000000000000101000000000000000000101"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 0, 59,   0, 0, "10100000000000000000010100000000000000000000000000000000000"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 1, 59,   0, 0, "00000101000000001010000000000000000000000000000000000000000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 0, 59,   0, 0, "00000000000000000000000000000000000000001010000000010100000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 1, 59,   0, 0, "00000000000000000000000000000000000101000000000000000000101"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 0, 64, ':', 3, "1:010:000:000:000:000:000:001:010:000:000:000:000:000:000:000:000:000:000:000:000:000"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 1, 64, ':', 3, "0:000:010:100:000:000:101:000:000:000:000:000:000:000:000:000:000:000:000:000:000:000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 0, 64, ':', 3, "0:000:000:000:000:000:000:000:000:000:000:000:000:000:000:001:010:000:000:010:100:000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 1, 64, ':', 3, "0:000:000:000:000:000:000:000:000:000:000:000:000:000:101:000:000:000:000:000:000:101"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 0, 59, '-', 3, "10-100-000-000-000-000-000-010-100-000-000-000-000-000-000-000-000-000-000-000"),
		NEWCASE(DUMM(0xA0,0,0x05), 0, 1, 59, '-', 3, "00-000-101-000-000-001-010-000-000-000-000-000-000-000-000-000-000-000-000-000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 0, 59, '-', 3, "00-000-000-000-000-000-000-000-000-000-000-000-000-001-010-000-000-010-100-000"),
		NEWCASE(DUMM(0xA0,0,0x05), 1, 1, 59, '-', 3, "00-000-000-000-000-000-000-000-000-000-000-000-101-000-000-000-000-000-000-101"),
	};
#undef NEWCASE
	brrlogctl.style_enabled = 0;
	for (brrsz i = 0; i < (BRRTIL_ARRLEN(cases)); ++i) {
		struct testcase cse = cases[i];
		cse.res_print = BRRLOGB_NORBNP(cse.result, cse.data, cse.bits_to_print, cse.reverse_bytes, cse.reverse_bits, cse.separator, cse.spacing);
		cse.res_length = strlen(cse.result);
		BRRTEST_ASSERT(cse.res_print == cse.res_length, MSG("case %zu bytes printed does not match measured length : %zu != %zu ('%s' , expected '%s')", i, cse.res_print, cse.res_length, cse.result, cse.correct));
		BRRTEST_ASSERT(cse.res_length == cse.correct_length, MSG("case %zu res length does not match correct length : %zu != %zu ('%s' , expected '%s')", i, cse.res_length, cse.correct_length, cse.result, cse.correct));
		cse.cmp = strcmp(cse.correct, cse.result);
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu string does not match expected : '%s' != '%s'", i, cse.result, cse.correct));
	}
	return "";
}
BRRTEST_TEST(LOG_DEINIT) {
	brrlog_deinit();
	return "";
}
BRRTEST_TEST(LOG_SETLOGMAX) {
	static const brrsz cases[] = {
		4096, 2048, 1024, 512, 0,
	};
	BRRTIL_ITERARR(brrsz, i, cases) {
		BRRTEST_ASSERT(brrlog_setlogmax(cases[i]), MSG("Failed to set max to %zu : %s", cases[i], strerror(errno)));
	}
	return "";
}
BRRTEST_SUITE(log, 0
	,LOG_INIT
	,LOG_MESSAGE
	,LOG_PRIORITIES
	,LOG_DIGITS
	,LOG_BITS
	,LOG_DEINIT
	,LOG_SETLOGMAX
)

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
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu failed. Expected '%s', got '%s'", i, BRRTIL_SSTR(cse.data), BRRTIL_SSTR(cse.result)));
		brrlib_alloc((void **)&cse.result, 0, 0);
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
		brrlib_alloc((void **)&cse.res, 0, 0);
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
		brrlib_alloc((void **)&cse.res, 0, 0);
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
		brrlib_alloc((void **)&cse.res, 0, 0);
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

BRRTEST_TEST(LIB_PAUSE) {
	BRRLOG_NOR("Pausing...");
	brrlib_pause();
	return "";
}
BRRTEST_TEST(LIB_CLEAR) {
	brrlib_clear();
	BRRLOG_NOR("Cleared");
	return "";
}
BRRTEST_TEST(LIB_UTIME) {
	brru8 u = brrlib_utime();
	brru8 s = u / 1000000, U = u % 1000000;
	brru8 m = s / 60, S = s % 60;
	brru8 h = m / 60, M = m % 60;
	brru8 d = h / 24, H = h % 24;
	BRRLOG_NOR("Time: u=%llu %llu, %llu:%llu:%llu.%llu", u, d, H, M, S, U);
	return "";
}
BRRTEST_TEST(LIB_USLEEP) {
	BRRLOG_NOR("Sleeping for 333333 usecs (1/3 sec)...");
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

BRRTEST_TEST(STR_STRLEN) {
	struct testcase {
		const char *const str;
		brrsz max;
		brrsz correct;

		brrsz result;
	};
#define NEWCASE(_s_, _m_, _c_) {_s_, _m_, _c_, 0}
	static const struct testcase cases[] = {
		NEWCASE(NULL,0,0),
		NEWCASE("",0,0),
		NEWCASE("",10,0),
		NEWCASE("0123456789",0,0),
		NEWCASE("0123456789",1,1),
		NEWCASE("0123456789",10,10),
		NEWCASE("0123456789",100,10),
		NEWCASE("0123456789",-1,10),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_strlen(cse.str, cse.max);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' incorrect length. Expected %zu, got %zu", i, BRRTIL_SSTR(cse.str), cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(STR_NEW) {
	struct testcase {
		const char *const str;
		brrsz max;
		brrstrT correct;

		brrstrT result;
		int cmp;
	};
#define NEWCASE(_s_, _m_, _c_) {_s_, _m_, _c_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(NULL,           0, DUMM("", 0)),
		NEWCASE("",             0, DUMM("", 0)),
		NEWCASE("",            10, DUMM("", 0)),
		NEWCASE("0123456789",   0, DUMM("", 0)),
		NEWCASE("0123456789",   1, DUMM("0", 1)),
		NEWCASE("0123456789",  10, DUMM("0123456789", 10)),
		NEWCASE("0123456789", 100, DUMM("0123456789", 10)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_new(cse.str, cse.max);
		cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu '%s':%zu had incorrect length. Expected %zu, got %zu", i, BRRTIL_SSTR(cse.str), cse.max, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu '%s':%zu had incorrect cstr. Expected '%s'", i, BRRTIL_SSTR(cse.str), cse.max, BRRTIL_SSTR(cse.correct.cstr)));
	}
	return "";
}
BRRTEST_TEST(STR_DELETE) {
	struct testcase {
		const char *const str;
		brrsz max;

		brrstrT result;
	};
#define NEWCASE(_s_, _m_) {_s_, _m_, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL,           0),
		NEWCASE("",             0),
		NEWCASE("",            10),
		NEWCASE("0123456789",   0),
		NEWCASE("0123456789",   1),
		NEWCASE("0123456789",  10),
		NEWCASE("0123456789", 100),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_new(cse.str, cse.max);
		brrstr_delete(&cse.result);
		BRRTEST_ASSERT(cse.result.cstr == NULL, MSG("case %zu '%s' had incorrect cstr. Expected NULL, got '%s'", i, BRRTIL_SSTR(cse.str), BRRTIL_SSTR(cse.result.cstr)));
		BRRTEST_ASSERT(cse.result.length == 0,  MSG("case %zu '%s' had incorrect length. Expected 0, got %zu", i, BRRTIL_SSTR(cse.str), cse.result.length));
	}
	return "";
}
BRRTEST_TEST(STR_DUPLICATE) {
	struct testcase {
		brrstrT correct;

		brrstrT result;
		int cmp;
	};
#define NEWCASE(_c_) {_c_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(DUMM(NULL, 0)),
		NEWCASE(DUMM("", 0)),
		NEWCASE(DUMM("0", 1)),
		NEWCASE(DUMM("0123456789", 10)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_duplicate(cse.correct);
		if (cse.correct.cstr)
			cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		else
			cse.cmp = cse.result.cstr != NULL;
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu had incorrect length. Expected %zu, got %zu", i, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu had incorrect cstr. Expected '%s'", i, BRRTIL_SSTR(cse.correct.cstr)));
	}
	return "";
}
BRRTEST_TEST(STR_COPY) {
	struct testcase {
		brrstrT correct;

		brrstrT result;
		int cmp;
	};
#define NEWCASE(_c_) {_c_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(DUMM(NULL, 0)),
		NEWCASE(DUMM("", 0)),
		NEWCASE(DUMM("0", 1)),
		NEWCASE(DUMM("0123456789", 10)),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		brrstr_copy(cse.correct, &cse.result);
		if (cse.correct.cstr)
			cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		else
			cse.cmp = cse.result.cstr == NULL;
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu had incorrect length. Expected %zu, got %zu", i, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu had incorrect cstr. Expected '%s'", i, BRRTIL_SSTR(cse.correct.cstr)));
	}
	return "";
}
BRRTEST_TEST(STR_RESIZE) {
	struct testcase {
		const char *const str;
		brrsz length;
		brrsz new_length;
		brrstrT correct;

		brrstrT result;
		int cmp;
	};
#define NEWCASE(_s_, _nl_, _c_) {_s_, sizeof(_s_) - 1, _nl_, {_c_, sizeof(_c_)-1}, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE("",            0, ""),
		NEWCASE("",            5, "     "),
		NEWCASE("0",           5, "0    "),
		NEWCASE("0123456789",  5, "01234"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_new(cse.str, cse.length);
		brrstr_resize(&cse.result, cse.new_length);
		cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu had incorrect length. Expected %zu, got %zu", i, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu had incorrect cstr. Expected '%s'", i, BRRTIL_SSTR(cse.correct.cstr)));
	}
	return "";
}
BRRTEST_TEST(STR_STRIP) {
	struct testcase {
		brrstrT str;
		brrstrT correct;
		brrstrT result;
		int cmp;
	};
#define NEWCASE(_s_, _c_) {{_s_, sizeof(_s_)-1}, {_c_, sizeof(_c_)-1}, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE("", ""),
		NEWCASE("     ", ""),
		NEWCASE("     a", "a"),
		NEWCASE("b     ", "b"),
		NEWCASE("     c     ", "c"),
		NEWCASE("  a  b  c  ", "a  b  c"),
	};
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_duplicate(cse.str);
		brrstr_strip(&cse.result);
		cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu invalid length. Expected %zu, got %zu", i, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu invalid cstr. Expected '%s'", i, cse.correct.cstr));
	}
#undef NEWCASE
	return "";
}
BRRTEST_TEST(STR_TRIM) {
	struct testcase {
		brrstrT str;
		brrstrT correct;
		brrstrT result;
		int cmp;
	};
#define NEWCASE(_s_, _c_) {{_s_, sizeof(_s_)-1}, {_c_, sizeof(_c_)-1}, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE("", ""),
		NEWCASE("     ", ""),
		NEWCASE("     a", "a"),
		NEWCASE("b     ", "b"),
		NEWCASE("     c     ", "c"),
		NEWCASE("  a  b  c  ", "a  b  c"),
	};
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_trim(cse.str);
		cse.cmp = strcmp(cse.result.cstr, cse.correct.cstr);
		brrlib_alloc((void **)&cse.result.cstr, 0, 0);
		BRRTEST_ASSERT(cse.result.length == cse.correct.length, MSG("case %zu invalid length. Expected %zu, got %zu", i, cse.correct.length, cse.result.length));
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu invalid cstr. Expected '%s'", i, cse.correct.cstr));
	}
#undef NEWCASE
	return "";
}
BRRTEST_TEST(STR_STR_COMPARE) {
	struct testcase {
		const char *const compare;
		int cse;
		int correct;
		int result;
		const char *const strs[11];
	};
#define NEWCASE(_s_, _c_, _C_, ...) {_s_, _c_, _C_, 0, {__VA_ARGS__}}
	static const struct testcase cases[] = {
		NEWCASE(NULL, 0,  0, NULL),
		NEWCASE(NULL, 1,  0, NULL),
		NEWCASE(NULL, 0,  1, "A", NULL),
		NEWCASE(NULL, 1,  1, "a", NULL),
		NEWCASE(NULL, 0,  2, "A", "a", NULL),
		NEWCASE(NULL, 1,  2, "a", "A", NULL),
		NEWCASE("A",  0, -1, NULL),
		NEWCASE("A",  1, -1, NULL),
		NEWCASE("A",  0,  0, "A", NULL),
		NEWCASE("A",  1,  0, "A", NULL),
		NEWCASE("A",  0, -1, "a", NULL),
		NEWCASE("A",  1,  0, "a", NULL),
		NEWCASE("A",  0,  1, "b", "A", NULL),
		NEWCASE("A",  1,  1, "b", "A", NULL),
		NEWCASE("A",  0, -1, "b", "a", NULL),
		NEWCASE("A",  1,  1, "b", "a", NULL),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstr_astr_compare(cse.compare, cse.cse, cse.strs);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu '%s' failed. Expected %i, got %i", i, BRRTIL_SSTR(cse.compare), cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(STR_STARTS_WITH) {
	struct testcase {
		const char *const str;
		const char *const prefix;
		int case_insensitive;
		int correct;
		brrstrT st;
		int result;
	};
#define NEWCASE(_s_, _p_, _i_, _c_) {_s_, _p_, _i_, _c_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(NULL, NULL, 0, 0),
		NEWCASE("",   NULL, 0, 0),
		NEWCASE("",   "",   0, 0),
		NEWCASE("ALPHABETA", "alpha", 0, 0),
		NEWCASE("ALPHABETA", "ALPHA", 0, 1),
		NEWCASE("ALPHABETA", "alpha", 1, 1),
		NEWCASE("ALPHABETA", "ALPHA", 1, 1),
		NEWCASE("ALPHABETA", "beta", 0, 0),
		NEWCASE("ALPHABETA", "BETA", 0, 0),
		NEWCASE("ALPHABETA", "beta", 1, 0),
		NEWCASE("ALPHABETA", "BETA", 1, 0),
		NEWCASE("ALPHABETA", "ALPHABETAGAMMA",  0, 0),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.st = brrstr_new(cse.str, -1);
		cse.result = brrstr_starts_with(cse.st, cse.prefix, cse.case_insensitive);
		brrstr_delete(&cse.st);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu failed. Expected %i, got %i", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(STR_ENDS_WITH) {
	struct testcase {
		const char *const str;
		const char *const suffix;
		int case_insensitive;
		int correct;
		brrstrT st;
		int result;
	};
#define NEWCASE(_s_, _S_, _i_, _c_) {_s_, _S_, _i_, _c_, {0}, 0}
	static const struct testcase cases[] = {
		NEWCASE(NULL, NULL, 0, 0),
		NEWCASE("",   NULL, 0, 0),
		NEWCASE("",   "",   0, 0),
		NEWCASE("ALPHABETA", "beta", 0, 0),
		NEWCASE("ALPHABETA", "BETA", 0, 1),
		NEWCASE("ALPHABETA", "beta", 1, 1),
		NEWCASE("ALPHABETA", "BETA", 1, 1),
		NEWCASE("ALPHABETA", "alpha", 0, 0),
		NEWCASE("ALPHABETA", "ALPHA", 0, 0),
		NEWCASE("ALPHABETA", "alpha", 1, 0),
		NEWCASE("ALPHABETA", "ALPHA", 1, 0),
		NEWCASE("ALPHABETA", "ALPHABETAGAMMA",  0, 0),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.st = brrstr_new(cse.str, -1);
		cse.result = brrstr_ends_with(cse.st, cse.suffix, cse.case_insensitive);
		brrstr_delete(&cse.st);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu failed. Expected %i, got %i", i, cse.correct, cse.result));
	}
	return "";
}
BRRTEST_TEST(STR_SPLIT) {
	struct testcase {
		const char *const name;
		const brrstrT str;
		char sep;
		int keep_separator;
		int include_consecutive;
		brrsz offset;
		brrsz max_split_size;
		brrsz count;
		const brrstrT *const correct;
		brrsz correct_count;
		brrstrT *result;
	};
/* Top-level trickery, right here */
#define STATICSTR(_s_) {_s_, sizeof(_s_) - 1}
#define NEWCASE(_nme_, _max_, _sep_, _kps_, _cns_, _ofs_, _str_, ...) \
    {#_nme_, {_str_, sizeof(_str_) - 1}, _sep_, _kps_, _cns_, _ofs_, _max_, 0, \
    (const brrstrT[]){BRRMAP_LIST(STATICSTR, __VA_ARGS__)}, BRRTIL_ARRLEN(((const char *[]){__VA_ARGS__})), NULL}
	const struct testcase cases[] = {
		NEWCASE(00, -1, '.', 0, 0, 0, ""        ,  ""),
		NEWCASE(01, -1, '.', 0, 0, 0, "."       ,  ""),
		NEWCASE(02, -1, '.', 0, 0, 0, ".."      ,  ""),
		NEWCASE(03, -1, '.', 0, 0, 0, "a"       , "a"),
		NEWCASE(04, -1, '.', 0, 0, 0, "a."      , "a"),
		NEWCASE(05, -1, '.', 0, 0, 0, "a.."     , "a"),
		NEWCASE(06, -1, '.', 0, 0, 0, "a.b"     , "a", "b"),
		NEWCASE(07, -1, '.', 0, 0, 0, "a.b."    , "a", "b"),
		NEWCASE(08, -1, '.', 0, 0, 0, "a.b.."   , "a", "b"),
		NEWCASE(09, -1, '.', 0, 0, 0, "a..b"    , "a", "b"),
		NEWCASE(0A, -1, '.', 0, 0, 0, "a..b."   , "a", "b"),
		NEWCASE(0B, -1, '.', 0, 0, 0, "a..b.."  , "a", "b"),
		NEWCASE(0C, -1, '.', 0, 0, 0, ".a"      , "a"),
		NEWCASE(0D, -1, '.', 0, 0, 0, ".a."     , "a"),
		NEWCASE(0E, -1, '.', 0, 0, 0, ".a.."    , "a"),
		NEWCASE(0F, -1, '.', 0, 0, 0, ".a.b"    , "a", "b"),
		NEWCASE(0G, -1, '.', 0, 0, 0, ".a.b."   , "a", "b"),
		NEWCASE(0H, -1, '.', 0, 0, 0, ".a.b.."  , "a", "b"),
		NEWCASE(0J, -1, '.', 0, 0, 0, ".a..b"   , "a", "b"),
		NEWCASE(0K, -1, '.', 0, 0, 0, ".a..b."  , "a", "b"),
		NEWCASE(0L, -1, '.', 0, 0, 0, ".a..b.." , "a", "b"),
		NEWCASE(0M, -1, '.', 0, 0, 0, "..a"     , "a"),
		NEWCASE(0N, -1, '.', 0, 0, 0, "..a."    , "a"),
		NEWCASE(0O, -1, '.', 0, 0, 0, "..a.."   , "a"),
		NEWCASE(0P, -1, '.', 0, 0, 0, "..a.b"   , "a", "b"),
		NEWCASE(0Q, -1, '.', 0, 0, 0, "..a.b."  , "a", "b"),
		NEWCASE(0R, -1, '.', 0, 0, 0, "..a.b.." , "a", "b"),
		NEWCASE(0S, -1, '.', 0, 0, 0, "..a..b"  , "a", "b"),
		NEWCASE(0T, -1, '.', 0, 0, 0, "..a..b." , "a", "b"),
		NEWCASE(0U, -1, '.', 0, 0, 0, "..a..b..", "a", "b"),

		NEWCASE(10,  0, '.', 0, 0, 0, ""        , ""),
		NEWCASE(11,  0, '.', 0, 0, 0, "."       , ""),
		NEWCASE(12,  0, '.', 0, 0, 0, ".."      , ""),
		NEWCASE(13,  0, '.', 0, 0, 0, "a"       , ""),
		NEWCASE(14,  0, '.', 0, 0, 0, "a."      , ""),
		NEWCASE(15,  0, '.', 0, 0, 0, "a.."     , ""),
		NEWCASE(16,  0, '.', 0, 0, 0, "a.b"     , "", ""),
		NEWCASE(17,  0, '.', 0, 0, 0, "a.b."    , "", ""),
		NEWCASE(18,  0, '.', 0, 0, 0, "a.b.."   , "", ""),
		NEWCASE(19,  0, '.', 0, 0, 0, "a..b"    , "", ""),
		NEWCASE(1A,  0, '.', 0, 0, 0, "a..b."   , "", ""),
		NEWCASE(1B,  0, '.', 0, 0, 0, "a..b.."  , "", ""),
		NEWCASE(1C,  0, '.', 0, 0, 0, ".a"      , ""),
		NEWCASE(1D,  0, '.', 0, 0, 0, ".a."     , ""),
		NEWCASE(1E,  0, '.', 0, 0, 0, ".a.."    , ""),
		NEWCASE(1F,  0, '.', 0, 0, 0, ".a.b"    , "", ""),
		NEWCASE(1G,  0, '.', 0, 0, 0, ".a.b."   , "", ""),
		NEWCASE(1H,  0, '.', 0, 0, 0, ".a.b.."  , "", ""),
		NEWCASE(1J,  0, '.', 0, 0, 0, ".a..b"   , "", ""),
		NEWCASE(1K,  0, '.', 0, 0, 0, ".a..b."  , "", ""),
		NEWCASE(1L,  0, '.', 0, 0, 0, ".a..b.." , "", ""),
		NEWCASE(1M,  0, '.', 0, 0, 0, "..a"     , ""),
		NEWCASE(1N,  0, '.', 0, 0, 0, "..a."    , ""),
		NEWCASE(1O,  0, '.', 0, 0, 0, "..a.."   , ""),
		NEWCASE(1P,  0, '.', 0, 0, 0, "..a.b"   , "", ""),
		NEWCASE(1Q,  0, '.', 0, 0, 0, "..a.b."  , "", ""),
		NEWCASE(1R,  0, '.', 0, 0, 0, "..a.b.." , "", ""),
		NEWCASE(1S,  0, '.', 0, 0, 0, "..a..b"  , "", ""),
		NEWCASE(1T,  0, '.', 0, 0, 0, "..a..b." , "", ""),
		NEWCASE(1U,  0, '.', 0, 0, 0, "..a..b..", "", ""),

		NEWCASE(20,  1, '.', 0, 0, 0, ""          ,  ""),
		NEWCASE(21,  1, '.', 0, 0, 0, "."         ,  ""),
		NEWCASE(22,  1, '.', 0, 0, 0, ".."        ,  ""),
		NEWCASE(23,  1, '.', 0, 0, 0, "aa"        , "a"),
		NEWCASE(24,  1, '.', 0, 0, 0, "aa."       , "a"),
		NEWCASE(25,  1, '.', 0, 0, 0, "aa.."      , "a"),
		NEWCASE(26,  1, '.', 0, 0, 0, "aa.bb"     , "a", "b"),
		NEWCASE(27,  1, '.', 0, 0, 0, "aa.bb."    , "a", "b"),
		NEWCASE(28,  1, '.', 0, 0, 0, "aa.bb.."   , "a", "b"),
		NEWCASE(29,  1, '.', 0, 0, 0, "aa..bb"    , "a", "b"),
		NEWCASE(2A,  1, '.', 0, 0, 0, "aa..bb."   , "a", "b"),
		NEWCASE(2B,  1, '.', 0, 0, 0, "aa..bb.."  , "a", "b"),
		NEWCASE(2C,  1, '.', 0, 0, 0, ".aa"       , "a"),
		NEWCASE(2D,  1, '.', 0, 0, 0, ".aa."      , "a"),
		NEWCASE(2E,  1, '.', 0, 0, 0, ".aa.."     , "a"),
		NEWCASE(2F,  1, '.', 0, 0, 0, ".aa.bb"    , "a", "b"),
		NEWCASE(2G,  1, '.', 0, 0, 0, ".aa.bb."   , "a", "b"),
		NEWCASE(2H,  1, '.', 0, 0, 0, ".aa.bb.."  , "a", "b"),
		NEWCASE(2J,  1, '.', 0, 0, 0, ".aa..bb"   , "a", "b"),
		NEWCASE(2K,  1, '.', 0, 0, 0, ".aa..bb."  , "a", "b"),
		NEWCASE(2L,  1, '.', 0, 0, 0, ".aa..bb.." , "a", "b"),
		NEWCASE(2M,  1, '.', 0, 0, 0, "..aa"      , "a"),
		NEWCASE(2N,  1, '.', 0, 0, 0, "..aa."     , "a"),
		NEWCASE(2O,  1, '.', 0, 0, 0, "..aa.."    , "a"),
		NEWCASE(2P,  1, '.', 0, 0, 0, "..aa.bb"   , "a", "b"),
		NEWCASE(2Q,  1, '.', 0, 0, 0, "..aa.bb."  , "a", "b"),
		NEWCASE(2R,  1, '.', 0, 0, 0, "..aa.bb.." , "a", "b"),
		NEWCASE(2S,  1, '.', 0, 0, 0, "..aa..bb"  , "a", "b"),
		NEWCASE(2T,  1, '.', 0, 0, 0, "..aa..bb." , "a", "b"),
		NEWCASE(2U,  1, '.', 0, 0, 0, "..aa..bb..", "a", "b"),
	};
#undef STATICSTR
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		int err = 0;
		brrsz errcse = 0;
		brrsz errel = 0;
		char errels[69+420] = {0};
		brrsz errln = 0;
		cse.result = brrstr_split(cse.str, cse.sep, cse.offset, cse.max_split_size, &cse.count);
#if 1
		BRRTEST_ASSERT(cse.count == cse.correct_count, MSG("case %zu '%s' incorrect count. Expected %zu, got %zu", i, cse.name, cse.correct_count, cse.count));
		for (brrsz j = 0; j < cse.count - 1; ++j) {
			if (!err) {
				err = strcmp(cse.result[j].cstr, cse.correct[j].cstr) != 0;
				if (err) {
					errcse = i;
					errel = j;
					errln = cse.result[j].length;
					memcpy(errels, cse.result[j].cstr, errln+1);
				}
			}
			brrstr_delete(&cse.result[j]);
		}
		BRRTEST_ASSERT(!err, MSG("case %zu '%s' element %zu failed. Expected '%s':%zu, got '%s':%zu", errcse, cases[errcse].name, errel, cases[errcse].correct[errel].cstr, cases[errcse].correct[errel].length, errels,errln));
#else
		BRRLOG_NORN("Split %s '%s' into %zu pieces: [", cse.name, cse.str.cstr, cse.count);
		for (brrsz j = 0; j < cse.count - 1; ++j) {
			BRRLOG_NORN("'%s', ", BRRTIL_SSTR(cse.result[j].cstr));
			brrstr_delete(&cse.result[j]);
		}
		BRRLOG_NOR("'%s']", BRRTIL_SSTR(cse.result[cse.count - 1].cstr));
#endif
		brrstr_delete(&cse.result[cse.count - 1]);
		brrlib_alloc((void **)&cse.result, 0, 0);
	}
	return "";
}
BRRTEST_SUITE(str, 0
	,STR_STRLEN
	,STR_NEW
	,STR_DELETE
	,STR_DUPLICATE
	,STR_COPY
	,STR_RESIZE
	,STR_STRIP
	,STR_TRIM
	,STR_STR_COMPARE
	,STR_STARTS_WITH
	,STR_ENDS_WITH
	,STR_SPLIT
)

int main(void)
{
	BRRTEST_RUN_SUITE(log);
	BRRTEST_RUN_SUITE(mem);
	BRRTEST_RUN_SUITE(lib);
	BRRTEST_RUN_SUITE(str);
}
