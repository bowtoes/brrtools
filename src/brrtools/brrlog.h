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

/*
typedef struct priority_prefix {
	const char *const prefix_str;
	brrstyle_t style;
} priority_prefix_t;
typedef struct priority_configuration {
	priority_prefix_t prefix; // (!24:!)
	int style_disabled;
	brrstyle_t default_style;
} priority_configuration_t;
brrlog_style_cfg(44, style_configuration_t):
brrlog_pri_cfg(9, prioriy_configuration_t);
brrlog(9, ...);

brrlog(9, "This is some unstyled, (!i:styled, and (!u:nested styled %s!)!)", "text");
This:
 brrlog(0, "Some (!s:styled!) text", ...);
would be valid and get styled, however this:
 brrlog(0, "Some ( !s:unstyled!) text", ...);
or
 brrlog(0, "Some (!s:unstyled! ) text", ...);
would be invalid, and be printed verbatim.

#define NOR 0 // Priority preset, defined through a function call
#define EXTRA_INFO "4" // Style preset, defined through a function call
brrlog(NOR, "Processing input (!"EXTRA_INFO":%*zu / %zu!) ", state->stats.n_input_digits, i+1, state->n_inputs);
// Maybe add a 'preproc_styles'; pass '$i' in place of style token, and the first arg is an array of the ones
// used? maybe?
brrlog(NOR, "Processing input (!$i:%*zu / %zu!) ", {EXTRA_INFO}, state->stats.n_input_digits, i+1, state->n_inputs);

Or maybe something like:

#define brrlog_color_black        30
#define brrlog_color_red          31
#define brrlog_color_green        32
#define brrlog_color_yellow       33
#define brrlog_color_blue         34
#define brrlog_color_magenta      35
#define brrlog_color_cyan         36
#define brrlog_color_white        37
#define brrlog_color_darkgrey     90
#define brrlog_color_lightred     91
#define brrlog_color_lightgreen   92
#define brrlog_color_lightyellow  93
#define brrlog_color_lightblue    94
#define brrlog_color_lightmagenta 95
#define brrlog_color_lightcyan    96
#define brrlog_color_lightwhite   97

#define _brrlog_code(_fg_, _bg_, _st_, _fn_) "\x1b["#_st_";"#_fg_";"#_bg_";"#_fn_"m"
#define brrlog_style_clear _brrlog_code(0, 39, 49, 10)
#define brrlog_lstyle(_s_, _fg_, _bg_, _st_, _fn_) _brrlog_code(_fg_, _bg_, _st_, _fn_) _s_ brrlog_style_clear

I still plan to have the normal 'brrlog[np]' macros, but they should be much fewer in number now.
 * */

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
	brrlog_col_darkgrey     =  9,
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

typedef struct brrlog_priority
{
	const char *pfx;
	brrlog_style_t style;
	brrsz _pfxl;
} brrlog_priority_t;

typedef brrlog_col_t brrlog_fg_t;
typedef brrlog_col_t brrlog_bg_t;

BRRAPI int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_style_t style, brrsz max_prefix);
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
	brrlog_priority_t default_priority; /* Priority to log in the case of trying to log a label that doesn't exist. */

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
	const char *const file;
	const char *const function;
	brrsz line;
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
