#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrtest.h"
#include "brrtools/brrlog.h"
#include "brrtools/brrnum.h"
#include "brrtools/brrpath.h"

BRRTEST_NEW_TEST(PATH_COMBINE) {
	struct testcase {
		const brrpath_components_t input;
		const brrstringr_t correct;
		brrstringr_t result;
	};
#define NEWCASE(_c_, _d_, _b_, _e_) {{.directory=brrstringr_literal(_d_), .base_name=brrstringr_literal(_b_), .extension=brrstringr_literal(_e_)}, .correct=brrstringr_literal(_c_), {0}}
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
	BRR_ITER_ARRAY(brrsz, i, cases) {
#define CSESTR "case %zu ('%s': '%s', '%s', '%s')"
#define CSEARGS i+1, cse.correct.cstr, BRR_NULL_STR(cse.input.directory.cstr), BRR_NULL_STR(cse.input.base_name.cstr), BRR_NULL_STR(cse.input.extension.cstr)
		static char err[BRRPATH_MAX_PATH+1] = {0};
		struct testcase cse = cases[i];
		int e = 0;
		BRRTEST_ASSERT(!(e = brrpath_combine(&cse.input, &cse.result)), brrtools_test_message(CSESTR" failed %d. Could not combine path : %s", CSEARGS, e, strerror(errno)));
		if ((e = brrstringr_compare(&cse.correct, &cse.result, brrtrue)))
			memcpy(err, BRR_NULL_STR(cse.result.cstr), cse.result.cstr?cse.result.length+1:6);
		brrstringr_free(&cse.result);
		BRRTEST_ASSERT(!e, brrtools_test_message(CSESTR" failed %d. Expected '%s', got '%s'", CSEARGS, e, cse.correct.cstr, err));
#undef CSESTR
#undef CSEARGS
	}
	return "";
}
BRRTEST_NEW_TEST(PATH_SPLIT) {
	struct testcase {
		const brrstringr_t input;
		const brrpath_components_t correct_components;
		brrpath_components_t result_components;
	};
#define NEWCASE(_i_, _d_, _b_, _e_) {brrstringr_literal(_i_), {.directory=brrstringr_literal(_d_), .base_name=brrstringr_literal(_b_), .extension=brrstringr_literal(_e_)}, {0}}
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
	BRR_ITER_ARRAY(brrsz, i, cases) {
#define CSESTR "case %zu ('%s': '%s', '%s', '%s')"
#define CSEARGS i+1, cse.input.cstr, BRR_NULL_STR(cse.correct_components.directory.cstr), BRR_NULL_STR(cse.correct_components.base_name.cstr), BRR_NULL_STR(cse.correct_components.extension.cstr)
#define CSEBONUS "('%s', '%s', '%s')"
#define CSEBONUSARGS err_dir, err_base, err_ext
		struct testcase cse = cases[i];
		char err_dir[BRRPATH_MAX_PATH + 1] = {0};
		char err_base[BRRPATH_MAX_PATH + 1] = {0};
		char err_ext[BRRPATH_MAX_PATH + 1] = {0};
		int e = brrpath_split(&cse.result_components, &cse.input);
		BRRTEST_ASSERT(!e, brrtools_test_message(CSESTR" failed to split path : %s", CSEARGS, strerror(errno)));
		if (brrstringr_compare(&cse.correct_components.directory, &cse.result_components.directory, brrtrue))
			e = 1;
		else if (brrstringr_compare(&cse.correct_components.base_name, &cse.result_components.base_name, brrtrue))
			e = 2;
		else if (brrstringr_compare(&cse.correct_components.extension, &cse.result_components.extension, brrtrue))
			e = 3;
		if (e) {
			memcpy(err_dir, cse.result_components.directory.cstr, cse.result_components.directory.length + 1);
			memcpy(err_base, cse.result_components.base_name.cstr, cse.result_components.base_name.length + 1);
			memcpy(err_ext, cse.result_components.extension.cstr, cse.result_components.extension.length + 1);
		}
		brrpath_components_free(&cse.result_components);
#if 0
		BRRLOG_NOR(CSESTR" "CSEBONUS, CSEARGS, CSEBONUSARGS);
#else
		BRRTEST_ASSERT(e != 1, brrtools_test_message(CSESTR" failed %i. Expected directory '%s', got '%s' "CSEBONUS, CSEARGS, e, BRR_NULL_STR(cse.correct_components.directory.cstr), err_dir, CSEBONUSARGS));
		BRRTEST_ASSERT(e != 2, brrtools_test_message(CSESTR" failed %i. Expected base name '%s', got '%s' "CSEBONUS, CSEARGS, e, BRR_NULL_STR(cse.correct_components.base_name.cstr), err_base, CSEBONUSARGS));
		BRRTEST_ASSERT(e != 3, brrtools_test_message(CSESTR" failed %i. Expected extension '%s', got '%s' "CSEBONUS, CSEARGS, e, BRR_NULL_STR(cse.correct_components.extension.cstr), err_ext, CSEBONUSARGS));
#endif
#undef CSESTR
#undef CSEARGS
#undef CSEBONUS
#undef CSEBONUSARGS
	}
	return "";
}
BRRTEST_NEW_SUITE(path, brrfalse
	,PATH_COMBINE
	,PATH_SPLIT
)
