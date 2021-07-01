#include "tests/stg.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrstg.h"
#include "brrtools/brrtest.h"
#include "brrtools/brrtil.h"
#include "brrtools/brrtypes.h"

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

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

void BRRCALL
brrtools_brrstg_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed)
{
	BRRTEST_RUN_SUITE(stg);
	if (total) {
		*total += BRRTEST_SUITE_TOTAL(stg);
	}
	if (ran) {
		*ran += BRRTEST_SUITE_RUN(stg);
	}
	if (skipped) {
		*skipped += BRRTEST_SUITE_SKIP(stg);
	}
	if (succeeded) {
		*succeeded += BRRTEST_SUITE_SUCCEED(stg);
	}
	if (failed) {
		*failed += BRRTEST_SUITE_FAIL(stg);
	}
}
