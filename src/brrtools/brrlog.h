#ifndef brrtools_brrlog_h
#define brrtools_brrlog_h

#include <stdarg.h>

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstringr.h>

BRR_cppstart

#if !defined(_WIN32)
#define _brrlog_can_style 1
#else
#define _brrlog_can_style 0
#endif

#define _brrlog_sts(_X_)\
	_X_(st, normal,        0,  "0")\
	_X_(st, bold,          1,  "1")\
	_X_(st, dim,           2,  "2")\
	_X_(st, italics,       3,  "3")\
	_X_(st, under,         4,  "4")\
	_X_(st, blink,         5,  "5")\
	_X_(st, fastblink,     6,  "6")\
	_X_(st, reverse,       7,  "7")\
	_X_(st, conceal,       8,  "8")\
	_X_(st, strikeout,     9,  "9")\
	_X_(st, fraktur,      10, "20")\
	_X_(st, nobold,       11, "21")\
	_X_(st, nobright,     12, "22")\
	_X_(st, noitalics,    13, "23")\
	_X_(st, nounder,      14, "24")\
	_X_(st, noblink,      15, "25")\
	_X_(st, noreverse,    16, "27")\
	_X_(st, reveal,       17, "28")\
	_X_(st, nostrikeout,  18, "29")\
	_X_(st, frame,        19, "51")\
	_X_(st, circle,       20, "52")\
	_X_(st, over,         21, "53")\
	_X_(st, noframe,      22, "54")\
	_X_(st, noover,       23, "55")\
	_X_(st, iunder,       24, "60")\
	_X_(st, idoubleunder, 25, "61")\
	_X_(st, iover,        26, "62")\
	_X_(st, idoubleover,  27, "63")\
	_X_(st, istress,      28, "64")\
	_X_(st, ioff,         29, "65")\

#define _brrlog_cols(_X_)\
	_X_(col, normal,        0, "39",  "49")\
	_X_(col, black,         1, "30",  "40")\
	_X_(col, red,           2, "31",  "41")\
	_X_(col, green,         3, "32",  "42")\
	_X_(col, yellow,        4, "33",  "43")\
	_X_(col, blue,          5, "34",  "44")\
	_X_(col, magenta,       6, "35",  "45")\
	_X_(col, cyan,          7, "36",  "46")\
	_X_(col, white,         8, "37",  "47")\
	_X_(col, grey,          9, "90", "100")\
	_X_(col, lightred,     10, "91", "101")\
	_X_(col, lightgreen,   11, "92", "102")\
	_X_(col, lightyellow,  12, "93", "103")\
	_X_(col, lightblue,    13, "94", "104")\
	_X_(col, lightmagenta, 14, "95", "105")\
	_X_(col, lightcyan,    15, "96", "106")\
	_X_(col, lightwhite,   16, "97", "107")\

#define _brrlog_fns(_X_)\
	_X_(fn, normal, 0, "10")\
	_X_(fn, 1,      1, "11")\
	_X_(fn, 2,      2, "12")\
	_X_(fn, 3,      3, "13")\
	_X_(fn, 4,      4, "14")\
	_X_(fn, 5,      5, "15")\
	_X_(fn, 6,      6, "16")\
	_X_(fn, 7,      7, "17")\
	_X_(fn, 8,      8, "18")\
	_X_(fn, 9,      9, "19")\

#define _brrlog_enum_op(_t_, _n_, _i_, ...) brrlog_##_t_##_##_n_ = _i_,
typedef enum brrlog_st
{
	brrlog_st_last         = -2,
	brrlog_st_unset        = -1,
	_brrlog_sts(_brrlog_enum_op)
	brrlog_st_count,
} brrlog_st_t;
typedef enum brrlog_col
{
	brrlog_col_last         = -2,
	brrlog_col_unset        = -1,
	_brrlog_cols(_brrlog_enum_op)
	brrlog_col_count,
} brrlog_col_t;
typedef enum brrlog_fn
{
	brrlog_fn_last   = -2,
	brrlog_fn_unset  = -1,
	_brrlog_fns(_brrlog_enum_op)
	brrlog_fn_count,
} brrlog_fn_t;
#undef _brrlog_enum_op

#define BRRLOG_MAX_STYLE_TOKEN 4

#define _brrlog_max_st 4
#define _brrlog_max_sts(_X_) _X_(0) _X_(1) _X_(2) _X_(3)
#ifndef _brrlog_keep_gens
# undef _brrlog_max_sts
#endif
#if !_brrlog_can_style
#define _brrlog_max_style 0;
#else
#define ST_PFX "\x1b["
#define ST_SFX "m"
#define ST_CLR ST_PFX "0" ST_SFX
#define _brrlog_max_style ((_brrlog_max_st + 3) * 4 + sizeof(ST_PFX) - 1 + sizeof(ST_SFX) - 1)
#endif
typedef struct brrlog_style
{
#if _brrlog_can_style
	char _s[_brrlog_max_style + 1];
	brrsz _l;
#endif
	brrlog_st_t st[_brrlog_max_st];
	brrlog_col_t fg;
	brrlog_col_t bg;
	brrlog_fn_t fn;
} brrlog_style_t;

/* Copies values from 'b' that are unset in 'a' */
BRRAPI brrlog_style_t BRRCALL
brrlog_style_init(brrlog_style_t source);
BRRAPI brrlog_style_t BRRCALL
brrlog_style_or(brrlog_style_t a, brrlog_style_t b);

typedef struct brrlog_priority
{
	const char *pfx;
#if _brrlog_can_style
	brrlog_style_t style;
#endif
	brrsz _pfxl;
} brrlog_priority_t;

BRRAPI int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_style_t style);
BRRAPI void BRRCALL
brrlog_priority_free(brrlog_priority_t *const pri);

/*
 *  brru4 : max_nest;
 *  int : min_label;
 *  int : max_label;
 *  brru2 : max_priorities;
 *
 *  brru2 : style_enabled;
 *  brru2 : prefixes_enabled;
 *  brru2 : newlines_enabled;
 *  brru2 : verbose;
 *  brru2 : debug;
 *  brru2 : flush_enabled;
 *  brru2 : flush_always;
 *
 *  brrsz : max_log;
 *  brru2 : max_prefix;
 *  brrlog_priority_t : default_priority;
 * */
typedef struct brrlog_cfg
{
	brru4 max_nest; /* Maximal style nest */
	int min_label; /* Minimum label that will log. */
	int max_label; /* Maximum label that will log. */
	brru2 max_priorities;
	brru2 _npri;

	union {
		brru2 settings;
		struct {
			brru2 style_enabled:1;    /* Whether output log styling is enabled at all; if not, then any
										 styled strings are simply removed from the output (Windows never
										 styles regardless of this).
										 Default 1. */
			brru2 prefixes_enabled:1; /* Disables label prefixes from ever printing when set to 0.
										 Default 1. */
			brru2 newlines_enabled:1; /* Disables automatic newlines from ever printing when set to 0.
										 Default 1. */
			brru2 verbose:1;          /* Log FILE, FUNC (if supported), and LINE information with each log.
										 Default 0. */
			brru2 debug:1;            /* Log every label, every time, regardless of any other determining factors.
										 Invalid labels get logged with a default priority.
										 Default 0. */
			brru2 flush_enabled:1;    /* Flush automatically when logging destination changes.
										 Default 1. */
			brru2 flush_always:1;     /* Flush output streams after every log (slow); useful for debugging
										 when the OS's default flushing is too infrequent; will flush
										 regardless of 'flush_enabled' setting.
										 Default 0. */
		};
	};

	brrsz max_log; /* Maximum output log length, including styling characters; may be 0 to indicating no maximum. */
	brru2 max_prefix; /* Maximum length to allocate for a prefix, without styling characters; no special behavior when 0. */
	brrlog_priority_t def_pri; /* Priority to log in the case of trying to log a label that doesn't exist. */

	const char *_sty_open;
	const char *_sty_close;
	int *_lbl;
	brrlog_priority_t *_pri;
	char *_log_buf;
	brru4 _sty_len;
	int _init;
} brrlog_cfg_t;
extern const brrlog_cfg_t brrlog_default_cfg;
extern brrlog_cfg_t brrlog_global_cfg;

BRRAPI int BRRCALL
brrlog_init(brrlog_cfg_t cfg, const char *const style_open, const char *const style_close);
BRRAPI void BRRCALL
brrlog_deinit(void);
/* Updates and confirms current brrlog configuration */
BRRAPI int BRRCALL
brrlog_validate(void);

BRRAPI brrsz BRRCALL
brrlog_priority_index(int label);
BRRAPI int BRRCALL
brrlog_priority_mod(int label, brrlog_priority_t cfg);
BRRAPI int BRRCALL
brrlog_priority_delete(int label);

BRRAPI int BRRCALL
brrlog_set_maxlog(brrsz new_max);

typedef enum brrlog_out_type
{
	brrlog_out_none = 0,
	brrlog_out_stream = 1,
	brrlog_out_buffer = 2,
	_brrlog_out_min = brrlog_out_stream,
	_brrlog_out_max = brrlog_out_buffer,
} brrlog_out_type_t;
typedef struct brrlog_out
{
	brrlog_out_type_t type;
	void *dst;
} brrlog_out_t;
extern const brrlog_out_t brrlog_stdout;
extern const brrlog_out_t brrlog_stderr;

typedef struct brrlog_parms
{
	int label;
	int print_newline;
	int print_prefix;
	int verbose;
} brrlog_parms_t;
BRRAPI brrsz BRRCALL
brrlogv(brrlog_out_t dst, brrlog_parms_t parms, const char *const message, va_list lptr);
/* Logs a message according to printf-fmt 'message' and any printf-vargs passed, with additional
 * functionality & settings, returning the number of characters actually logged, or BRRSZ_MAX on error.
 */
BRRAPI brrsz BRRCALL
brrlog(brrlog_out_t dst, brrlog_parms_t parms, const char *const message, ...);

BRR_cppend

#ifndef _brrlog_keep_gens
# undef _brrlog_sts
# undef _brrlog_cols
# undef _brrlog_fns
#endif

#endif /* brrtools_brrlog_h */
