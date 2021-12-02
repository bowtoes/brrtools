#include <errno.h>
#include <string.h>

#include "brrtools/brrtest.h"
#include "brrtools/brrlog.h"

BRRTEST_NEW_TEST(LOG_INIT) {
	BRRTEST_ASSERT(!brrlog_init(), brrtools_test_message("Failed to initialize log : %s", strerror(errno)));
	return "";
}
BRRTEST_NEW_TEST(LOG_MESSAGE) {
	static const char fmt[] = "%s wow\n";
	static const char msg[] = "wow";
	static const brrsz len = sizeof(msg) + sizeof(fmt) - 2 - 2;
	static brrsz logged = 0;
	gbrrlogctl.style_enabled = 0;
	BRRTEST_ASSERT(!brrlog_init(), "could not init log");
	BRRTEST_ASSERT(len == (logged = brrlog_text(-1, -1, NULL, -1, -1, -1, -1, NULL, 0, 0, __FILE__, __func__, 0, fmt, msg)), brrtools_test_message("logged incorrect amount of bytes: %zu != %zu", len, logged));
	return "";
}
BRRTEST_NEW_TEST(LOG_PRIORITIES) {
	static const char fmt[] = "wow nice styles dude\n";
	gbrrlogctl.style_enabled = 1;
	BRRTEST_ASSERT(!brrlog_init(), "could not init log");
	brrlog_setminpriority(brrlog_priority_none);
	brrlog_setmaxpriority(brrlog_priority_count);
	gbrrlogctl.debug_enabled = 1;
	BRRTEST_ASSERT(0 != BRRLOG_CRIN(fmt), "did not log critical with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_ERRN(fmt), "did not log error with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_NORN(fmt), "did not log normal with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_WARN(fmt), "did not log warning with max priority at none, min at critical");
	BRRTEST_ASSERT(0 != BRRLOG_DEBN(fmt), "did not log debug while enabled");
	brrlog_setminpriority(brrlog_priority_error);
	brrlog_setmaxpriority(brrlog_priority_normal);
	gbrrlogctl.debug_enabled = 0;
	BRRTEST_ASSERT(0 == BRRLOG_CRIN(fmt), "logged critical with max priority at normal, min at error");
	BRRTEST_ASSERT(0 != BRRLOG_ERRN(fmt), "did not log error with max priority at normal, min at error");
	BRRTEST_ASSERT(0 != BRRLOG_NORN(fmt), "did not log normal with max priority at normal, min at error");
	BRRTEST_ASSERT(0 == BRRLOG_WARN(fmt), "logged warning with max priority at normal, min at error");
	BRRTEST_ASSERT(0 == BRRLOG_DEBN(fmt), "logged debug while disabled");
	return "";
}
BRRTEST_NEW_TEST(LOG_DIGITS) {
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
	gbrrlogctl.style_enabled = 0;
	for (brrsz i = 0; i < (BRR_ARRAY_LENGTH(cases)); ++i) {
		struct testcase cse = cases[i];
		cse.res_print = BRRLOGD_NORBNP(cse.result, cse.number, cse.base, cse.is_signed, cse.separator, cse.spacing);
		cse.res_length = strlen(cse.result);
		BRRTEST_ASSERT(cse.res_print == cse.res_length, brrtools_test_message("case %zu bytes printed does not match measured length : %zu != %zu ('%s' , expected '%s')", i, cse.res_print, cse.res_length, cse.result, cse.correct));
		BRRTEST_ASSERT(cse.res_length == cse.correct_length, brrtools_test_message("case %zu res length does not match correct length : %zu != %zu ('%s' , expected '%s')", i, cse.res_length, cse.correct_length, cse.result, cse.correct));
		cse.cmp = strcmp(cse.correct, cse.result);
		BRRTEST_ASSERT(cse.cmp == 0, brrtools_test_message("case %zu string does not match expected : '%s' != '%s'", i, cse.result, cse.correct));
	}

	return "";
}
BRRTEST_NEW_TEST(LOG_BITS) {
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
	gbrrlogctl.style_enabled = 0;
	for (brrsz i = 0; i < (BRR_ARRAY_LENGTH(cases)); ++i) {
		struct testcase cse = cases[i];
		cse.res_print = BRRLOGB_NORBNP(cse.result, cse.data, cse.bits_to_print, cse.reverse_bytes, cse.reverse_bits, cse.separator, cse.spacing);
		cse.res_length = strlen(cse.result);
		BRRTEST_ASSERT(cse.res_print == cse.res_length, brrtools_test_message("case %zu bytes printed does not match measured length : %zu != %zu ('%s' , expected '%s')", i, cse.res_print, cse.res_length, cse.result, cse.correct));
		BRRTEST_ASSERT(cse.res_length == cse.correct_length, brrtools_test_message("case %zu res length does not match correct length : %zu != %zu ('%s' , expected '%s')", i, cse.res_length, cse.correct_length, cse.result, cse.correct));
		cse.cmp = strcmp(cse.correct, cse.result);
		BRRTEST_ASSERT(cse.cmp == 0, brrtools_test_message("case %zu string does not match expected : '%s' != '%s'", i, cse.result, cse.correct));
	}
	return "";
}
BRRTEST_NEW_TEST(LOG_DEINIT) {
	brrlog_deinit();
	return "";
}
BRRTEST_NEW_TEST(LOG_SETLOGMAX) {
	static const brrsz cases[] = {
		4096, 2048, 1024, 512, 0,
	};
	BRR_ITER_ARRAY(brrsz, i, cases) {
		BRRTEST_ASSERT(!brrlog_setlogmax(cases[i]), brrtools_test_message("Failed to set max to %zu : %s", cases[i], strerror(errno)));
	}
	return "";
}
BRRTEST_NEW_SUITE(log, 0
	,LOG_INIT
	,LOG_MESSAGE
	,LOG_PRIORITIES
	,LOG_DIGITS
	,LOG_BITS
	,LOG_DEINIT
	,LOG_SETLOGMAX
)
