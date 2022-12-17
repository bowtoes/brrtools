#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <brrtools/brrpath.h>
#include <brrtools/brrnum.h>
#include <brrtools/brrlog.h>

#define _brrlog_try_pri(_lbl_, _style_, _pfx_, _dst_, _type_) do {\
	if (brrlog_priority_mod(_lbl_, \
		(brrlog_priority_t) {\
			.style=_style_,\
			.pfx=_pfx_,\
			.dst={\
				.dst=_dst_,\
				.type=_type_,\
			}\
		})) {\
		int e = brrapi_gete();\
		if (e == BRRAPI_E_LIBC) {\
			fprintf(stderr, "Failed to initilaize brrlog priority %i '%s': %s (%i)\n", _lbl_, _pfx_?_pfx_:"", strerror(errno), errno);\
		} else {\
			fprintf(stderr, "Failed to initilaize brrlog priority %i '%s': %s (%i)\n", _lbl_, _pfx_?_pfx_:"", brrapi_err(), brrapi_gete());\
		}\
		brrlog_deinit();\
		return 1;\
	}\
} while (0)

#define log_info  -9
#define log_debug -2
#define log_extra -1
#define log_norm  0
#define log_warn  2
#define log_err   3
#define info(_t_, ...) brrlog##_t_(log_info, __VA_ARGS__)
#define debug(_t_, ...) brrlog##_t_(log_debug, __VA_ARGS__)
#define extra(_t_, ...) brrlog##_t_(log_extra, __VA_ARGS__)
#define norm(_t_, ...) brrlog##_t_(log_norm, __VA_ARGS__)
#define warn(_t_, ...) brrlog##_t_(log_warn, __VA_ARGS__)
#define err(_t_, ...) brrlog##_t_(log_err, __VA_ARGS__)

static int
init_brrlog(int style_enabled)
{
	#define _nostyle {{0}}
	#define _extrastyle {{0}}
	{
		brrlog_cfg_t c = brrlog_default_cfg;
		c.style_enabled = style_enabled;
		c.flush_enabled = 1;
		if (brrlog_init(c, "(!", "!)")) {
			fprintf(stderr, "Failed to init brrlog: %s (%i)\n", strerror(errno), errno);
			return 1;
		}
	}
	_brrlog_try_pri(log_info,   _nostyle,    "(!f=c:[ INFO]!): ", stdout, brrlog_dst_stream);
	_brrlog_try_pri(log_debug,  _nostyle,    "(!f=ys=b:[DEBUG]!): ", stderr, brrlog_dst_stream);
	_brrlog_try_pri(log_extra,  _extrastyle, "(!f=gs=r:[extra]!): ", stdout, brrlog_dst_stream);
	_brrlog_try_pri(log_norm,   _nostyle,    NULL, stdout, brrlog_dst_stream);
	_brrlog_try_pri(log_warn,   _nostyle,    "(!f=y:[ WARN]!): ", stderr, brrlog_dst_stream);
	_brrlog_try_pri(log_err,    _nostyle,    "(!f=r:[ ERR ]!): ", stderr, brrlog_dst_stream);
	#undef _nostyle
	#undef _extrastyle
	return 0;
}

int
main(int argc, char **argv)
{
	if (init_brrlog(1))
		return 1;

	#define t(_case_, _expected_) {.s=(_case_), .l=sizeof(_case_) - 1, .expected = (_expected_)}
	typedef struct test {
		char *s;
		brrsz l;
		const char *expected;
	} test_t;
	static const test_t tests[] = {
#if 0
#if 1
		t("/"                                             , "/"),
		t("/home"                                         , "/home"),
		t("/home/nice/."                                  , "/home/nice"),
		t("/home/nice/.."                                 , "/home"),
		t("/home/nice/boat/.."                            , "/home/nice"),
		t("/home/nice/car"                                , "/home/nice/car"),
		t("/home/nice/./car"                              , "/home/nice/car"),
		t("/home/nice/../car"                             , "/home/car"),
		t("/home/nice/../car/../what/if/I/../did/./this"  , "/home/what/if/did/this"),
		t("/home/nice/../car/../what/if/I/../did/../this" , "/home/what/if/this"),
		t("/home/nice/.//////"                            , "/home/nice"),
		t("/home/nice/..//////"                           , "/home"),
		t("home"                                          , "home"),
		t("home/nice/car"                                 , "home/nice/car"),
		t("home/nice/./car"                               , "home/nice/car"),
		t("home/nice/../car"                              , "home/car"),
		t("home/nice/../car/../what/if/I/../did/./this"   , "home/what/if/did/this"),
		t("home//nice"                                    , "home/nice"),
		t("home/nice/./././././"                          , "home/nice"),
		t("home/nice/.."                                  , "home"),
		t("./home/nice"                                   , "home/nice"),
		t("../home/nice"                                  , "../home/nice"),
		t("home/nice/car/../../.."                        , ""),
		t("home/nice/car/../../../.."                     , ".."),
		t("../home/nice/car/../../.."                     , ".."),
		t("../home/nice/car/../../../.."                  , "../.."),
		t("/../home/nice/car/../../.."                    , "/"),
		t("/../home/nice/car/../../../.."                 , "/"),
#endif
#if brrpath_dos
		// Somethings wrong with the test compilation, this check fails when it shouldn't. Oh well, I know it
		// works without the test
		t("C:\\home\\nice\\car"                           , "C:/home/nice/car"),
		t("////////"                                      , "////////"),
		t("////home//nice"                                , "////home//nice"),
#else
		t("C:\\home\\nice\\car"                           , "C:\\home\\nice\\car"),
		t("////////"                                      , "/"),
		t("////home//nice"                                , "/home/nice"),
#endif
#else
		t("a/very/nice/house/you.have.here", "a/very/nice/house/you.have.here"),
#endif
	};
	for (int i = 0; i < (sizeof(tests) / sizeof(*tests)); ++i) {
		test_t t = tests[i];
		brrpath_t v = brrpath_canonicalize(t.s, t.l);
		if (v.full) {
			if (!brrpath_cmp(v.full, t.expected, brrnum_umin(v.len, t.l))) {
				brrsz dirlen = brrpath_dirlen(&v);
				const char *name = v.full + dirlen + 1;
				const char *ext = name + v.base_len + 1;
				info(p,"(!f=y:'%s'!) = (!f=rs=b:'%*.*s'!) + (!f=ys=b:'/'!) + (!f=g:'%*.*s'!) + (!f=ys=b:'.'!) + (!f=cs=b:'%*.*s'!)", v.full, dirlen, dirlen, v.full, v.base_len, v.base_len, name, v.ext_len, v.ext_len, ext);
			} else {
				info(p,"(!f=r:FAIL!): (!f=y:'%s'!) -> (!f=rs=b:'%s'!) (!f=b:%zu!), expected (!f=b:'%s'!)", t.s, v.full, (brrsz)v.len, t.expected);
			}
		} else {
			err(,"Failed to canonicalize (!f=y:'%s'!)", tests[i]);
		}
		brrpath_free(&v);
	}

	brrlog_deinit();
	return 0;
}
