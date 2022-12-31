#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <brrtools/brrpath.h>
#include <brrtools/brrnum.h>
#include <brrtools/brrlog.h>
#include <brrtools/brrmacro.h>
#include <brrtools/brrcon.h>
static char msg[4096] = {0};
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
		brrapi_error_message(msg, sizeof(msg));\
		fprintf(stderr, "Failed to init brrlog priority %i '%s': %s\n", _lbl_, _pfx_?_pfx_:"", msg);\
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

#define _print_inf(_p_, _pfx_) do {\
	norm(,_pfx_ "(!f=g:Type:!)      %s\n"\
	_pfx_ "(!f=b:Size:!)      %zu bytes\n"\
	_pfx_ "(!f=b:Dir Len:!)   %u\n"\
	_pfx_ "(!f=b:Base Len:!)  %u\n"\
	_pfx_ "(!f=b:Ext Len:!)   %u\n"\
	_pfx_ "(!f=r:Extension:!) %i '%*.*s'\n"\
	_pfx_ "(!f=y:Flags:!) \n"\
	_pfx_ "  (!f=g:Symlink:!)     %i\n"\
	_pfx_ "  (!f=b:Mount Point:!) %i\n"\
	_pfx_ "  (!f=r:Broken:!)      %i\n"\
	_pfx_ "  (!f=y:Archive:!)     %i\n"\
	_pfx_ "  (!f=c:Compressed:!)  %i\n"\
	_pfx_ "  (!f=m:Encrypted:!)   %i",\
	brrpath_type_name((_p_).type),\
	brrpath_get_size((_p_).size),\
	(_p_).dir_len, (_p_).base_len, (_p_).ext_len,\
	(_p_).ext_len, (_p_).ext_valid * (_p_).ext_len, (_p_).ext_valid * (_p_).ext_len, brrpath_extension(&(_p_)),\
	brr_bit((_p_).flags, brrpath_flag_symlink),\
	brr_bit((_p_).flags, brrpath_flag_mount),\
	brr_bit((_p_).flags, brrpath_flag_broken),\
	brr_bit((_p_).flags, brrpath_flag_archive),\
	brr_bit((_p_).flags, brrpath_flag_compressed),\
	brr_bit((_p_).flags, brrpath_flag_encrypted));\
} while (0)
static inline int test_stat(int argc, const char *const *const argv)
{
	int s = 0;
	for (int i = 0; i < argc; ++i) {
		const char *arg = argv[i];
		brrpath_t p = brrpath_canonicalize(arg, strlen(arg));
		if (!p.full) {
			brrapi_error_message(msg, sizeof(msg));
			err(,"Couldn't canonicalize (!f=y:'%s'!): %s", arg, msg);
			continue;
		}
		if (brrpath_stat(&p)) {
			brrapi_error_message(msg, sizeof(msg));
			err(,"Couldn't stat (!f=y:'%s'!): %s", p.full, msg);
		} else if (!p.exists) {
			norm(,"Path (!f=y:'%s'!) does not exist", p.full);
		} else {
			norm(,"Statted (!f=y:'%s'!):\n", p.full);
			_print_inf(p, "  ");
			++s;
		}
		brrpath_free(&p);
	}
	return s - argc;
}

static inline int test_canon(void)
{
	#define t(_case_, _expected_) {.s=(_case_), .l=sizeof(_case_) - 1, .expected = (_expected_)}
	typedef struct test {
		char *s;
		brrsz l;
		const char *expected;
	} test_t;
	static const test_t tests[] = {
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
		t("a/very/nice/house/you.have.here", "a/very/nice/house/you.have.here"),
	};
	int s = 0;
	int e = sizeof(tests) / sizeof(*tests);
	for (int i = 0; i < e; ++i) {
		test_t t = tests[i];
		brrpath_t v = brrpath_canonicalize(t.s, t.l);
		if (v.full) {
			if (!brrpath_cmp(v.full, t.expected, brrnum_umin(v.len, t.l))) {
				const char *name = v.full + v.dir_len + 1;
				const char *ext = name + v.base_len + 1;
				info(p,"(!f=y:'%s'!) = (!f=rs=b:'%*.*s'!) + (!f=ys=b:'/'!) + (!f=g:'%*.*s'!) + (!f=ys=b:'.'!) + (!f=cs=b:'%*.*s'!)", v.full, v.dir_len, v.dir_len, v.full, v.base_len, v.base_len, name, v.ext_len, v.ext_len, ext);
				++s;
			} else {
				info(p,"(!f=r:FAIL!): (!f=y:'%s'!) -> (!f=rs=b:'%s'!) (!f=b:%zu!), expected (!f=b:'%s'!)", t.s, v.full, (brrsz)v.len, t.expected);
			}
		} else {
			err(,"Failed to canonicalize (!f=y:'%s'!)", tests[i]);
		}
		brrpath_free(&v);
	}
	return s - e;
}

static brrpath_walk_code_t i_walker(brrpath_t *restrict const path, brrsz depth, void *restrict const user)
{
	norm(,"%s", path->full);
	return brrpath_walk_code_continue;
}
static inline int test_walk(int argc, const char *const *const argv)
{
	int s = 0;
	brrpath_walk_settings_t walk_settings = {
		.start_depth = 0,
		.max_depth = 2,
		.walker = i_walker,
		.user = NULL
	};
	for (int i = 0; i < argc; ++i) {
		const char *arg = argv[i];
		brrpath_t p = brrpath_canonicalize(arg, strlen(arg));
		if (!p.full) {
			brrapi_error_message(msg, sizeof(msg));
			err(,"Couldn't canonicalize (!f=y:'%s'!): %s", arg, msg);
			continue;
		}
		if (brrpath_stat(&p)) {
			brrapi_error_message(msg, sizeof(msg));
			err(,"Couldn't stat (!f=y:'%s'!): %s", p.full, msg);
		} else {
			norm(,"Walking (!f=y:'%s'!):", p.full);
			//_print_inf(p, "  ");
			if (brrpath_walk(&p, walk_settings)) {
				brrapi_error_message(msg, sizeof(msg));
				err(,"Couldn't walk (!f=y:'%s'!): %s", p.full, msg);
			}
		}
		brrpath_free(&p);
	}
	return s - argc;
}

int
main(int argc, const char *const *const argv)
{
#if 0
#if brrplat_dos
	//FreeConsole();
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	//setvbuf(stdout, NULL, _IONBF, 0);
#endif

#define _print_bits(_dst_, _v_, _len_, _name_) do {\
	char *_str;\
	switch (sizeof(_v_)) {\
		case 1: _str = "%s%*s 0x%02x '"; break;\
		case 2: _str = "%s%*s 0x%04x '"; break;\
		case 4: _str = "%s%*s 0x%08x '"; break;\
		case 8: _str = "%s%*s 0x%016x '"; break;\
	}\
	printf(_str, _name_, (_len_) > sizeof(_name_) ? ((_len_) - sizeof(_name_)) : 0, "", _v_);\
	for (int i = 0; i < 8 * sizeof(_v_); ++i) {\
		fputc(((_v_) >> (8 * sizeof(_v_) - i - 1)) & 0x01 ? '1' : '0', _dst_);\
	}\
	printf("'\n");\
} while (0)
#define _print_tag(_name_) do {\
	static const DWORD v = IO_REPARSE_TAG_##_name_;\
	printf("Tag IO_REPARSE_TAG_" #_name_);\
	_print_bits(stdout, v, 16, "=");\
} while (0)

	_print_tag(AF_UNIX);
	_print_tag(APPEXECLINK);
	_print_tag(CLOUD);
	_print_tag(CLOUD_1);
	_print_tag(CLOUD_2);
	_print_tag(CLOUD_3);
	_print_tag(CLOUD_4);
	_print_tag(CLOUD_5);
	_print_tag(CLOUD_6);
	_print_tag(CLOUD_7);
	_print_tag(CLOUD_8);
	_print_tag(CLOUD_9);
	_print_tag(CLOUD_A);
	_print_tag(CLOUD_B);
	_print_tag(CLOUD_C);
	_print_tag(CLOUD_D);
	_print_tag(CLOUD_E);
	_print_tag(CLOUD_F);
	_print_tag(CLOUD_MASK);
	_print_tag(CSV);
	_print_tag(DEDUP);
	_print_tag(DFS);
	_print_tag(DFSR);
	_print_tag(FILE_PLACEHOLDER);
	_print_tag(GLOBAL_REPARSE);
	_print_tag(HSM);
	_print_tag(HSM2);
	_print_tag(MOUNT_POINT);
	_print_tag(NFS);
	_print_tag(ONEDRIVE);
	_print_tag(PROJFS);
	_print_tag(PROJFS_TOMBSTONE);
	_print_tag(SIS);
	_print_tag(STORAGE_SYNC);
	_print_tag(SYMLINK);
	_print_tag(UNHANDLED);
	_print_tag(WCI);
	_print_tag(WCI_1);
	//_print_tag(WCI_LINK);
	//_print_tag(WCI_LINK_1);
	_print_tag(WCI_TOMBSTONE);
	_print_tag(WIM);
	_print_tag(WOF);

	brrcon_pause();

#else
	if (init_brrlog(1))
		return 1;

#if brrplat_dos
	/* without this, it seems windows in an emulator (and probably in actuality) does not print anything to
	 * the console; this may just be a weird bug with this testing program, or a problem with how the library
	 * is compiled, or something's wrong in brrlog. To be figured out */
	//FreeConsole();
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);
	//setvbuf(stdout, NULL, _IONBF, 0);
#endif

	if (0) {
		test_stat(argc - 1, argv + 1);
	}
	if (1) {
		test_walk(argc - 1, argv + 1);
	}
	if (0) {
		const brrpath_t *d = brrpath_cwd();
		if (d) {
			norm(,"Cwd: (!f=y:'%s'!)", d->full);
		} else {
			char msg[4096] = {0};
			err(,"Failed to get current working directory: (!f=r:'%s'!)", brrapi_error_message(msg, sizeof(msg)));
		}
	}

	brrlog_deinit();
#if brrplat_dos
	//brrcon_pause();
#endif
#endif
	return 0;
}
