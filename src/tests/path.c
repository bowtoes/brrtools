#include "tests/path.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlog.h"
#include "brrtools/brrtest.h"
#include "brrtools/brrstg.h"
#include "brrtools/brrpath.h"
#include "brrtools/brrtil.h"

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

BRRTEST_TEST(PATH_COMBINE) {
	struct testcase {
		const char *const in_dir;
		const char *const in_bse;
		const char *const in_ext;
		const char *const correct;
		brrstgT result;
	};
#define NEWCASE(_c_, _d_, _b_, _e_) {_d_, _b_, _e_, _c_, {0}}
	static const struct testcase cases[] = {
		NEWCASE("",      NULL, "",   NULL),NEWCASE(".",      NULL, "",   ""),NEWCASE(".a",      NULL, "",   "a"),NEWCASE(".bb",      NULL, "",   "bb"),
		NEWCASE("A",     NULL, "A",  NULL),NEWCASE("A.",     NULL, "A",  ""),NEWCASE("A.a",     NULL, "A",  "a"),NEWCASE("A.bb",     NULL, "A",  "bb"),
		NEWCASE("BB",    NULL, "BB", NULL),NEWCASE("BB.",    NULL, "BB", ""),NEWCASE("BB.a",    NULL, "BB", "a"),NEWCASE("BB.bb",    NULL, "BB", "bb"),
		NEWCASE(""BRRPATH_SEP_STR"",     "",   "",   NULL),NEWCASE(""BRRPATH_SEP_STR".",     "",   "",   ""),NEWCASE(""BRRPATH_SEP_STR".a",     "",   "",   "a"),NEWCASE(""BRRPATH_SEP_STR".bb",     "",   "",   "bb"),
		NEWCASE(""BRRPATH_SEP_STR"A",    "",   "A",  NULL),NEWCASE(""BRRPATH_SEP_STR"A.",    "",   "A",  ""),NEWCASE(""BRRPATH_SEP_STR"A.a",    "",   "A",  "a"),NEWCASE(""BRRPATH_SEP_STR"A.bb",    "",   "A",  "bb"),
		NEWCASE(""BRRPATH_SEP_STR"BB",   "",   "BB", NULL),NEWCASE(""BRRPATH_SEP_STR"BB.",   "",   "BB", ""),NEWCASE(""BRRPATH_SEP_STR"BB.a",   "",   "BB", "a"),NEWCASE(""BRRPATH_SEP_STR"BB.bb",   "",   "BB", "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"",    "x",  "",   NULL),NEWCASE("x"BRRPATH_SEP_STR".",    "x",  "",   ""),NEWCASE("x"BRRPATH_SEP_STR".a",    "x",  "",   "a"),NEWCASE("x"BRRPATH_SEP_STR".bb",    "x",  "",   "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"A",   "x",  "A",  NULL),NEWCASE("x"BRRPATH_SEP_STR"A.",   "x",  "A",  ""),NEWCASE("x"BRRPATH_SEP_STR"A.a",   "x",  "A",  "a"),NEWCASE("x"BRRPATH_SEP_STR"A.bb",   "x",  "A",  "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"BB",  "x",  "BB", NULL),NEWCASE("x"BRRPATH_SEP_STR"BB.",  "x",  "BB", ""),NEWCASE("x"BRRPATH_SEP_STR"BB.a",  "x",  "BB", "a"),NEWCASE("x"BRRPATH_SEP_STR"BB.bb",  "x",  "BB", "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"",   "yy", "",   NULL),NEWCASE("yy"BRRPATH_SEP_STR".",   "yy", "",   ""),NEWCASE("yy"BRRPATH_SEP_STR".a",   "yy", "",   "a"),NEWCASE("yy"BRRPATH_SEP_STR".bb",   "yy", "",   "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"A",  "yy", "A",  NULL),NEWCASE("yy"BRRPATH_SEP_STR"A.",  "yy", "A",  ""),NEWCASE("yy"BRRPATH_SEP_STR"A.a",  "yy", "A",  "a"),NEWCASE("yy"BRRPATH_SEP_STR"A.bb",  "yy", "A",  "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"BB", "yy", "BB", NULL),NEWCASE("yy"BRRPATH_SEP_STR"BB.", "yy", "BB", ""),NEWCASE("yy"BRRPATH_SEP_STR"BB.a", "yy", "BB", "a"),NEWCASE("yy"BRRPATH_SEP_STR"BB.bb", "yy", "BB", "bb"),

		NEWCASE(""BRRPATH_SEP_STR"",      "",   "",   NULL),NEWCASE("."BRRPATH_SEP_STR"",      ".",   "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"",      ".a",   "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"",      ".bb",   "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"",     "A",  "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"",     "A.",  "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"",     "A.a",  "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"",     "A.bb",  "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"",    "BB", "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"",    "BB.", "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"",    "BB.a", "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"",    "BB.bb", "",    NULL),

		NEWCASE(""BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ""BRRPATH_SEP_STR"n",    "",   NULL),NEWCASE("."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    "."BRRPATH_SEP_STR"n",    "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ".a"BRRPATH_SEP_STR"n",    "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ".bb"BRRPATH_SEP_STR"n",    "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A"BRRPATH_SEP_STR"n",   "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A."BRRPATH_SEP_STR"n",   "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A.a"BRRPATH_SEP_STR"n",   "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A.bb"BRRPATH_SEP_STR"n",   "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB"BRRPATH_SEP_STR"n",  "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB."BRRPATH_SEP_STR"n",  "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB.a"BRRPATH_SEP_STR"n",  "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB.bb"BRRPATH_SEP_STR"n",  "",    NULL),
		NEWCASE(""BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ""BRRPATH_SEP_STR"mm",   "",   NULL),NEWCASE("."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   "."BRRPATH_SEP_STR"mm",   "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ".a"BRRPATH_SEP_STR"mm",   "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ".bb"BRRPATH_SEP_STR"mm",   "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A"BRRPATH_SEP_STR"mm",  "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A."BRRPATH_SEP_STR"mm",  "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A.a"BRRPATH_SEP_STR"mm",  "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A.bb"BRRPATH_SEP_STR"mm",  "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB"BRRPATH_SEP_STR"mm", "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB."BRRPATH_SEP_STR"mm", "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB.a"BRRPATH_SEP_STR"mm", "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB.bb"BRRPATH_SEP_STR"mm", "",    NULL),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu ('%s': '%s', '%s', '%s')"
#define CSEARGS i+1, cse.correct, BRRTIL_NULSTR(cse.in_dir), BRRTIL_NULSTR(cse.in_bse), BRRTIL_NULSTR(cse.in_ext)
#define CSEBONUS "('%s')"
#define CSEBONUSARGS err
		static char err[BRRPATH_MAX_PATH+1] = {0};
		struct testcase cse = cases[i];
		int e = 0;
		BRRTEST_ASSERT((e = brrpath_combine(&cse.result, cse.in_dir, cse.in_bse, cse.in_ext)) == 0, MSG(CSESTR" failed %d. Could not combine path : %s", CSEARGS, e, strerror(errno)));
		if (!cse.result.opaque) {
			e = 1;
		} else if (0 != strcmp(cse.correct, cse.result.opaque)) {
			e = 2;
		}
		if (e) {
			memcpy(err, BRRTIL_NULSTR((char *)cse.result.opaque), cse.result.opaque?cse.result.length+1:6);
		}
		brrstg_delete(&cse.result);
		BRRTEST_ASSERT(e != 1, MSG(CSESTR" failed %d. Result was null, expected '%s'", CSEARGS, e, cse.correct));
		BRRTEST_ASSERT(e != 1, MSG(CSESTR" failed %d. Expected '%s', got '%s'", CSEARGS, e, cse.correct, err));
#undef CSESTR
#undef CSEARGS
#undef CSEBONUS
#undef CSEBONUSARGS
	}
	return "";
}
BRRTEST_TEST(PATH_SPLIT) {
	struct testcase {
		const char *const input;
		const char *const correct_directory;
		const char *const correct_base_name;
		const char *const correct_extension;
		brrstgT result_directory;
		brrstgT result_base_name;
		brrstgT result_extension;
	};
#define NEWCASE(_i_, _d_, _b_, _e_) {_i_, _d_, _b_, _e_, {0}, {0}, {0}}
	static const struct testcase cases[] = {
		NEWCASE("",      NULL, "",   NULL),NEWCASE(".",      NULL, "",   ""),NEWCASE(".a",      NULL, "",   "a"),NEWCASE(".bb",      NULL, "",   "bb"),
		NEWCASE("A",     NULL, "A",  NULL),NEWCASE("A.",     NULL, "A",  ""),NEWCASE("A.a",     NULL, "A",  "a"),NEWCASE("A.bb",     NULL, "A",  "bb"),
		NEWCASE("BB",    NULL, "BB", NULL),NEWCASE("BB.",    NULL, "BB", ""),NEWCASE("BB.a",    NULL, "BB", "a"),NEWCASE("BB.bb",    NULL, "BB", "bb"),
		NEWCASE(""BRRPATH_SEP_STR"",     "",   "",   NULL),NEWCASE(""BRRPATH_SEP_STR".",     "",   "",   ""),NEWCASE(""BRRPATH_SEP_STR".a",     "",   "",   "a"),NEWCASE(""BRRPATH_SEP_STR".bb",     "",   "",   "bb"),
		NEWCASE(""BRRPATH_SEP_STR"A",    "",   "A",  NULL),NEWCASE(""BRRPATH_SEP_STR"A.",    "",   "A",  ""),NEWCASE(""BRRPATH_SEP_STR"A.a",    "",   "A",  "a"),NEWCASE(""BRRPATH_SEP_STR"A.bb",    "",   "A",  "bb"),
		NEWCASE(""BRRPATH_SEP_STR"BB",   "",   "BB", NULL),NEWCASE(""BRRPATH_SEP_STR"BB.",   "",   "BB", ""),NEWCASE(""BRRPATH_SEP_STR"BB.a",   "",   "BB", "a"),NEWCASE(""BRRPATH_SEP_STR"BB.bb",   "",   "BB", "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"",    "x",  "",   NULL),NEWCASE("x"BRRPATH_SEP_STR".",    "x",  "",   ""),NEWCASE("x"BRRPATH_SEP_STR".a",    "x",  "",   "a"),NEWCASE("x"BRRPATH_SEP_STR".bb",    "x",  "",   "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"A",   "x",  "A",  NULL),NEWCASE("x"BRRPATH_SEP_STR"A.",   "x",  "A",  ""),NEWCASE("x"BRRPATH_SEP_STR"A.a",   "x",  "A",  "a"),NEWCASE("x"BRRPATH_SEP_STR"A.bb",   "x",  "A",  "bb"),
		NEWCASE("x"BRRPATH_SEP_STR"BB",  "x",  "BB", NULL),NEWCASE("x"BRRPATH_SEP_STR"BB.",  "x",  "BB", ""),NEWCASE("x"BRRPATH_SEP_STR"BB.a",  "x",  "BB", "a"),NEWCASE("x"BRRPATH_SEP_STR"BB.bb",  "x",  "BB", "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"",   "yy", "",   NULL),NEWCASE("yy"BRRPATH_SEP_STR".",   "yy", "",   ""),NEWCASE("yy"BRRPATH_SEP_STR".a",   "yy", "",   "a"),NEWCASE("yy"BRRPATH_SEP_STR".bb",   "yy", "",   "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"A",  "yy", "A",  NULL),NEWCASE("yy"BRRPATH_SEP_STR"A.",  "yy", "A",  ""),NEWCASE("yy"BRRPATH_SEP_STR"A.a",  "yy", "A",  "a"),NEWCASE("yy"BRRPATH_SEP_STR"A.bb",  "yy", "A",  "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR"BB", "yy", "BB", NULL),NEWCASE("yy"BRRPATH_SEP_STR"BB.", "yy", "BB", ""),NEWCASE("yy"BRRPATH_SEP_STR"BB.a", "yy", "BB", "a"),NEWCASE("yy"BRRPATH_SEP_STR"BB.bb", "yy", "BB", "bb"),

		NEWCASE(""BRRPATH_SEP_STR"",      "",   "",   NULL),NEWCASE("."BRRPATH_SEP_STR"",      ".",   "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"",      ".a",   "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"",      ".bb",   "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"",     "A",  "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"",     "A.",  "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"",     "A.a",  "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"",     "A.bb",  "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"",    "BB", "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"",    "BB.", "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"",    "BB.a", "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"",    "BB.bb", "",    NULL),
		NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"",     "",   "",   NULL),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR".",     "",    "",   ""),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR".a",     "",     "",   "a"),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR".bb",     "",      "",   "bb"),
		NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"A",    "",   "A",  NULL),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.",    "",    "A",  ""),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.a",    "",     "A",  "a"),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.bb",    "",      "A",  "bb"),
		NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB",   "",   "BB", NULL),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.",   "",    "BB", ""),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.a",   "",     "BB", "a"),NEWCASE(""BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.bb",   "",      "BB", "bb"),
		NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"",    "x",  "",   NULL),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR".",    "x",   "",   ""),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR".a",    "x",    "",   "a"),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR".bb",    "x",     "",   "bb"),
		NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A",   "x",  "A",  NULL),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.",   "x",   "A",  ""),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.a",   "x",    "A",  "a"),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.bb",   "x",     "A",  "bb"),
		NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB",  "x",  "BB", NULL),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.",  "x",   "BB", ""),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.a",  "x",    "BB", "a"),NEWCASE("x"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.bb",  "x",     "BB", "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"",   "yy", "",   NULL),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR".",   "yy",  "",   ""),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR".a",   "yy",   "",   "a"),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR".bb",   "yy",    "",   "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A",  "yy", "A",  NULL),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.",  "yy",  "A",  ""),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.a",  "yy",   "A",  "a"),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"A.bb",  "yy",    "A",  "bb"),
		NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB", "yy", "BB", NULL),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.", "yy",  "BB", ""),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.a", "yy",   "BB", "a"),NEWCASE("yy"BRRPATH_SEP_STR""BRRPATH_SEP_STR"BB.bb", "yy",    "BB", "bb"),

		NEWCASE(""BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ""BRRPATH_SEP_STR"n",    "",   NULL),NEWCASE("."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    "."BRRPATH_SEP_STR"n",    "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ".a"BRRPATH_SEP_STR"n",    "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",    ".bb"BRRPATH_SEP_STR"n",    "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A"BRRPATH_SEP_STR"n",   "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A."BRRPATH_SEP_STR"n",   "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A.a"BRRPATH_SEP_STR"n",   "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",   "A.bb"BRRPATH_SEP_STR"n",   "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB"BRRPATH_SEP_STR"n",  "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB."BRRPATH_SEP_STR"n",  "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB.a"BRRPATH_SEP_STR"n",  "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"n"BRRPATH_SEP_STR"",  "BB.bb"BRRPATH_SEP_STR"n",  "",    NULL),
		NEWCASE(""BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ""BRRPATH_SEP_STR"mm",   "",   NULL),NEWCASE("."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   "."BRRPATH_SEP_STR"mm",   "", NULL),NEWCASE(".a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ".a"BRRPATH_SEP_STR"mm",   "",  NULL),NEWCASE(".bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",   ".bb"BRRPATH_SEP_STR"mm",   "",    NULL),
		NEWCASE("A"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A"BRRPATH_SEP_STR"mm",  "",   NULL),NEWCASE("A."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A."BRRPATH_SEP_STR"mm",  "", NULL),NEWCASE("A.a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A.a"BRRPATH_SEP_STR"mm",  "",  NULL),NEWCASE("A.bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"",  "A.bb"BRRPATH_SEP_STR"mm",  "",    NULL),
		NEWCASE("BB"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB"BRRPATH_SEP_STR"mm", "",   NULL),NEWCASE("BB."BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB."BRRPATH_SEP_STR"mm", "", NULL),NEWCASE("BB.a"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB.a"BRRPATH_SEP_STR"mm", "",  NULL),NEWCASE("BB.bb"BRRPATH_SEP_STR"mm"BRRPATH_SEP_STR"", "BB.bb"BRRPATH_SEP_STR"mm", "",    NULL),
	};
#undef NEWCASE
	BRRTIL_ITERARR(brrsz, i, cases) {
#define CSESTR "case %zu ('%s': '%s', '%s', '%s')"
#define CSEARGS i+1, cse.input, BRRTIL_NULSTR(cse.correct_directory), BRRTIL_NULSTR(cse.correct_base_name), BRRTIL_NULSTR(cse.correct_extension)
#define CSEBONUS "('%s', '%s', '%s')"
#define CSEBONUSARGS err_dir, err_bse, err_ext
		struct testcase cse = cases[i];
		char err_dir[BRRPATH_MAX_PATH + 1] = {0};
		char err_bse[BRRPATH_MAX_NAME + 1] = {0};
		char err_ext[BRRPATH_MAX_NAME + 1] = {0};
		int e = brrpath_split(&cse.result_directory, &cse.result_base_name, &cse.result_extension, cse.input);
		BRRTEST_ASSERT(e != -1, MSG(CSESTR" failed to split path : %s", CSEARGS, strerror(errno)));
		if (BRRTIL_XOR(cse.correct_directory, cse.result_directory.opaque)) {
			e = 1;
		} else if (cse.correct_directory && 0 != strcmp(cse.correct_directory, cse.result_directory.opaque)) {
			e = 2;
		} else if (BRRTIL_XOR(cse.correct_base_name, cse.result_base_name.opaque)) {
			e = 3;
		} else if (cse.correct_base_name && 0 != strcmp(cse.correct_base_name, cse.result_base_name.opaque)) {
			e = 4;
		} else if (BRRTIL_XOR(cse.correct_extension, cse.result_extension.opaque)) {
			e = 5;
		} else if (cse.correct_extension && 0 != strcmp(cse.correct_extension, cse.result_extension.opaque)) {
			e = 6;
		}
		if (e) {
			memcpy(err_dir, BRRTIL_NULSTR((char *)cse.result_directory.opaque), cse.result_directory.opaque?cse.result_directory.length + 1:6);
			memcpy(err_bse, BRRTIL_NULSTR((char *)cse.result_base_name.opaque), cse.result_base_name.opaque?cse.result_base_name.length + 1:6);
			memcpy(err_ext, BRRTIL_NULSTR((char *)cse.result_extension.opaque), cse.result_extension.opaque?cse.result_extension.length + 1:6);
		}
		brrstg_delete(&cse.result_directory);
		brrstg_delete(&cse.result_base_name);
		brrstg_delete(&cse.result_extension);
#if 0
		BRRLOG_NOR(CSESTR" "CSEBONUS, CSEARGS, CSEBONUSARGS);
#else
		BRRTEST_ASSERT(e != 1 && e != 2, MSG(CSESTR" failed %i. Expected directory '%s', got '%s' "CSEBONUS, CSEARGS, e, BRRTIL_NULSTR(cse.correct_directory), err_dir, CSEBONUSARGS));
		BRRTEST_ASSERT(e != 3 && e != 4, MSG(CSESTR" failed %i. Expected base name '%s', got '%s' "CSEBONUS, CSEARGS, e, BRRTIL_NULSTR(cse.correct_base_name), err_bse, CSEBONUSARGS));
		BRRTEST_ASSERT(e != 5 && e != 6, MSG(CSESTR" failed %i. Expected extension '%s', got '%s' "CSEBONUS, CSEARGS, e, BRRTIL_NULSTR(cse.correct_extension), err_ext, CSEBONUSARGS));
#endif
#undef CSESTR
#undef CSEARGS
#undef CSEBONUS
#undef CSEBONUSARGS
	}
	return "";
}
BRRTEST_SUITE(path, 0
	,PATH_COMBINE
	,PATH_SPLIT
)

void BRRCALL
brrtools_brrpath_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed)
{
	BRRTEST_RUN_SUITE(path);
	if (total) {
		*total += BRRTEST_SUITE_TOTAL(path);
	}
	if (ran) {
		*ran += BRRTEST_SUITE_RUN(path);
	}
	if (skipped) {
		*skipped += BRRTEST_SUITE_SKIP(path);
	}
	if (succeeded) {
		*succeeded += BRRTEST_SUITE_SUCCEED(path);
	}
	if (failed) {
		*failed += BRRTEST_SUITE_FAIL(path);
	}
}
