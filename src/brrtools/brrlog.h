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

#define BRRLOG_MAX_STYLE_TOKEN 4

typedef enum brrlog_col
{
	brrlog_col_last         = -2,
	brrlog_col_unset        = -1,
	brrlog_col_normal       =  0,
	brrlog_col_black        =  1,
	brrlog_col_red          =  2,
	brrlog_col_green        =  3,
	brrlog_col_yellow       =  4,
	brrlog_col_blue         =  5,
	brrlog_col_magenta      =  6,
	brrlog_col_cyan         =  7,
	brrlog_col_white        =  8,
	brrlog_col_grey         =  9,
	brrlog_col_lightred     = 10,
	brrlog_col_lightgreen   = 11,
	brrlog_col_lightyellow  = 12,
	brrlog_col_lightblue    = 13,
	brrlog_col_lightmagenta = 14,
	brrlog_col_lightcyan    = 15,
	brrlog_col_lightwhite   = 16,
	brrlog_col_count,
} brrlog_col_t;
typedef enum brrlog_fn
{
	brrlog_fn_last   = -2,
	brrlog_fn_unset  = -1,
	brrlog_fn_normal =  0,
	brrlog_fn_1      =  1,
	brrlog_fn_2      =  2,
	brrlog_fn_3      =  3,
	brrlog_fn_4      =  4,
	brrlog_fn_5      =  5,
	brrlog_fn_6      =  6,
	brrlog_fn_7      =  7,
	brrlog_fn_8      =  8,
	brrlog_fn_9      =  9,
	brrlog_fn_count,
} brrlog_fn_t;
typedef enum brrlog_st
{
	brrlog_st_last         = -2,
	brrlog_st_unset        = -1,
	brrlog_st_normal       =  0,
	brrlog_st_bold         =  1,
	brrlog_st_dim          =  2,
	brrlog_st_italics      =  3,
	brrlog_st_under        =  4,
	brrlog_st_blink        =  5,
	brrlog_st_fastblink    =  6,
	brrlog_st_reverse      =  7,
	brrlog_st_conceal      =  8,
	brrlog_st_strikeout    =  9,
	brrlog_st_fraktur      = 10,
	brrlog_st_nobold       = 11,
	brrlog_st_nobright     = 12,
	brrlog_st_noitalics    = 13,
	brrlog_st_nounder      = 14,
	brrlog_st_noblink      = 15,
	brrlog_st_noreverse    = 16,
	brrlog_st_reveal       = 17,
	brrlog_st_nostrikeout  = 18,
	brrlog_st_frame        = 19,
	brrlog_st_circle       = 20,
	brrlog_st_over         = 21,
	brrlog_st_noframe      = 22,
	brrlog_st_noover       = 23,
	brrlog_st_iunder       = 24,
	brrlog_st_idoubleunder = 25,
	brrlog_st_iover        = 26,
	brrlog_st_idoubleover  = 27,
	brrlog_st_istress      = 28,
	brrlog_st_ioff         = 29,
	brrlog_st_count,
} brrlog_st_t;

typedef struct brrlog_style
{
	brrlog_st_t st;
	brrlog_col_t fg;
	brrlog_col_t bg;
	brrlog_fn_t fn;
#if _brrlog_can_style
	brrsz _len;
#endif
} brrlog_style_t;

/* Copies values from 'b' that are unset in 'a' */
BRRAPI brrlog_style_t BRRCALL
brrlog_style_or(brrlog_style_t a, brrlog_style_t b);

typedef enum brrlog_dst_type
{
	_brrlog_dst_min    =  0,

	brrlog_dst_invalid = -1,
	brrlog_dst_stream,
	brrlog_dst_buffer,
	brrlog_dst_count,

	_brrlog_dst_max = brrlog_dst_count - 1,
} brrlog_dst_type_t;

typedef struct brrlog_dst
{
	brrlog_dst_type_t type;
	union {
		void *stream;
		char *buffer;
		void *_loc;
	};
} brrlog_dst_t;

typedef struct brrlog_priority
{
	const char *pfx;
	brrlog_dst_t dst;
#if _brrlog_can_style
	brrlog_style_t style;
#endif
	brrsz _pfxl;
} brrlog_priority_t;

typedef brrlog_col_t brrlog_fg_t;
typedef brrlog_col_t brrlog_bg_t;

BRRAPI int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_dst_t dst, brrlog_style_t style);
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
	brrsz _buf_alloc;
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

typedef struct brrlog_parms
{
	int label;
	int print_newline;
	int print_prefix;
	int verbose;
} brrlog_parms_t;

/* Consumes 'lptr' */
BRRAPI brrsz BRRCALL
brrlogv(brrlog_parms_t parms, const char *const message, va_list lptr);

/* Logs a message according to printf-fmt 'message' and any printf-vargs passed, with additional
 * functionality & settings, returning the number of characters actually logged, or BRRSZ_MAX on error.
 */
BRRAPI brrsz BRRCALL
brrlog(brrlog_parms_t parms, const char *const message, ...);

BRR_cppend

#endif /* brrtools_brrlog_h */
