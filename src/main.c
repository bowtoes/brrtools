#include <ctype.h>
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
#include <brrtools/brrstg.h>

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
		BRRTEST_ASSERT(cse.cmp == 0, MSG("case %zu failed. Expected '%s', got '%s'", i, BRRTIL_NULSTR(cse.data), BRRTIL_NULSTR(cse.result)));
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

#define CSEPREAMBLE(...) BRRTEST_ASSERT(0 == brrstg_new(__VA_ARGS__), \
		    MSG(CSESTR" failed. Could not init result string : %s", CSEARGS, strerror(errno)))
#define CSEERRCK(_r_, _c_) do { \
	int _e_ = 0; \
	brrsz _el_ = 0; \
	char _es_[420] = {0}; \
	if ((_r_)->length != (_c_)->length) { \
		_e_ = 1; \
	} else if (0 != strcmp((_r_)->opaque, (_c_)->opaque)) { \
		_e_ = 2; \
	} else { \
		_e_ = 0; \
	} \
	if (_e_) { \
		_el_ = (_r_)->length; \
		if ((_r_)->opaque) \
			memcpy(_es_, (_r_)->opaque, _el_ + 1); \
		else \
			memcpy(_es_, "(nul)", 6); \
	} \
	brrstg_delete((_r_)); \
	BRRTEST_ASSERT(_e_ != 1, \
	    MSG(CSESTR" had incorrect length. Expected %zu, got %zu ('%s', '%s')", \
	    CSEARGS, \
	    (_c_)->length, _el_, BRRTIL_EMPTYSTR((char *)(_c_)->opaque), _es_)); \
	BRRTEST_ASSERT(_e_ != 2, \
	    MSG(CSESTR" failed. Expected '%s', got '%s' (%zu, %zu)", \
	    CSEARGS, \
	    BRRTIL_EMPTYSTR((char *)(_c_)->opaque), _es_, (_c_)->length, _el_)); \
} while (0)
BRRTEST_TEST(STG_STRLEN) {
	struct testcase {
		const char *const cstr;
		brrsz max;
		const brrsz correct;
		brrsz result;
	};
#define NEWCASE(_s_, _m_) {_s_, _m_, (_m_<=(sizeof(_s_)-1)?_m_:(sizeof(_s_)-1)), 0}
	static const struct testcase cases[] = {
		{NULL, -1, 0, 0},
		NEWCASE("", -1),
		NEWCASE("1234567890", -1),
		NEWCASE("1234567890", 0),
		NEWCASE("1234567890", 1),
		NEWCASE("1234567890", 2),
		NEWCASE("1234567890", 3),
		NEWCASE("1234567890", 4),
		NEWCASE("1234567890", 5),
		NEWCASE("1234567890", 6),
		NEWCASE("1234567890", 7),
		NEWCASE("1234567890", 8),
		NEWCASE("1234567890", 9),
		NEWCASE("1234567890", 10),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		cse.result = brrstg_strlen(cse.cstr, cse.max);
		BRRTEST_ASSERT(cse.result == cse.correct, MSG("case %zu failed. Expected %zu, got %zu ('%s')", i+1, cse.correct, cse.result, BRRTIL_NULSTR(cse.cstr)));
	}
	return "";
}
BRRTEST_TEST(STG_NEW) {
	struct testcase {
		const char *const cstr;
		brrsz max;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_, _m_, _c_) {_s_, _m_, {.opaque=_c_,.length=sizeof(_c_)-1}, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL, -1, ""),
		NEWCASE("", -1, ""),
		NEWCASE("1234567890", -1, "1234567890"),
		NEWCASE("1234567890",  0, ""),
		NEWCASE("1234567890",  1, "1"),
		NEWCASE("1234567890",  2, "12"),
		NEWCASE("1234567890",  3, "123"),
		NEWCASE("1234567890",  4, "1234"),
		NEWCASE("1234567890",  5, "12345"),
		NEWCASE("1234567890",  6, "123456"),
		NEWCASE("1234567890",  7, "1234567"),
		NEWCASE("1234567890",  8, "12345678"),
		NEWCASE("1234567890",  9, "123456789"),
		NEWCASE("1234567890", 10, "1234567890"),
		NEWCASE("1234567890", 11, "1234567890"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, cse.max);
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}

	return "";
}
BRRTEST_TEST(STG_DELETE) {
	struct testcase {
		const char *const cstr;
		brrstgT result;
	};
#define NEWCASE(_s_) {_s_, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL),
		NEWCASE(""),
		NEWCASE("WOAH"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		brrstg_delete(&cse.result);
		BRRTEST_ASSERT(cse.result.length == 0, MSG("case %zu failed. 'length' was not 0", i+1));
		BRRTEST_ASSERT(cse.result.opaque == NULL, MSG("case %zu failed. 'opaque' was not NULL", i+1));
	};
	return "";
}
BRRTEST_TEST(STG_STRING) {
	struct testcase {
		const char *const cstr;
		brrstgT result;
	};
#define NEWCASE(_s_) {_s_, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL),
		NEWCASE(""),
		NEWCASE("1"),
		NEWCASE("12"),
		NEWCASE("123"),
		NEWCASE("1234"),
		NEWCASE("12345"),
		NEWCASE("123456"),
		NEWCASE("1234567"),
		NEWCASE("12345678"),
		NEWCASE("123456789"),
		NEWCASE("1234567890"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		static char result_cstr[420+69];
		BRRTEST_ASSERT(0 == brrstg_new(&cse.result, cse.cstr, -1), MSG("case %zu failed. Could not init string '%s' : %s", i+1, BRRTIL_NULSTR(cse.cstr), strerror(errno)));
		brrstg_string(&cse.result, result_cstr);
		brrstg_delete(&cse.result);
		BRRTEST_ASSERT(0 == strcmp(BRRTIL_EMPTYSTR(cse.cstr), result_cstr), MSG("case %zu failed. Expected '%s', got '%s'", i+1, BRRTIL_EMPTYSTR(cse.cstr), result_cstr));
	}
	return "";
}
BRRTEST_TEST(STG_RAW) {
	struct testcase {
		const char *const cstr;
		const char *result_cstr;
		brrstgT result;
	};
#define NEWCASE(_s_) {_s_, NULL, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL),
		NEWCASE(""),
		NEWCASE("1"),
		NEWCASE("12"),
		NEWCASE("123"),
		NEWCASE("1234"),
		NEWCASE("12345"),
		NEWCASE("123456"),
		NEWCASE("1234567"),
		NEWCASE("12345678"),
		NEWCASE("123456789"),
		NEWCASE("1234567890"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
		struct testcase cse = cases[i];
		static char errstr[420];
		static int err = 0;
		BRRTEST_ASSERT(0 == brrstg_new(&cse.result, cse.cstr, -1), MSG("case %zu failed. Could not init string '%s' : %s", i+1, BRRTIL_NULSTR(cse.cstr), strerror(errno)));
		cse.result_cstr = brrstg_raw(&cse.result);
		if (0 == strcmp(cse.result_cstr, BRRTIL_EMPTYSTR(cse.cstr))) {
			err = 0;
		} else {
			err = 1;
			memcpy(errstr, cse.result.opaque, cse.result.length + 1);
		}
		brrstg_delete(&cse.result);
		BRRTEST_ASSERT(err != 1, MSG("case %zu failed. Expected '%s', got '%s'", i+1, BRRTIL_EMPTYSTR(cse.cstr), errstr));
	}
	return "";
}
BRRTEST_TEST(STG_RESIZE) {
	struct testcase {
		const char *const cstr;
		brrsz newlen;
		char fill;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_, _n_, _f_, _c_) {_s_, _n_, _f_, {.opaque=_c_,.length=_n_}, {0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL, 0, 0, ""),
		NEWCASE("", 0, 0, ""),
		NEWCASE("1234567890",  0,   0, ""),
		NEWCASE("1234567890",  1,   0, "1"),
		NEWCASE("1234567890",  2,   0, "12"),
		NEWCASE("1234567890",  3,   0, "123"),
		NEWCASE("1234567890",  4,   0, "1234"),
		NEWCASE("1234567890",  5,   0, "12345"),
		NEWCASE("1234567890",  6,   0, "123456"),
		NEWCASE("1234567890",  7,   0, "1234567"),
		NEWCASE("1234567890",  8,   0, "12345678"),
		NEWCASE("1234567890",  9,   0, "123456789"),
		NEWCASE("1234567890", 10,   0, "1234567890"),
		NEWCASE("1234567890", 11,   0, "1234567890 "),
		NEWCASE("1234567890", 12,   0, "1234567890  "),
		NEWCASE("1234567890", 12, '*', "1234567890**"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		BRRTEST_ASSERT(0 == brrstg_resize(&cse.result, cse.newlen, cse.fill),
			MSG(CSESTR" failed. Could not resize : %s", CSEARGS, strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	};
	return "";
}
BRRTEST_TEST(STG_COPY) {
	struct testcase {
		const brrstgT str;
		brrstgT result;
	};
#define NEWCASE(_s_) {{.opaque=_s_,.length=sizeof(_s_)-1}, {0}}
	static const struct testcase cases[] = {
		NEWCASE(""),
		NEWCASE("12345"),
		NEWCASE("1234567890"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, NULL, 0);
		BRRTEST_ASSERT(0 == brrstg_copy(&cse.result, &cse.str),
		    MSG(CSESTR" failed to copy string : %s", CSEARGS, strerror(errno)));
		CSEERRCK(&cse.result, &cse.str);
#undef CSESTR
#undef CSEARGS
	}

	return "";
}
BRRTEST_TEST(STG_MERGE) {
	struct testcase {
		int use_prefix;
		const brrstgT prefix;
		int use_suffix;
		const brrstgT suffix;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_p_, _s_, _c_) {(_p_)?1:0,{.opaque=(_p_),.length=(_p_)?sizeof(_p_)-1:0},(_s_)?1:0,{.opaque=(_s_),.length=(_s_)?sizeof(_s_)-1:0},{.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE(NULL, NULL, ""),
		NEWCASE(NULL, "b", "b"),
		NEWCASE("a", NULL, "a"),
		NEWCASE("a", "b", "ab"),
		NEWCASE(NULL, "bb", "bb"),
		NEWCASE("aa", NULL, "aa"),
		NEWCASE("aa", "bb", "aabb"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, NULL, 0);
		BRRTEST_ASSERT(0 == brrstg_merge(&cse.result, cse.use_prefix?&cse.prefix:NULL, cse.use_suffix?&cse.suffix:NULL),
		    MSG(CSESTR" failed to join strings '%s' + '%s' : %s.",
		    CSEARGS, BRRTIL_NULSTR((char *)cse.prefix.opaque), BRRTIL_NULSTR((char *)cse.suffix.opaque), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}

	return "";
}
BRRTEST_TEST(STG_APPEND) {
	struct testcase {
		const char *const cstr;
		int use_suffix;
		const brrstgT suffix;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_p_, _s_, _c_) {_p_,(_s_)?1:0,{.opaque=(_s_),.length=(_s_)?sizeof(_s_)-1:0},{.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE("a", NULL, "a"),
		NEWCASE("a", "b", "ab"),
		NEWCASE("a", "bb", "abb"),
		NEWCASE("aa", NULL, "aa"),
		NEWCASE("aa", "b", "aab"),
		NEWCASE("aa", "bb", "aabb"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		BRRTEST_ASSERT(0 == brrstg_append(&cse.result, cse.use_suffix?&cse.suffix:NULL),
		    MSG(CSESTR" failed to append string '%s' <- '%s' : %s.",
		    CSEARGS, BRRTIL_NULSTR(cse.cstr), BRRTIL_NULSTR((char *)cse.suffix.opaque), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}

	return "";
}
BRRTEST_TEST(STG_PREPEND) {
	struct testcase {
		const char *const cstr;
		int use_prefix;
		const brrstgT prefix;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_, _p_, _c_) {_s_,(_p_)?1:0,{.opaque=(_p_),.length=(_p_)?sizeof(_p_)-1:0},{.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE("a", NULL, "a"),
		NEWCASE("a", "b", "ba"),
		NEWCASE("a", "bb", "bba"),
		NEWCASE("aa", NULL, "aa"),
		NEWCASE("aa", "b", "baa"),
		NEWCASE("aa", "bb", "bbaa"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz,	 i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		BRRTEST_ASSERT(0 == brrstg_prepend(&cse.result, cse.use_prefix?&cse.prefix:NULL),
		    MSG(CSESTR" failed to prepend string '%s' -> '%s' : %s.",CSEARGS,
	    	BRRTIL_NULSTR((char *)cse.prefix.opaque), BRRTIL_NULSTR(cse.cstr), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}

	return "";
}
BRRTEST_TEST(STG_STRIP) {
	struct testcase {
		const char *const cstr;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_, _c_) {_s_, {.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE("", ""),NEWCASE(" ", ""),NEWCASE("  ", ""),
		NEWCASE("a", "a"),NEWCASE("a ", "a"),NEWCASE("a  ", "a"),
		NEWCASE(" a", "a"),NEWCASE(" a ", "a"),NEWCASE(" a  ", "a"),
		NEWCASE("  a", "a"),NEWCASE("  a ", "a"),NEWCASE("  a  ", "a"),
		NEWCASE("ab", "ab"),NEWCASE("ab ", "ab"),NEWCASE("ab  ", "ab"),
		NEWCASE(" ab", "ab"),NEWCASE(" ab ", "ab"),NEWCASE(" ab  ", "ab"),
		NEWCASE("  ab", "ab"),NEWCASE("  ab ", "ab"),NEWCASE("  ab  ", "ab"),
		NEWCASE("a b", "a b"),NEWCASE("a b ", "a b"),NEWCASE("a b  ", "a b"),
		NEWCASE(" a b", "a b"),NEWCASE(" a b ", "a b"),NEWCASE(" a b  ", "a b"),
		NEWCASE("  a b", "a b"),NEWCASE("  a b ", "a b"),NEWCASE("  a b  ", "a b"),
		NEWCASE("a  b", "a  b"),NEWCASE("a  b ", "a  b"),NEWCASE("a  b  ", "a  b"),
		NEWCASE(" a  b", "a  b"),NEWCASE(" a  b ", "a  b"),NEWCASE(" a  b  ", "a  b"),
		NEWCASE("  a  b", "a  b"),NEWCASE("  a  b ", "a  b"),NEWCASE("  a  b  ", "a  b"),
		NEWCASE("ab c", "ab c"),NEWCASE("ab c ", "ab c"),NEWCASE("ab c  ", "ab c"),
		NEWCASE(" ab c", "ab c"),NEWCASE(" ab c ", "ab c"),NEWCASE(" ab c  ", "ab c"),
		NEWCASE("  ab c", "ab c"),NEWCASE("  ab c ", "ab c"),NEWCASE("  ab c  ", "ab c"),
		NEWCASE("ab  c", "ab  c"),NEWCASE("ab  c ", "ab  c"),NEWCASE("ab  c  ", "ab  c"),
		NEWCASE(" ab  c", "ab  c"),NEWCASE(" ab  c ", "ab  c"),NEWCASE(" ab  c  ", "ab  c"),
		NEWCASE("  ab  c", "ab  c"),NEWCASE("  ab  c ", "ab  c"),NEWCASE("  ab  c  ", "ab  c"),
		NEWCASE("a bc", "a bc"),NEWCASE("a bc ", "a bc"),NEWCASE("a bc  ", "a bc"),
		NEWCASE(" a bc", "a bc"),NEWCASE(" a bc ", "a bc"),NEWCASE(" a bc  ", "a bc"),
		NEWCASE("  a bc", "a bc"),NEWCASE("  a bc ", "a bc"),NEWCASE("  a bc  ", "a bc"),
		NEWCASE("a  bc", "a  bc"),NEWCASE("a  bc ", "a  bc"),NEWCASE("a  bc  ", "a  bc"),
		NEWCASE(" a  bc", "a  bc"),NEWCASE(" a  bc ", "a  bc"),NEWCASE(" a  bc  ", "a  bc"),
		NEWCASE("  a  bc", "a  bc"),NEWCASE("  a  bc ", "a  bc"),NEWCASE("  a  bc  ", "a  bc"),
		NEWCASE("ab cd", "ab cd"),NEWCASE("ab cd ", "ab cd"),NEWCASE("ab cd  ", "ab cd"),
		NEWCASE(" ab cd", "ab cd"),NEWCASE(" ab cd ", "ab cd"),NEWCASE(" ab cd  ", "ab cd"),
		NEWCASE("  ab cd", "ab cd"),NEWCASE("  ab cd ", "ab cd"),NEWCASE("  ab cd  ", "ab cd"),
		NEWCASE("ab  cd", "ab  cd"),NEWCASE("ab  cd ", "ab  cd"),NEWCASE("ab  cd  ", "ab  cd"),
		NEWCASE(" ab  cd", "ab  cd"),NEWCASE(" ab  cd ", "ab  cd"),NEWCASE(" ab  cd  ", "ab  cd"),
		NEWCASE("  ab  cd", "ab  cd"),NEWCASE("  ab  cd ", "ab  cd"),NEWCASE("  ab  cd  ", "ab  cd"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		BRRTEST_ASSERT(0 == brrstg_strip(&cse.result, NULL),
		    MSG(CSESTR" failed to strip string '%s' -> '%s' : %s.",
		    CSEARGS, BRRTIL_NULSTR(cse.cstr), BRRTIL_NULSTR((char *)cse.correct.opaque), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
static int BRRCALL stgfilter(char chr, brrsz position, brrsz length)
{
	return !isspace(chr);
}
BRRTEST_TEST(STG_FILTER) {
	struct testcase {
		const char *const cstr;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_, _c_) {_s_, {.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE("", ""),NEWCASE(" ", ""),NEWCASE("  ", ""),
		NEWCASE("a", "a"),NEWCASE("a ", "a"),NEWCASE("a  ", "a"),
		NEWCASE(" a", "a"),NEWCASE(" a ", "a"),NEWCASE(" a  ", "a"),
		NEWCASE("  a", "a"),NEWCASE("  a ", "a"),NEWCASE("  a  ", "a"),
		NEWCASE("ab", "ab"),NEWCASE("ab ", "ab"),NEWCASE("ab  ", "ab"),
		NEWCASE(" ab", "ab"),NEWCASE(" ab ", "ab"),NEWCASE(" ab  ", "ab"),
		NEWCASE("  ab", "ab"),NEWCASE("  ab ", "ab"),NEWCASE("  ab  ", "ab"),
		NEWCASE("ab", "ab"),NEWCASE("a b ", "ab"),NEWCASE("a b  ", "ab"),
		NEWCASE(" a b", "ab"),NEWCASE(" a b ", "ab"),NEWCASE(" a b  ", "ab"),
		NEWCASE("  a b", "ab"),NEWCASE("  a b ", "ab"),NEWCASE("  a b  ", "ab"),
		NEWCASE("ab", "ab"),NEWCASE("a  b ", "ab"),NEWCASE("a  b  ", "ab"),
		NEWCASE(" a  b", "ab"),NEWCASE(" a  b ", "ab"),NEWCASE(" a  b  ", "ab"),
		NEWCASE("  a  b", "ab"),NEWCASE("  a  b ", "ab"),NEWCASE("  a  b  ", "ab"),
		NEWCASE("abc", "abc"),NEWCASE("ab c ", "abc"),NEWCASE("ab c  ", "abc"),
		NEWCASE(" ab c", "abc"),NEWCASE(" ab c ", "abc"),NEWCASE(" ab c  ", "abc"),
		NEWCASE("  ab c", "abc"),NEWCASE("  ab c ", "abc"),NEWCASE("  ab c  ", "abc"),
		NEWCASE("abc", "abc"),NEWCASE("ab  c ", "abc"),NEWCASE("ab  c  ", "abc"),
		NEWCASE(" ab  c", "abc"),NEWCASE(" ab  c ", "abc"),NEWCASE(" ab  c  ", "abc"),
		NEWCASE("  ab  c", "abc"),NEWCASE("  ab  c ", "abc"),NEWCASE("  ab  c  ", "abc"),
		NEWCASE("abc", "abc"),NEWCASE("a bc ", "abc"),NEWCASE("a bc  ", "abc"),
		NEWCASE(" a bc", "abc"),NEWCASE(" a bc ", "abc"),NEWCASE(" a bc  ", "abc"),
		NEWCASE("  a bc", "abc"),NEWCASE("  a bc ", "abc"),NEWCASE("  a bc  ", "abc"),
		NEWCASE("abc", "abc"),NEWCASE("a  bc ", "abc"),NEWCASE("a  bc  ", "abc"),
		NEWCASE(" a  bc", "abc"),NEWCASE(" a  bc ", "abc"),NEWCASE(" a  bc  ", "abc"),
		NEWCASE("  a  bc", "abc"),NEWCASE("  a  bc ", "abc"),NEWCASE("  a  bc  ", "abc"),
		NEWCASE("abcd", "abcd"),NEWCASE("ab cd ", "abcd"),NEWCASE("ab cd  ", "abcd"),
		NEWCASE(" ab cd", "abcd"),NEWCASE(" ab cd ", "abcd"),NEWCASE(" ab cd  ", "abcd"),
		NEWCASE("  ab cd", "abcd"),NEWCASE("  ab cd ", "abcd"),NEWCASE("  ab cd  ", "abcd"),
		NEWCASE("abcd", "abcd"),NEWCASE("ab  cd ", "abcd"),NEWCASE("ab  cd  ", "abcd"),
		NEWCASE(" ab  cd", "abcd"),NEWCASE(" ab  cd ", "abcd"),NEWCASE(" ab  cd  ", "abcd"),
		NEWCASE("  ab  cd", "abcd"),NEWCASE("  ab  cd ", "abcd"),NEWCASE("  ab  cd  ", "abcd"),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu"
#define CSEARGS i+1
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, cse.cstr, -1);
		BRRTEST_ASSERT(0 == brrstg_filter(&cse.result, stgfilter),
		    MSG(CSESTR" failed to filter string '%s' -> '%s' : %s.", CSEARGS, BRRTIL_NULSTR(cse.cstr), BRRTIL_NULSTR((char *)cse.correct.opaque), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
BRRTEST_TEST(STG_RANGE) {
	struct testcase {
		const brrstgT source;
		brrsz start;
		brrsz end;
		int reverse;
		int wrap;
		const brrstgT correct;
		brrstgT result;
	};
#define NEWCASE(_s_,_S_,_e_,_r_,_w_,_c_) {{.opaque=_s_,.length=sizeof(_s_)-1},_S_,_e_,_r_,_w_,{.opaque=_c_,.length=sizeof(_c_)-1},{0}}
	static const struct testcase cases[] = {
		NEWCASE("1234567890", 0,  0, 0, 0, ""),NEWCASE("1234567890", 0,  1, 0, 0, "1"),
		NEWCASE("1234567890", 0,  2, 0, 0, "12"),NEWCASE("1234567890", 0,  3, 0, 0, "123"),
		NEWCASE("1234567890", 0,  4, 0, 0, "1234"),NEWCASE("1234567890", 0,  5, 0, 0, "12345"),
		NEWCASE("1234567890", 0,  6, 0, 0, "123456"),NEWCASE("1234567890", 0,  7, 0, 0, "1234567"),
		NEWCASE("1234567890", 0,  8, 0, 0, "12345678"),NEWCASE("1234567890", 0,  9, 0, 0, "123456789"),
		NEWCASE("1234567890", 0, 10, 0, 0, "1234567890"),NEWCASE("1234567890", 0, 11, 0, 0, "1234567890"),
		NEWCASE("1234567890", 0, 12, 0, 0, "1234567890"),
		NEWCASE("1234567890", 0,  0, 1, 0, ""),NEWCASE("1234567890", 0,  1, 1, 0, "1"),
		NEWCASE("1234567890", 0,  2, 1, 0, "21"),NEWCASE("1234567890", 0,  3, 1, 0, "321"),
		NEWCASE("1234567890", 0,  4, 1, 0, "4321"),NEWCASE("1234567890", 0,  5, 1, 0, "54321"),
		NEWCASE("1234567890", 0,  6, 1, 0, "654321"),NEWCASE("1234567890", 0,  7, 1, 0, "7654321"),
		NEWCASE("1234567890", 0,  8, 1, 0, "87654321"),NEWCASE("1234567890", 0,  9, 1, 0, "987654321"),
		NEWCASE("1234567890", 0, 10, 1, 0, "0987654321"),NEWCASE("1234567890", 0, 11, 1, 0, "0987654321"),
		NEWCASE("1234567890", 0, 12, 1, 0, "0987654321"),
		NEWCASE("1234567890", 0,  0, 0, 1, ""),NEWCASE("1234567890", 0,  1, 0, 1, "1"),
		NEWCASE("1234567890", 0,  2, 0, 1, "12"),NEWCASE("1234567890", 0,  3, 0, 1, "123"),
		NEWCASE("1234567890", 0,  4, 0, 1, "1234"),NEWCASE("1234567890", 0,  5, 0, 1, "12345"),
		NEWCASE("1234567890", 0,  6, 0, 1, "123456"),NEWCASE("1234567890", 0,  7, 0, 1, "1234567"),
		NEWCASE("1234567890", 0,  8, 0, 1, "12345678"),NEWCASE("1234567890", 0,  9, 0, 1, "123456789"),
		NEWCASE("1234567890", 0, 10, 0, 1, "1234567890"),NEWCASE("1234567890", 0, 11, 0, 1, ""),
		NEWCASE("1234567890", 0, 12, 0, 1, "1"),
		NEWCASE("1234567890", 0,  0, 1, 1, ""),NEWCASE("1234567890", 0,  1, 1, 1, "1"),
		NEWCASE("1234567890", 0,  2, 1, 1, "21"),NEWCASE("1234567890", 0,  3, 1, 1, "321"),
		NEWCASE("1234567890", 0,  4, 1, 1, "4321"),NEWCASE("1234567890", 0,  5, 1, 1, "54321"),
		NEWCASE("1234567890", 0,  6, 1, 1, "654321"),NEWCASE("1234567890", 0,  7, 1, 1, "7654321"),
		NEWCASE("1234567890", 0,  8, 1, 1, "87654321"),NEWCASE("1234567890", 0,  9, 1, 1, "987654321"),
		NEWCASE("1234567890", 0, 10, 1, 1, "0987654321"),NEWCASE("1234567890", 0, 11, 1, 1, ""),
		NEWCASE("1234567890", 0, 12, 1, 1, "1"),
		NEWCASE("1234567890", 5,  0, 0, 0, "12345"),NEWCASE("1234567890", 5,  1, 0, 0, "2345"),
		NEWCASE("1234567890", 5,  2, 0, 0, "345"),NEWCASE("1234567890", 5,  3, 0, 0, "45"),
		NEWCASE("1234567890", 5,  4, 0, 0, "5"),NEWCASE("1234567890", 5,  5, 0, 0, ""),
		NEWCASE("1234567890", 5,  6, 0, 0, "6"),NEWCASE("1234567890", 5,  7, 0, 0, "67"),
		NEWCASE("1234567890", 5,  8, 0, 0, "678"),NEWCASE("1234567890", 5,  9, 0, 0, "6789"),
		NEWCASE("1234567890", 5, 10, 0, 0, "67890"),NEWCASE("1234567890", 5, 11, 0, 0, "67890"),
		NEWCASE("1234567890", 5, 12, 0, 0, "67890"),
		NEWCASE("1234567890", 5,  0, 1, 0, "54321"),NEWCASE("1234567890", 5,  1, 1, 0, "5432"),
		NEWCASE("1234567890", 5,  2, 1, 0, "543"),NEWCASE("1234567890", 5,  3, 1, 0, "54"),
		NEWCASE("1234567890", 5,  4, 1, 0, "5"),NEWCASE("1234567890", 5,  5, 1, 0, ""),
		NEWCASE("1234567890", 5,  6, 1, 0, "6"),NEWCASE("1234567890", 5,  7, 1, 0, "76"),
		NEWCASE("1234567890", 5,  8, 1, 0, "876"),NEWCASE("1234567890", 5,  9, 1, 0, "9876"),
		NEWCASE("1234567890", 5, 10, 1, 0, "09876"),NEWCASE("1234567890", 5, 11, 1, 0, "09876"),
		NEWCASE("1234567890", 5, 12, 1, 0, "09876"),
		NEWCASE("1234567890", 5,  0, 0, 1, "12345"),NEWCASE("1234567890", 5,  1, 0, 1, "2345"),
		NEWCASE("1234567890", 5,  2, 0, 1, "345"),NEWCASE("1234567890", 5,  3, 0, 1, "45"),
		NEWCASE("1234567890", 5,  4, 0, 1, "5"),NEWCASE("1234567890", 5,  5, 0, 1, ""),
		NEWCASE("1234567890", 5,  6, 0, 1, "6"),NEWCASE("1234567890", 5,  7, 0, 1, "67"),
		NEWCASE("1234567890", 5,  8, 0, 1, "678"),NEWCASE("1234567890", 5,  9, 0, 1, "6789"),
		NEWCASE("1234567890", 5, 10, 0, 1, "67890"),NEWCASE("1234567890", 5, 11, 0, 1, "12345"),
		NEWCASE("1234567890", 5, 12, 0, 1, "2345"),
		NEWCASE("1234567890", 5,  0, 1, 1, "54321"),NEWCASE("1234567890", 5,  1, 1, 1, "5432"),
		NEWCASE("1234567890", 5,  2, 1, 1, "543"),NEWCASE("1234567890", 5,  3, 1, 1, "54"),
		NEWCASE("1234567890", 5,  4, 1, 1, "5"),NEWCASE("1234567890", 5,  5, 1, 1, ""),
		NEWCASE("1234567890", 5,  6, 1, 1, "6"),NEWCASE("1234567890", 5,  7, 1, 1, "76"),
		NEWCASE("1234567890", 5,  8, 1, 1, "876"),NEWCASE("1234567890", 5,  9, 1, 1, "9876"),
		NEWCASE("1234567890", 5, 10, 1, 1, "09876"),NEWCASE("1234567890", 5, 11, 1, 1, "54321"),
		NEWCASE("1234567890", 5, 12, 1, 1, "5432"),
		NEWCASE("1234567890", 12,  0, 0, 0, "1234567890"),NEWCASE("1234567890", 12,  1, 0, 0, "234567890"),
		NEWCASE("1234567890", 12,  2, 0, 0, "34567890"),NEWCASE("1234567890", 12,  3, 0, 0, "4567890"),
		NEWCASE("1234567890", 12,  4, 0, 0, "567890"),NEWCASE("1234567890", 12,  5, 0, 0, "67890"),
		NEWCASE("1234567890", 12,  6, 0, 0, "7890"),NEWCASE("1234567890", 12,  7, 0, 0, "890"),
		NEWCASE("1234567890", 12,  8, 0, 0, "90"),NEWCASE("1234567890", 12,  9, 0, 0, "0"),
		NEWCASE("1234567890", 12, 10, 0, 0, ""),NEWCASE("1234567890", 12, 11, 0, 0, ""),
		NEWCASE("1234567890", 12, 12, 0, 0, ""),
		NEWCASE("1234567890", 12,  0, 1, 0, "0987654321"),NEWCASE("1234567890", 12,  1, 1, 0, "098765432"),
		NEWCASE("1234567890", 12,  2, 1, 0, "09876543"),NEWCASE("1234567890", 12,  3, 1, 0, "0987654"),
		NEWCASE("1234567890", 12,  4, 1, 0, "098765"),NEWCASE("1234567890", 12,  5, 1, 0, "09876"),
		NEWCASE("1234567890", 12,  6, 1, 0, "0987"),NEWCASE("1234567890", 12,  7, 1, 0, "098"),
		NEWCASE("1234567890", 12,  8, 1, 0, "09"),NEWCASE("1234567890", 12,  9, 1, 0, "0"),
		NEWCASE("1234567890", 12, 10, 1, 0, ""),NEWCASE("1234567890", 12, 11, 1, 0, ""),
		NEWCASE("1234567890", 12, 12, 1, 0, ""),
		NEWCASE("1234567890", 12,  0, 0, 1, "1"),NEWCASE("1234567890", 12,  1, 0, 1, ""),
		NEWCASE("1234567890", 12,  2, 0, 1, "2"),NEWCASE("1234567890", 12,  3, 0, 1, "23"),
		NEWCASE("1234567890", 12,  4, 0, 1, "234"),NEWCASE("1234567890", 12,  5, 0, 1, "2345"),
		NEWCASE("1234567890", 12,  6, 0, 1, "23456"),NEWCASE("1234567890", 12,  7, 0, 1, "234567"),
		NEWCASE("1234567890", 12,  8, 0, 1, "2345678"),NEWCASE("1234567890", 12,  9, 0, 1, "23456789"),
		NEWCASE("1234567890", 12, 10, 0, 1, "234567890"),NEWCASE("1234567890", 12, 11, 0, 1, "1"),
		NEWCASE("1234567890", 12, 12, 0, 1, ""),
		NEWCASE("1234567890", 12,  0, 1, 1, "1"),NEWCASE("1234567890", 12,  1, 1, 1, ""),
		NEWCASE("1234567890", 12,  2, 1, 1, "2"),NEWCASE("1234567890", 12,  3, 1, 1, "32"),
		NEWCASE("1234567890", 12,  4, 1, 1, "432"),NEWCASE("1234567890", 12,  5, 1, 1, "5432"),
		NEWCASE("1234567890", 12,  6, 1, 1, "65432"),NEWCASE("1234567890", 12,  7, 1, 1, "765432"),
		NEWCASE("1234567890", 12,  8, 1, 1, "8765432"),NEWCASE("1234567890", 12,  9, 1, 1, "98765432"),
		NEWCASE("1234567890", 12, 10, 1, 1, "098765432"),NEWCASE("1234567890", 12, 11, 1, 1, "1"),
		NEWCASE("1234567890", 12, 12, 1, 1, ""),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu (%zu -> %zu, %s%s)"
#define CSEARGS i+1, cse.start, cse.end, cse.reverse?"reversed":"normal", cse.wrap?" + wrap":""
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, NULL, 0);
		BRRTEST_ASSERT(0 == brrstg_range(&cse.result, &cse.source, cse.start, cse.end, cse.reverse, cse.wrap),
		    MSG(CSESTR" failed to range string '%s' : %s", CSEARGS, BRRTIL_EMPTYSTR((char *)cse.source.opaque), strerror(errno)));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
BRRTEST_TEST(STG_AJOIN) {
	struct testcase {
		const brrstgT *separator;
		int skip_empty;
		brrsz count;
		const brrstgT *const strings;
		const brrstgT correct;
		brrstgT result;
	};
#define STATICSTR(_s_) {_s_, sizeof(_s_) - 1}
#define NEWCASE(_s_, _e_, _c_, _C_, ...) {_s_?(const brrstgT[]){STATICSTR(_s_)}:NULL, _e_, _c_, _c_?(const brrstgT[_c_?_c_:1]){BRRMAP_LIST(STATICSTR, __VA_ARGS__)}:NULL, {.opaque=_C_,.length=sizeof(_C_) - 1}, {0}}
	const struct testcase cases[] = {
		NEWCASE(NULL, 0, 0, "", NULL),
		NEWCASE(NULL, 0, 1, "", ""),
		NEWCASE(NULL, 0, 2, "", "", ""),
		NEWCASE(NULL, 0, 3, "", "", "", ""),

		NEWCASE(NULL, 1, 0, "", NULL),
		NEWCASE(NULL, 1, 1, "", ""),
		NEWCASE(NULL, 1, 2, "", "", ""),
		NEWCASE(NULL, 1, 3, "", "", "", ""),

		NEWCASE(NULL, 0, 0, "", NULL),
		NEWCASE(NULL, 0, 1, "a", "a"),
		NEWCASE(NULL, 0, 2, "ab", "a", "b"),
		NEWCASE(NULL, 0, 3, "ab", "a", "", "b"),

		NEWCASE(NULL, 1, 0, "", NULL),
		NEWCASE(NULL, 1, 1, "a", "a"),
		NEWCASE(NULL, 1, 2, "ab", "a", "b"),
		NEWCASE(NULL, 1, 3, "ab", "a", "", "b"),

		NEWCASE("", 0, 0, "", NULL),
		NEWCASE("", 0, 1, "", ""),
		NEWCASE("", 0, 2, "", "", ""),
		NEWCASE("", 0, 3, "", "", "", ""),

		NEWCASE("", 1, 0, "", NULL),
		NEWCASE("", 1, 1, "", ""),
		NEWCASE("", 1, 2, "", "", ""),
		NEWCASE("", 1, 3, "", "", "", ""),

		NEWCASE("", 0, 0, "", NULL),
		NEWCASE("", 0, 1, "a", "a"),
		NEWCASE("", 0, 2, "ab", "a", "b"),
		NEWCASE("", 0, 3, "ab", "a", "", "b"),

		NEWCASE("", 1, 0, "", NULL),
		NEWCASE("", 1, 1, "a", "a"),
		NEWCASE("", 1, 2, "ab", "a", "b"),
		NEWCASE("", 1, 3, "ab", "a", "", "b"),

		NEWCASE("|", 0, 0, "", NULL),
		NEWCASE("|", 0, 1, "", ""),
		NEWCASE("|", 0, 2, "|", "", ""),
		NEWCASE("|", 0, 3, "||", "", "", ""),

		NEWCASE("|", 1, 0, "", NULL),
		NEWCASE("|", 1, 1, "", ""),
		NEWCASE("|", 1, 2, "", "", ""),
		NEWCASE("|", 1, 3, "", "", "", ""),

		NEWCASE("|", 0, 0, "", NULL),
		NEWCASE("|", 0, 1, "a", "a"),
		NEWCASE("|", 0, 2, "a|b", "a", "b"),
		NEWCASE("|", 0, 3, "a||b", "a", "", "b"),

		NEWCASE("|", 1, 0, "", NULL),
		NEWCASE("|", 1, 1, "a", "a"),
		NEWCASE("|", 1, 2, "a|b", "a", "b"),
		NEWCASE("|", 1, 3, "a|b", "a", "", "b"),
	};
#undef NEWCASE
#undef STATICSTR
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu (%s %zu, '%s')"
#define CSEARGS i+1, cse.skip_empty?"skip":"noskip", cse.count, cse.separator?BRRTIL_NULSTR((char *)cse.separator->opaque):"(nul)"
		struct testcase cse = cases[i];
		CSEPREAMBLE(&cse.result, NULL, 0);
		BRRTEST_ASSERT(0 == brrstg_ajoin(&cse.result, cse.separator, cse.skip_empty, cse.count, cse.strings),
		    MSG(CSESTR" failed to join (expected '%s':%zu)", CSEARGS, BRRTIL_EMPTYSTR((char *)cse.correct.opaque), cse.correct.length));
		CSEERRCK(&cse.result, &cse.correct);
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
BRRTEST_TEST(STG_SPLIT) {
	struct testcase {
		const brrstgT str;
		const brrstgT split;
		int skip;
		int keep;
		brrsz offset;
		brrsz max;
		const brrsz correct_count;
		const brrstgT *const correct;
		brrsz count;
		brrstgT *result;
	};
#define STATICSTR(_s_) {.opaque=_s_, .length=_s_?sizeof(_s_) - 1:0}
#define NEWCASE(_s_, _S_, _sk_, _k_, _o_, _m_, _c_, ...) \
	{ \
		STATICSTR(_s_), STATICSTR(_S_), _sk_, _k_, _o_, _m_, _c_, \
		_c_?(const brrstgT[]){BRRMAP_LIST(STATICSTR,__VA_ARGS__)}:NULL, 0, NULL \
	}
	const struct testcase cases[] = {
		NEWCASE("", "", 0, 0, 0, 0, 1, ""),
		NEWCASE("1234567890",         "",     0, 0, 0, -1, 1, "1234567890"),
		NEWCASE("1234567890",         "  ",   0, 0, 0, -1, 1, "1234567890"),
		NEWCASE("123/_\\/_\\4567890", "/_\\", 0, 0, 0, -1, 3, "123", "", "4567890"),
		NEWCASE("123/_\\/_\\4567890", "/_\\", 1, 0, 0, -1, 2, "123", "4567890"),
		NEWCASE("123/_\\/_\\4567890", "/_\\", 0, 1, 0, -1, 5, "123", "/_\\", "", "/_\\", "4567890"),
		NEWCASE("123/_\\/_\\4567890", "/_\\", 1, 1, 0, -1, 3, "123", "/_\\", "4567890"),
		NEWCASE("123/_\\/_\\4567890/_\\", "/_\\", 0, 0, 0, -1, 4, "123", "", "4567890", ""),
		NEWCASE("123/_\\/_\\4567890/_\\", "/_\\", 1, 0, 0, -1, 2, "123", "4567890"),
		NEWCASE("123/_\\/_\\4567890/_\\", "/_\\", 0, 1, 0, -1, 7, "123", "/_\\", "", "/_\\", "4567890", "/_\\", ""),
		NEWCASE("123/_\\/_\\4567890/_\\", "/_\\", 1, 1, 0, -1, 4, "123", "/_\\", "4567890", "/_\\"),
		NEWCASE("123/_\\/_\\4567890/_\\/_\\", "/_\\", 0, 0, 0, -1, 5, "123", "", "4567890", "", ""),
		NEWCASE("123/_\\/_\\4567890/_\\/_\\", "/_\\", 1, 0, 0, -1, 2, "123", "4567890"),
		NEWCASE("123/_\\/_\\4567890/_\\/_\\", "/_\\", 0, 1, 0, -1, 9, "123", "/_\\", "", "/_\\", "4567890", "/_\\", "", "/_\\", ""),
		NEWCASE("123/_\\/_\\4567890/_\\/_\\", "/_\\", 1, 1, 0, -1, 4, "123", "/_\\", "4567890", "/_\\"),
		NEWCASE("/_\\123/_\\/_\\4567890", "/_\\", 0, 0, 0, -1, 4, "", "123", "", "4567890"),
		NEWCASE("/_\\123/_\\/_\\4567890", "/_\\", 1, 0, 0, -1, 2, "123", "4567890"),
		NEWCASE("/_\\123/_\\/_\\4567890", "/_\\", 0, 1, 0, -1, 7, "", "/_\\", "123", "/_\\", "", "/_\\", "4567890"),
		NEWCASE("/_\\123/_\\/_\\4567890", "/_\\", 1, 1, 0, -1, 3, "123", "/_\\", "4567890"),
		NEWCASE("/_\\/_\\123/_\\/_\\4567890", "/_\\", 0, 0, 0, -1, 5, "", "", "123", "", "4567890"),
		NEWCASE("/_\\/_\\123/_\\/_\\4567890", "/_\\", 1, 0, 0, -1, 2, "123", "4567890"),
		NEWCASE("/_\\/_\\123/_\\/_\\4567890", "/_\\", 0, 1, 0, -1, 9, "", "/_\\", "", "/_\\", "123", "/_\\", "", "/_\\", "4567890"),
		NEWCASE("/_\\/_\\123/_\\/_\\4567890", "/_\\", 1, 1, 0, -1, 3, "123", "/_\\", "4567890"),
	};
#undef NEWCASE
#undef STATICSTR
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu ('%s', split '%s', %zu, %zu, %s%s)"
#define CSEARGS i+1, (char *)cse.str.opaque, (char *)cse.split.opaque, \
		        cse.offset, cse.max, \
		        cse.skip?"skip consecutive":"no skip of cnsec", cse.keep?", keep split":", split drop"
		struct testcase cse = cases[i];
		static int err = 0;
		static brrsz errln = 0;
		static char errstr[420] = {0};
		static brrsz erridx = 0;
		BRRTEST_ASSERT(0 == brrstg_split(&cse.str, &cse.split, cse.skip, cse.keep, cse.offset, cse.max, &cse.result, &cse.count),
			MSG(CSESTR" failed to split string : %s", CSEARGS, strerror(errno)));
		if (cse.count != cse.correct_count)
			err = 1;
		if (cse.result) {
			for (brrsz j = 0; j < cse.count; ++j) {
				if (!err) {
					if (cse.result[j].length != cse.correct[j].length) {
						err = 2;
					} else if (0 != strcmp(cse.result[j].opaque, cse.correct[j].opaque)) {
						err = 3;
					}
					if (err) {
						erridx = j;
						errln = cse.result[j].length;
						memcpy(errstr, cse.result[j].opaque, errln);
					}
				}
				brrstg_delete(&cse.result[j]);
			}
			free(cse.result);
			cse.result = NULL;
		}
		BRRTEST_ASSERT(err != 1, MSG(CSESTR" had incorrect count. Expected %zu, got %zu", CSEARGS, cse.correct_count, cse.count));
		BRRTEST_ASSERT(err != 2, MSG(CSESTR" result %zu had incorrect length. Expected %zu, got %zu ('%s', '%s')", CSEARGS, erridx, cse.correct[erridx].length, errln, BRRTIL_EMPTYSTR((char *)cse.correct[erridx].opaque), errstr));
		BRRTEST_ASSERT(err != 3, MSG(CSESTR" result %zu was incorrect. Expected '%s', got '%s' (%zu, %zu)", CSEARGS, erridx, BRRTIL_EMPTYSTR((char *)cse.correct[erridx].opaque), errstr, cse.correct[erridx].length, errln));
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
BRRTEST_SUITE(stg, 0
    ,STG_STRLEN
    ,STG_NEW
    ,STG_DELETE
    ,STG_STRING
    ,STG_RAW
    ,STG_RESIZE
    ,STG_COPY
    ,STG_MERGE
    ,STG_APPEND
    ,STG_PREPEND
    ,STG_STRIP
    ,STG_FILTER
    ,STG_RANGE
    ,STG_AJOIN
    ,STG_SPLIT
)

int main(void)
{
	BRRTEST_RUN_SUITE(log);
	BRRTEST_RUN_SUITE(mem);
	BRRTEST_RUN_SUITE(lib);
	BRRTEST_RUN_SUITE(stg);
}
