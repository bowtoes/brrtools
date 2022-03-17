/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef BRRTOOLS_BRRLOG_H
#define BRRTOOLS_BRRLOG_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

_brrcppstart

#define _ex(_t,_N,_n,_i,_d) brrlog_##_t##_##_n = _i,

/* ANSI colors used for logging */
typedef enum brrlog_color {
#define _brrlog_color_gen(_ex_)\
    _ex_(color, LAST,         last,         -1, "Last used color")\
    _ex_(color, NORM,         normal,        0,  "Normal/default color")\
    _ex_(color, BLACK,        black,         1,  "Black")\
    _ex_(color, RED,          red,           2,  "Red")\
    _ex_(color, GREEN,        green,         3,  "Green")\
    _ex_(color, YELLOW,       yellow,        4,  "Yellow")\
    _ex_(color, BLUE,         blue,          5,  "Blue")\
    _ex_(color, MAGENTA,      magenta,       6,  "Magenta")\
    _ex_(color, CYAN,         cyan,          7,  "Cyan")\
    _ex_(color, WHITE,        white,         8,  "White")\
    _ex_(color, DARKGREY,     darkgrey,      9,  "Dark grey/light black")\
    _ex_(color, LIGHTRED,     lightred,     10, "Light red")\
    _ex_(color, LIGHTGREEN,   lightgreen,   11, "Light green")\
    _ex_(color, LIGHTYELLOW,  lightyellow,  12, "Light yellow")\
    _ex_(color, LIGHTBLUE,    lightblue,    13, "Light blue")\
    _ex_(color, LIGHTMAGENTA, lightmagenta, 14, "Light magenta")\
    _ex_(color, LIGHTCYAN,    lightcyan,    15, "Light cyan")\
    _ex_(color, LIGHTWHITE,   lightwhite,   16, "Light white")\

	_brrlog_color_gen(_ex)
	brrlog_color_count,
} brrlog_color_t;
extern const char *brrlog_color_names[brrlog_color_count + 1];
extern const char *brrlog_color_debug_names[brrlog_color_count + 1];
/* Get a string representation of the given color */
BRRAPI const char *BRRCALL brrlog_color_str(brrlog_color_t color);

/* Font types to used for logging */
typedef enum brrlog_font {
#define _brrlog_font_gen(_ex_)\
    _ex_(font, LAST,   last,   -1, "Last used font")\
    _ex_(font, NORMAL, normal,  0,  "Use the normal font")\
    _ex_(font, ONE,    1,       1,  "Use alternate font 1")\
    _ex_(font, TWO,    2,       2,  "Use alternate font 2")\
    _ex_(font, THREE,  3,       3,  "Use alternate font 3")\
    _ex_(font, FOUR,   4,       4,  "Use alternate font 4")\
    _ex_(font, FIVE,   5,       5,  "Use alternate font 5")\
    _ex_(font, SIX,    6,       6,  "Use alternate font 6")\
    _ex_(font, SEVEN,  7,       7,  "Use alternate font 7")\
    _ex_(font, EIGHT,  8,       8,  "Use alternate font 8")\
    _ex_(font, NINE,   9,       9,  "Use alternate font 9")\

	_brrlog_font_gen(_ex)
	brrlog_font_count
} brrlog_font_t;
extern const char *brrlog_font_names[brrlog_font_count + 1];
extern const char *brrlog_font_debug_names[brrlog_font_count + 1];
/* Get a string representation of the given font */
BRRAPI const char *BRRCALL brrlog_font_str(brrlog_font_t font);

/* Text styles used for logging */
typedef enum brrlog_style {
#define _brrlog_style_gen(_ex_)\
    _ex_(style, LAST,         last,         -1, "Last used style")\
    _ex_(style, NORMAL,       normal,        0, "Normal style, text is plain")\
    _ex_(style, BOLD,         bold,          1, "Bold/bright text")\
    _ex_(style, DIM,          dim,           2, "Dim/darker text")\
    _ex_(style, ITALICS,      italics,       3, "Text is italics")\
    _ex_(style, UNDER,        under,         4, "Underlined text")\
    _ex_(style, BLINK,        blink,         5, "Blinking text")\
    _ex_(style, FASTBLINK,    fastblink,     6, "Blinking text, but faster")\
    _ex_(style, REVERSE,      reverse,       7, "Text foreground and background colors are swapped")\
    _ex_(style, CONCEAL,      conceal,       8, "Text isn't drawn")\
    _ex_(style, STRIKEOUT,    strikeout,     9, "Text is striked out")\
    _ex_(style, FRAKTUR,      fraktur,      10, "Medieval text")\
    _ex_(style, NOBOLD,       nobold,       11, "Disable bold text/double underlined text")\
    _ex_(style, NOBRIGHT,     nobright,     12, "Disable text brightening/dimming")\
    _ex_(style, NOITALICS,    noitalics,    13, "Disable italics/fraktur text style")\
    _ex_(style, NOUNDER,      nounder,      14, "Disable text underline")\
    _ex_(style, NOBLINK,      noblink,      15, "Disable text blinking")\
    _ex_(style, NOREVERSE,    noreverse,    16, "Disable text reversal")\
    _ex_(style, REVEAL,       reveal,       17, "Disable text conceal")\
    _ex_(style, NOSTRIKEOUT,  nostrikeout,  18, "Disable text strikeout")\
    _ex_(style, FRAME,        frame,        19, "Text is framed")\
    _ex_(style, CIRCLE,       circle,       20, "Text is encircled")\
    _ex_(style, OVER,         over,         21, "Overlined text")\
    _ex_(style, NOFRAME,      noframe,      22, "Disable text framing/encircling")\
    _ex_(style, NOOVER,       noover,       23, "Disable text overline")\
    _ex_(style, IUNDER,       iunder,       24, "Ideogram underline")\
    _ex_(style, IDOUBLEUNDER, idoubleunder, 25, "Ideogram double-underline")\
    _ex_(style, IOVER,        iover,        26, "Ideogram overline")\
    _ex_(style, IDOUBLEOVER,  idoubleover,  27, "Ideogram double-overline")\
    _ex_(style, ISTRESS,      istress,      28, "Ideogram stressing")\
    _ex_(style, IOFF,         ioff,         29, "Disable ideograms")\

	_brrlog_style_gen(_ex)
	brrlog_style_count,
} brrlog_style_t;
extern const char *brrlog_style_names[brrlog_style_count + 1];
extern const char *brrlog_style_debug_names[brrlog_style_count + 1];
/* Get a string representation of the given style */
BRRAPI const char *BRRCALL brrlog_style_str(brrlog_style_t style);

/* Log levels used for logging.
 * These control when a message is logged, based on the currenlty set minimum
 * and maximum log levels.
 * */
typedef enum brrlog_priority {
#define _brrlog_priority_gen(_ex_)\
    _ex_(priority, LST, last,     -1, "Last used log priority")\
    _ex_(priority, NON, none,      0, "No priority; nothing is logged")\
    _ex_(priority, CRI, critical,  1, "Critical error")\
    _ex_(priority, ERR, error,     2, "Non-fatal error")\
    _ex_(priority, NOR, normal,    3, "Normal log message")\
    _ex_(priority, WAR, warning,   4, "Warning")\
    _ex_(priority, DBG, debug,     5, "Debug message, only logged when debug is enabled")\

	_brrlog_priority_gen(_ex)
	brrlog_priority_count,
} brrlog_priority_t;
extern const char *brrlog_priority_names[brrlog_priority_count + 1];
extern const char *brrlog_priority_debug_names[brrlog_priority_count + 1];
/* Get a string representation of the given level */
BRRAPI const char *BRRCALL brrlog_priority_str(brrlog_priority_t level);

/* Destinations for log messages to go.
 * Only 'stdout' and 'stderr' loggable, for now.
 * TODO loggable to generic 'FILE *'.
 * */
typedef enum brrlog_destination {
#define _brrlog_destination_gen(_ex_)\
	_ex_(destination, LAST, last,   -1, "Last used output destination")\
	_ex_(destination, NULL, null,    0, "No output destination, i.e. nothing is logged")\
	_ex_(destination, STDO, stdout,  1, "Output to standard-out")\
	_ex_(destination, STDE, stderr,  2, "Output to standard-error")\

	_brrlog_destination_gen(_ex)
	brrlog_destination_count,
} brrlog_destination_t;
extern const char *brrlog_destination_names[brrlog_destination_count + 1];
extern const char *brrlog_destination_debug_names[brrlog_destination_count + 1];
/* Get a debug string representation of the given level */
BRRAPI const char *BRRCALL brrlog_priority_dbgstr(brrlog_priority_t level);

#undef _ex

/* Basic log-level structure, storing a log message's priortiy, destination, and
 * prefix.
 * */
typedef struct brrlog_level {
	brrlog_priority_t priority;
	brrlog_destination_t destination;
	const char *prefix;
} brrlog_level_t;
/* Settings used for stylizing log output */
typedef struct brrlog_format {
	brrlog_color_t foreground; /* Format foreground color */
	brrlog_color_t background; /* Format background color */
	brrlog_style_t style;      /* Format text style */
	brrlog_font_t  font;       /* Format format font */
} brrlog_format_t;
/* There is almost certainly a very tricky way to pack these four macros into one */
#define BRRLOG_FORMAT_FONT(_f_, _b_, _s_, _F_) ((brrlog_format_t){_f_, _b_, _s_, _F_})
#define BRRLOG_FORMAT_STYLE(_f_, _b_, _s_)     BRRLOG_FORMAT_FONT(_f_, _b_, _s_, brrlog_font_normal)
#define BRRLOG_FORMAT_BACK(_f_, _b_)           BRRLOG_FORMAT_STYLE(_f_, _b_, brrlog_style_normal)
#define BRRLOG_FORMAT_FORE(_f_)                BRRLOG_FORMAT_BACK(_f_, brrlog_color_normal)

typedef struct brrlog_type {
	brrlog_level_t level;
	brrlog_format_t format;
} brrlog_type_t;

/* Logs of 'critical' priority will be logged with this style by default */
extern BRRAPI brrlog_type_t gbrrlog_type_critical;
/* Logs of 'error' priority will be logged with this style by default */
extern BRRAPI brrlog_type_t gbrrlog_type_error;
/* Logs of 'normal' priority will be logged with this style by default */
extern BRRAPI brrlog_type_t gbrrlog_type_normal;
/* Logs of 'warning' priority will be logged with this style by default */
extern BRRAPI brrlog_type_t gbrrlog_type_warning;
/* Logs of 'debug' priority will be logged with this style by default */
extern BRRAPI brrlog_type_t gbrrlog_type_debug;
/* Format used to clear log styles once log output has finished */
extern BRRAPI brrlog_type_t gbrrlog_type_clear;
/* Settings last-used to log a message */
extern BRRAPI brrlog_type_t gbrrlog_type_last;

#define gbrrlog_level_critical (gbrrlog_type_critical.level)
#define gbrrlog_level_error    (gbrrlog_type_error.level)
#define gbrrlog_level_normal   (gbrrlog_type_normal.level)
#define gbrrlog_level_warning  (gbrrlog_type_warning.level)
#define gbrrlog_level_debug    (gbrrlog_type_debug.level)
#define gbrrlog_level_clear    (gbrrlog_type_clear.level)
#define gbrrlog_level_last     (gbrrlog_type_last.level)

#define gbrrlog_format_critical (gbrrlog_type_critical.format)
#define gbrrlog_format_error    (gbrrlog_type_error.format)
#define gbrrlog_format_normal   (gbrrlog_type_normal.format)
#define gbrrlog_format_warning  (gbrrlog_type_warning.format)
#define gbrrlog_format_debug    (gbrrlog_type_debug.format)
#define gbrrlog_format_clear    (gbrrlog_type_clear.format)
#define gbrrlog_format_last     (gbrrlog_type_last.format)

/* Various control settings for the log system */
extern BRRAPI struct gbrrlogctl {
	brrbl style_enabled:1;    /* Whether log styling is enabled at all; disabled always on Windows. Default: 1 */
	brrbl debug_enabled:1;    /* Whether the 'debug' log priority is enabled. Default: 0 */
	brrbl flush_enabled:1;    /* Whether automatic stream flushing is enabled. Default: 1 */
	brrbl flush_always:1;     /* Whether to always flush after logging; useful for debugging sometimes. Default: 0 */
	brrbl verbose_enabled:1;  /* Whether file, function, and line information should also be logged. Default: 0 */
	brrbl prefixes_enabled:1; /* Whether logs should print their prefixes by default. Default: 1 */
	brrbl newline_enabled:1;  /* Whether logs should print a newline by default. Default: 1 */
} gbrrlogctl;

/* Get maximum number of bytes that can be logged at once */
BRRAPI brrsz BRRCALL brrlog_logmax(void);
/* Set maximum number of bytes that can be logged at once.
 * Set to 0 to disable maximum.
 * If no error occurs, returns 0.
 * If an error occurs, returns -1.
 * MUST be called before using any log functions, acts as an initialization.
 * */
BRRAPI int BRRCALL brrlog_setlogmax(brrsz newmax);

/* Returns number of times logged */
BRRAPI brru8 BRRCALL brrlog_count(void);
/* Resets log count to 0 */
BRRAPI void BRRCALL brrlog_resetcount(void);

/* Returns the current minimum loggable priority */
BRRAPI brrlog_priority_t BRRCALL brrlog_minpriority(void);
/* Sets the minimum priority that will be logged.
 * Any priority less than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_setminpriority(brrlog_priority_t newmin);

/* Returns the current maximum loggable priority */
BRRAPI brrlog_priority_t BRRCALL brrlog_maxpriority(void);
/* Sets the maximum priority that will be logged.
 * Any priority greater than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_setmaxpriority(brrlog_priority_t newmax);

/* Initialize the log buffer.
 * The buffer is only used if 'logmax' isn't 0.
 * Run either this or 'brrlog_setlogmax' before trying to log.
 * If the log buffer was successfully initialized, 0 is returned.
 * If an error occurs, -1 is returned.
 * */
BRRAPI int BRRCALL brrlog_init(void);
/* Frees the log buffer used by brrlog */
BRRAPI void BRRCALL brrlog_deinit(void);

#define _brrlog_log_params \
    brrlog_priority_t priority, brrlog_destination_t destination, const char *prefix, \
    brrlog_color_t foreground, brrlog_color_t background, brrlog_style_t style, brrlog_font_t font, \
    char *const buffer, brrbl print_prefix, brrbl print_newline, \
    const char *const file, const char *const function, brru8 line

/* TODO doc comment
 * */
BRRAPI brrsz BRRCALL brrlog_text(_brrlog_log_params, const char *const format, ...);
/* TODO doc comment
 * */
BRRAPI brrsz BRRCALL brrlog_digits(_brrlog_log_params, brru8 number, int base,
    brrbl is_signed, char digit_separator, brrsz separator_spacing);
/* TODO doc comment
 * */
BRRAPI brrsz BRRCALL brrlog_bits(_brrlog_log_params, const void *const data,
    brrsz bits_to_print, brrbl reverse_bytes, brrbl reverse_bits, char bit_separator,
    brrsz separator_spacing);

#ifndef _brrlog_keep_generators
# undef _brrlog_color_gen
# undef _brrlog_font_gen
# undef _brrlog_style_gen
# undef _brrlog_priority_gen
# undef _brrlog_destination_gen
#endif

#define _brrlog_cat0(cat, \
    priority, destination, prefix, foreground, background, style, font, \
    buffer, print_prefix, print_newline, ...) \
    brrlog_##cat(priority, destination, prefix, foreground, background, style, font, \
        buffer, print_prefix, print_newline, __FILE__, __func__, __LINE__, __VA_ARGS__)

#define _brrlog_cat1(cat, \
    priority, destination, prefix, foreground, background, style, font, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, NULL, 1, 1, __VA_ARGS__)
#define _brrlog_cat2(cat, \
    priority, destination, prefix, foreground, background, style, font, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, NULL, 1, 0, __VA_ARGS__)
#define _brrlog_cat3(cat, \
    priority, destination, prefix, foreground, background, style, font, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, NULL, 0, 1, __VA_ARGS__)
#define _brrlog_cat4(cat, \
    priority, destination, prefix, foreground, background, style, font, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, NULL, 0, 0, __VA_ARGS__)
#define _brrlog_cat5(cat, \
    priority, destination, prefix, foreground, background, style, font, buffer, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, buffer, 1, 1, __VA_ARGS__)
#define _brrlog_cat6(cat, \
    priority, destination, prefix, foreground, background, style, font, buffer, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, buffer, 1, 0, __VA_ARGS__)
#define _brrlog_cat7(cat, \
    priority, destination, prefix, foreground, background, style, font, buffer, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, buffer, 0, 1, __VA_ARGS__)
#define _brrlog_cat8(cat, \
    priority, destination, prefix, foreground, background, style, font, buffer, ...) \
    _brrlog_cat0(cat, priority, destination, prefix, foreground, background, style, font, buffer, 0, 0, __VA_ARGS__)

#define _brrlog_docat0(type, cat, priority, destination, prefix, foreground, background, style, font, ...) \
    _brrlog_cat##type(cat, priority, destination, prefix, foreground, background, style, font, __VA_ARGS__)
#define _brrlog_docat1(type, cat, level, foreground, background, style, font, ...) \
    _brrlog_cat##type(cat, level.priority, level.destination, level.prefix, foreground, background, style, font, __VA_ARGS__)
#define _brrlog_docat2(type, cat, priority, destination, prefix, format, ...) \
    _brrlog_cat##type(cat, priority, destination, prefix, \
        format.foreground, format.background, format.style, format.font, \
        __VA_ARGS__)
#define _brrlog_docat3(type, cat, level, format, ...) \
    _brrlog_cat##type(cat, level.priority, level.destination, level.prefix, \
        format.foreground, format.background, format.style, format.font, \
        __VA_ARGS__)

#define _brrlog_docatn(typeA, typeB, cat, ...) _brrlog_docat##typeA(typeB, cat, __VA_ARGS__)

/* Explicit level, explicit format */
#define BRRLOG_LOG_MESSAGE(...)          _brrlog_docatn(0, 0, text, __VA_ARGS__)
#define BRRLOG_MESSAGE(...)              _brrlog_docatn(0, 1, text, __VA_ARGS__)
#define BRRLOG_MESSAGEN(...)             _brrlog_docatn(0, 2, text, __VA_ARGS__)
#define BRRLOG_MESSAGEP(...)             _brrlog_docatn(0, 3, text, __VA_ARGS__)
#define BRRLOG_MESSAGENP(...)            _brrlog_docatn(0, 4, text, __VA_ARGS__)
#define BRRLOG_MESSAGEB(...)             _brrlog_docatn(0, 5, text, __VA_ARGS__)
#define BRRLOG_MESSAGEBN(...)            _brrlog_docatn(0, 6, text, __VA_ARGS__)
#define BRRLOG_MESSAGEBP(...)            _brrlog_docatn(0, 7, text, __VA_ARGS__)
#define BRRLOG_MESSAGEBNP(...)           _brrlog_docatn(0, 8, text, __VA_ARGS__)
/* Struct level, explicit format */
#define BRRLOG_LOG_MESSAGE_FORMAT(...)   _brrlog_docatn(1, 0, text, __VA_ARGS__)
#define BRRLOG_MESSAGEF(...)             _brrlog_docatn(1, 1, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFN(...)            _brrlog_docatn(1, 2, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFP(...)            _brrlog_docatn(1, 3, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFNP(...)           _brrlog_docatn(1, 4, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFB(...)            _brrlog_docatn(1, 5, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFBN(...)           _brrlog_docatn(1, 6, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFBP(...)           _brrlog_docatn(1, 7, text, __VA_ARGS__)
#define BRRLOG_MESSAGEFBNP(...)          _brrlog_docatn(1, 8, text, __VA_ARGS__)
/* Explicit level, struct format */
#define BRRLOG_LOG_MESSAGE_LOCATION(...) _brrlog_docatn(2, 0, text, __VA_ARGS__)
#define BRRLOG_MESSAGEL(...)             _brrlog_docatn(2, 1, text, __VA_ARGS__)
#define BRRLOG_MESSAGELN(...)            _brrlog_docatn(2, 2, text, __VA_ARGS__)
#define BRRLOG_MESSAGELP(...)            _brrlog_docatn(2, 3, text, __VA_ARGS__)
#define BRRLOG_MESSAGELNP(...)           _brrlog_docatn(2, 4, text, __VA_ARGS__)
#define BRRLOG_MESSAGELB(...)            _brrlog_docatn(2, 5, text, __VA_ARGS__)
#define BRRLOG_MESSAGELBN(...)           _brrlog_docatn(2, 6, text, __VA_ARGS__)
#define BRRLOG_MESSAGELBP(...)           _brrlog_docatn(2, 7, text, __VA_ARGS__)
#define BRRLOG_MESSAGELBNP(...)          _brrlog_docatn(2, 8, text, __VA_ARGS__)
/* Struct level, struct format */
#define BRRLOG_LOG_MESSAGE_TYPE(...)     _brrlog_docatn(3, 0, text, __VA_ARGS__)
#define BRRLOG_MESSAGET(...)             _brrlog_docatn(3, 1, text, __VA_ARGS__)
#define BRRLOG_MESSAGETN(...)            _brrlog_docatn(3, 2, text, __VA_ARGS__)
#define BRRLOG_MESSAGETP(...)            _brrlog_docatn(3, 3, text, __VA_ARGS__)
#define BRRLOG_MESSAGETNP(...)           _brrlog_docatn(3, 4, text, __VA_ARGS__)
#define BRRLOG_MESSAGETB(...)            _brrlog_docatn(3, 5, text, __VA_ARGS__)
#define BRRLOG_MESSAGETBN(...)           _brrlog_docatn(3, 6, text, __VA_ARGS__)
#define BRRLOG_MESSAGETBP(...)           _brrlog_docatn(3, 7, text, __VA_ARGS__)
#define BRRLOG_MESSAGETBNP(...)          _brrlog_docatn(3, 8, text, __VA_ARGS__)

#define BRRLOG_MESSAGE_CRITICAL(...) BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_critical, gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_MESSAGE_ERROR(...)    BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_error,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_MESSAGE_NORMAL(...)   BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_normal,   gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_MESSAGE_WARNING(...)  BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_warning,  gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_MESSAGE_DEBUG(...)    BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_debug,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_CRITICAL(...)   BRRLOG_MESSAGE_CRITICAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_CRITICALN(...)  BRRLOG_MESSAGE_CRITICAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_CRITICALP(...)  BRRLOG_MESSAGE_CRITICAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_CRITICALNP(...) BRRLOG_MESSAGE_CRITICAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_CRITICALB(buffer, ...)   BRRLOG_MESSAGE_CRITICAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_CRITICALBN(buffer, ...)  BRRLOG_MESSAGE_CRITICAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_CRITICALBP(buffer, ...)  BRRLOG_MESSAGE_CRITICAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_CRITICALBNP(buffer, ...) BRRLOG_MESSAGE_CRITICAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_CRI   BRRLOG_CRITICAL
#define BRRLOG_CRIN  BRRLOG_CRITICALN
#define BRRLOG_CRIP  BRRLOG_CRITICALP
#define BRRLOG_CRINP BRRLOG_CRITICALNP
#define BRRLOG_CRIB   BRRLOG_CRITICALB
#define BRRLOG_CRIBN  BRRLOG_CRITICALBN
#define BRRLOG_CRIBP  BRRLOG_CRITICALBP
#define BRRLOG_CRIBNP BRRLOG_CRITICALBNP
#define BRRLOG_ERROR(...)   BRRLOG_MESSAGE_ERROR(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_ERRORN(...)  BRRLOG_MESSAGE_ERROR(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_ERRORP(...)  BRRLOG_MESSAGE_ERROR(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_ERRORNP(...) BRRLOG_MESSAGE_ERROR(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_ERRORB(buffer, ...)   BRRLOG_MESSAGE_ERROR(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_ERRORBN(buffer, ...)  BRRLOG_MESSAGE_ERROR(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_ERRORBP(buffer, ...)  BRRLOG_MESSAGE_ERROR(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_ERRORBNP(buffer, ...) BRRLOG_MESSAGE_ERROR(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_ERR   BRRLOG_ERROR
#define BRRLOG_ERRN  BRRLOG_ERRORN
#define BRRLOG_ERRP  BRRLOG_ERRORP
#define BRRLOG_ERRNP BRRLOG_ERRORNP
#define BRRLOG_ERRB   BRRLOG_ERRORB
#define BRRLOG_ERRBN  BRRLOG_ERRORBN
#define BRRLOG_ERRBP  BRRLOG_ERRORBP
#define BRRLOG_ERRBNP BRRLOG_ERRORBNP
#define BRRLOG_NORMAL(...)   BRRLOG_MESSAGE_NORMAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_NORMALN(...)  BRRLOG_MESSAGE_NORMAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_NORMALP(...)  BRRLOG_MESSAGE_NORMAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_NORMALNP(...) BRRLOG_MESSAGE_NORMAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_NORMALB(buffer, ...)   BRRLOG_MESSAGE_NORMAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_NORMALBN(buffer, ...)  BRRLOG_MESSAGE_NORMAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_NORMALBP(buffer, ...)  BRRLOG_MESSAGE_NORMAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_NORMALBNP(buffer, ...) BRRLOG_MESSAGE_NORMAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_NOR   BRRLOG_NORMAL
#define BRRLOG_NORN  BRRLOG_NORMALN
#define BRRLOG_NORP  BRRLOG_NORMALP
#define BRRLOG_NORNP BRRLOG_NORMALNP
#define BRRLOG_NORB   BRRLOG_NORMALB
#define BRRLOG_NORBN  BRRLOG_NORMALBN
#define BRRLOG_NORBP  BRRLOG_NORMALBP
#define BRRLOG_NORBNP BRRLOG_NORMALBNP
#define BRRLOG_WARNING(...)   BRRLOG_MESSAGE_WARNING(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_WARNINGN(...)  BRRLOG_MESSAGE_WARNING(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_WARNINGP(...)  BRRLOG_MESSAGE_WARNING(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_WARNINGNP(...) BRRLOG_MESSAGE_WARNING(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_WARNINGB(buffer, ...)   BRRLOG_MESSAGE_WARNING(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_WARNINGBN(buffer, ...)  BRRLOG_MESSAGE_WARNING(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_WARNINGBP(buffer, ...)  BRRLOG_MESSAGE_WARNING(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_WARNINGBNP(buffer, ...) BRRLOG_MESSAGE_WARNING(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_WAR   BRRLOG_WARNING
#define BRRLOG_WARN  BRRLOG_WARNINGN
#define BRRLOG_WARP  BRRLOG_WARNINGP
#define BRRLOG_WARNP BRRLOG_WARNINGNP
#define BRRLOG_WARB   BRRLOG_WARNINGB
#define BRRLOG_WARBN  BRRLOG_WARNINGBN
#define BRRLOG_WARBP  BRRLOG_WARNINGBP
#define BRRLOG_WARBNP BRRLOG_WARNINGBNP
#define BRRLOG_DEBUG(...)   BRRLOG_MESSAGE_DEBUG(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_DEBUGN(...)  BRRLOG_MESSAGE_DEBUG(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_DEBUGP(...)  BRRLOG_MESSAGE_DEBUG(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_DEBUGNP(...) BRRLOG_MESSAGE_DEBUG(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_DEBUGB(buffer, ...)   BRRLOG_MESSAGE_DEBUG(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_DEBUGBN(buffer, ...)  BRRLOG_MESSAGE_DEBUG(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_DEBUGBP(buffer, ...)  BRRLOG_MESSAGE_DEBUG(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_DEBUGBNP(buffer, ...) BRRLOG_MESSAGE_DEBUG(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_DEB   BRRLOG_DEBUG
#define BRRLOG_DEBN  BRRLOG_DEBUGN
#define BRRLOG_DEBP  BRRLOG_DEBUGP
#define BRRLOG_DEBNP BRRLOG_DEBUGNP
#define BRRLOG_DEBB   BRRLOG_DEBUGB
#define BRRLOG_DEBBN  BRRLOG_DEBUGBN
#define BRRLOG_DEBBP  BRRLOG_DEBUGBP
#define BRRLOG_DEBBNP BRRLOG_DEBUGBNP

#define BRRLOG_MESSAGE_LAST(...) BRRLOG_LOG_MESSAGE_TYPE(gbrrlog_level_last, gbrrlog_format_last, __VA_ARGS__)
#define BRRLOG_LAST(...)   BRRLOG_MESSAGE_LAST(NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_LASTN(...)  BRRLOG_MESSAGE_LAST(NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_LASTP(...)  BRRLOG_MESSAGE_LAST(NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_LASTNP(...) BRRLOG_MESSAGE_LAST(NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_LASTB(buffer, ...)   BRRLOG_MESSAGE_LAST(buffer, 1, 1, __VA_ARGS__)
#define BRRLOG_LASTBN(buffer, ...)  BRRLOG_MESSAGE_LAST(buffer, 1, 0, __VA_ARGS__)
#define BRRLOG_LASTBP(buffer, ...)  BRRLOG_MESSAGE_LAST(buffer, 0, 1, __VA_ARGS__)
#define BRRLOG_LASTBNP(buffer, ...) BRRLOG_MESSAGE_LAST(buffer, 0, 0, __VA_ARGS__)

#define BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_LOG_MESSAGE_FORMAT(gbrrlog_level_last, _fg_, _bg_, _st_, _fn_, __VA_ARGS__)
#define BRRLOG_FONT(_fg_, _bg_, _st_, _fn_, ...)   BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_FONTN(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_FONTP(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_FONTNP(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_FONTB(_buffer_, _fg_, _bg_, _st_, _fn_, ...)   BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOG_FONTBN(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOG_FONTBP(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOG_FONTBNP(_buffer_, _fg_, _bg_, _st_, _fn_, ...) BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, ...) BRRLOG_MESSAGE_FONT(_fg_, _bg_, _st_, 0, __VA_ARGS__)
#define BRRLOG_STYLE(_fg_, _bg_, _st_, ...)   BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_STYLEN(_fg_, _bg_, _st_, ...)  BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_STYLEP(_fg_, _bg_, _st_, ...)  BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_STYLENP(_fg_, _bg_, _st_, ...) BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_STYLEB(_buffer_, _fg_, _bg_, _st_, ...)   BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOG_STYLEBN(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOG_STYLEBP(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOG_STYLEBNP(_buffer_, _fg_, _bg_, _st_, ...) BRRLOG_MESSAGE_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_BACK(_fg_, _bg_, ...) BRRLOG_MESSAGE_STYLE(_fg_, _bg_, 0, __VA_ARGS__);
#define BRRLOG_BACK(_fg_, _bg_, ...)   BRRLOG_MESSAGE_BACK(_fg_, _bg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_BACKN(_fg_, _bg_, ...)  BRRLOG_MESSAGE_BACK(_fg_, _bg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_BACKP(_fg_, _bg_, ...)  BRRLOG_MESSAGE_BACK(_fg_, _bg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_BACKNP(_fg_, _bg_, ...) BRRLOG_MESSAGE_BACK(_fg_, _bg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_BACKB(_buffer_, _fg_, _bg_, ...)   BRRLOG_MESSAGE_BACK(_fg_, _bg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOG_BACKBN(_buffer_, _fg_, _bg_, ...)  BRRLOG_MESSAGE_BACK(_fg_, _bg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOG_BACKBP(_buffer_, _fg_, _bg_, ...)  BRRLOG_MESSAGE_BACK(_fg_, _bg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOG_BACKBNP(_buffer_, _fg_, _bg_, ...) BRRLOG_MESSAGE_BACK(_fg_, _bg_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_FORE(_fg_, ...) BRRLOG_MESSAGE_BACK(_fg_, 0, __VA_ARGS__);
#define BRRLOG_FORE(_fg_, ...)   BRRLOG_MESSAGE_FORE(_fg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOG_FOREN(_fg_, ...)  BRRLOG_MESSAGE_FORE(_fg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOG_FOREP(_fg_, ...)  BRRLOG_MESSAGE_FORE(_fg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOG_FORENP(_fg_, ...) BRRLOG_MESSAGE_FORE(_fg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOG_FOREB(_buffer_, _fg_, ...)   BRRLOG_MESSAGE_FORE(_fg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOG_FOREBN(_buffer_, _fg_, ...)  BRRLOG_MESSAGE_FORE(_fg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOG_FOREBP(_buffer_, _fg_, ...)  BRRLOG_MESSAGE_FORE(_fg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOG_FOREBNP(_buffer_, _fg_, ...) BRRLOG_MESSAGE_FORE(_fg_, _buffer_, 0, 0, __VA_ARGS__)

/* Explicit level, explicit format */
#define BRRLOG_LOG_DIGITS(...)          _brrlog_docatn(0, 0, digits, __VA_ARGS__)
#define BRRLOG_DIGITS(...)              _brrlog_docatn(0, 1, digits, __VA_ARGS__)
#define BRRLOG_DIGITSN(...)             _brrlog_docatn(0, 2, digits, __VA_ARGS__)
#define BRRLOG_DIGITSP(...)             _brrlog_docatn(0, 3, digits, __VA_ARGS__)
#define BRRLOG_DIGITSNP(...)            _brrlog_docatn(0, 4, digits, __VA_ARGS__)
#define BRRLOG_DIGITSB(...)             _brrlog_docatn(0, 5, digits, __VA_ARGS__)
#define BRRLOG_DIGITSBN(...)            _brrlog_docatn(0, 6, digits, __VA_ARGS__)
#define BRRLOG_DIGITSBP(...)            _brrlog_docatn(0, 7, digits, __VA_ARGS__)
#define BRRLOG_DIGITSBNP(...)           _brrlog_docatn(0, 8, digits, __VA_ARGS__)
/* Struct level, explicit format */
#define BRRLOG_LOG_DIGITS_FORMAT(...)   _brrlog_docatn(1, 0, digits, __VA_ARGS__)
#define BRRLOG_DIGITSF(...)             _brrlog_docatn(1, 1, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFN(...)            _brrlog_docatn(1, 2, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFP(...)            _brrlog_docatn(1, 3, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFNP(...)           _brrlog_docatn(1, 4, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFB(...)            _brrlog_docatn(1, 5, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFBN(...)           _brrlog_docatn(1, 6, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFBP(...)           _brrlog_docatn(1, 7, digits, __VA_ARGS__)
#define BRRLOG_DIGITSFBNP(...)          _brrlog_docatn(1, 8, digits, __VA_ARGS__)
/* Explicit level, struct format */
#define BRRLOG_LOG_DIGITS_LOCATION(...) _brrlog_docatn(2, 0, digits, __VA_ARGS__)
#define BRRLOG_DIGITSL(...)             _brrlog_docatn(2, 1, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLN(...)            _brrlog_docatn(2, 2, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLP(...)            _brrlog_docatn(2, 3, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLNP(...)           _brrlog_docatn(2, 4, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLB(...)            _brrlog_docatn(2, 5, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLBN(...)           _brrlog_docatn(2, 6, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLBP(...)           _brrlog_docatn(2, 7, digits, __VA_ARGS__)
#define BRRLOG_DIGITSLBNP(...)          _brrlog_docatn(2, 8, digits, __VA_ARGS__)
/* Struct level, struct format */
#define BRRLOG_LOG_DIGITS_TYPE(...)     _brrlog_docatn(3, 0, digits, __VA_ARGS__)
#define BRRLOG_DIGITST(...)             _brrlog_docatn(3, 1, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTN(...)            _brrlog_docatn(3, 2, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTP(...)            _brrlog_docatn(3, 3, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTNP(...)           _brrlog_docatn(3, 4, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTB(...)            _brrlog_docatn(3, 5, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTBN(...)           _brrlog_docatn(3, 6, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTBP(...)           _brrlog_docatn(3, 7, digits, __VA_ARGS__)
#define BRRLOG_DIGITSTBNP(...)          _brrlog_docatn(3, 8, digits, __VA_ARGS__)

#define BRRLOG_DIGITS_CRITICAL(...) BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_critical, gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_DIGITS_ERROR(...)    BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_error,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_DIGITS_NORMAL(...)   BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_normal,   gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_DIGITS_WARNING(...)  BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_warning,  gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_DIGITS_DEBUG(...)    BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_debug,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOGD_CRITICAL(...)   BRRLOG_DIGITS_CRITICAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALN(...)  BRRLOG_DIGITS_CRITICAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_CRITICALP(...)  BRRLOG_DIGITS_CRITICAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALNP(...) BRRLOG_DIGITS_CRITICAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_CRITICALB(buffer, ...)   BRRLOG_DIGITS_CRITICAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALBN(buffer, ...)  BRRLOG_DIGITS_CRITICAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_CRITICALBP(buffer, ...)  BRRLOG_DIGITS_CRITICAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALBNP(buffer, ...) BRRLOG_DIGITS_CRITICAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGD_CRI   BRRLOGD_CRITICAL
#define BRRLOGD_CRIN  BRRLOGD_CRITICALN
#define BRRLOGD_CRIP  BRRLOGD_CRITICALP
#define BRRLOGD_CRINP BRRLOGD_CRITICALNP
#define BRRLOGD_CRIB   BRRLOGD_CRITICALB
#define BRRLOGD_CRIBN  BRRLOGD_CRITICALBN
#define BRRLOGD_CRIBP  BRRLOGD_CRITICALBP
#define BRRLOGD_CRIBNP BRRLOGD_CRITICALBNP
#define BRRLOGD_ERROR(...)   BRRLOG_DIGITS_ERROR(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_ERRORN(...)  BRRLOG_DIGITS_ERROR(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_ERRORP(...)  BRRLOG_DIGITS_ERROR(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_ERRORNP(...) BRRLOG_DIGITS_ERROR(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_ERRORB(buffer, ...)   BRRLOG_DIGITS_ERROR(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_ERRORBN(buffer, ...)  BRRLOG_DIGITS_ERROR(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_ERRORBP(buffer, ...)  BRRLOG_DIGITS_ERROR(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_ERRORBNP(buffer, ...) BRRLOG_DIGITS_ERROR(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGD_ERR   BRRLOGD_ERROR
#define BRRLOGD_ERRN  BRRLOGD_ERRORN
#define BRRLOGD_ERRP  BRRLOGD_ERRORP
#define BRRLOGD_ERRNP BRRLOGD_ERRORNP
#define BRRLOGD_ERRB   BRRLOGD_ERRORB
#define BRRLOGD_ERRBN  BRRLOGD_ERRORBN
#define BRRLOGD_ERRBP  BRRLOGD_ERRORBP
#define BRRLOGD_ERRBNP BRRLOGD_ERRORBNP
#define BRRLOGD_NORMAL(...)   BRRLOG_DIGITS_NORMAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_NORMALN(...)  BRRLOG_DIGITS_NORMAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_NORMALP(...)  BRRLOG_DIGITS_NORMAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_NORMALNP(...) BRRLOG_DIGITS_NORMAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_NORMALB(buffer, ...)   BRRLOG_DIGITS_NORMAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_NORMALBN(buffer, ...)  BRRLOG_DIGITS_NORMAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_NORMALBP(buffer, ...)  BRRLOG_DIGITS_NORMAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_NORMALBNP(buffer, ...) BRRLOG_DIGITS_NORMAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGD_NOR   BRRLOGD_NORMAL
#define BRRLOGD_NORN  BRRLOGD_NORMALN
#define BRRLOGD_NORP  BRRLOGD_NORMALP
#define BRRLOGD_NORNP BRRLOGD_NORMALNP
#define BRRLOGD_NORB   BRRLOGD_NORMALB
#define BRRLOGD_NORBN  BRRLOGD_NORMALBN
#define BRRLOGD_NORBP  BRRLOGD_NORMALBP
#define BRRLOGD_NORBNP BRRLOGD_NORMALBNP
#define BRRLOGD_WARNING(...)   BRRLOG_DIGITS_WARNING(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGN(...)  BRRLOG_DIGITS_WARNING(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_WARNINGP(...)  BRRLOG_DIGITS_WARNING(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGNP(...) BRRLOG_DIGITS_WARNING(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_WARNINGB(buffer, ...)   BRRLOG_DIGITS_WARNING(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGBN(buffer, ...)  BRRLOG_DIGITS_WARNING(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_WARNINGBP(buffer, ...)  BRRLOG_DIGITS_WARNING(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGBNP(buffer, ...) BRRLOG_DIGITS_WARNING(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGD_WAR   BRRLOGD_WARNING
#define BRRLOGD_WARN  BRRLOGD_WARNINGN
#define BRRLOGD_WARP  BRRLOGD_WARNINGP
#define BRRLOGD_WARNP BRRLOGD_WARNINGNP
#define BRRLOGD_WARB   BRRLOGD_WARNINGB
#define BRRLOGD_WARBN  BRRLOGD_WARNINGBN
#define BRRLOGD_WARBP  BRRLOGD_WARNINGBP
#define BRRLOGD_WARBNP BRRLOGD_WARNINGBNP
#define BRRLOGD_DEBUG(...)   BRRLOG_DIGITS_DEBUG(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGN(...)  BRRLOG_DIGITS_DEBUG(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_DEBUGP(...)  BRRLOG_DIGITS_DEBUG(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGNP(...) BRRLOG_DIGITS_DEBUG(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_DEBUGB(buffer, ...)   BRRLOG_DIGITS_DEBUG(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGBN(buffer, ...)  BRRLOG_DIGITS_DEBUG(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_DEBUGBP(buffer, ...)  BRRLOG_DIGITS_DEBUG(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGBNP(buffer, ...) BRRLOG_DIGITS_DEBUG(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGD_DEB   BRRLOGD_DEBUG
#define BRRLOGD_DEBN  BRRLOGD_DEBUGN
#define BRRLOGD_DEBP  BRRLOGD_DEBUGP
#define BRRLOGD_DEBNP BRRLOGD_DEBUGNP
#define BRRLOGD_DEBB   BRRLOGD_DEBUGB
#define BRRLOGD_DEBBN  BRRLOGD_DEBUGBN
#define BRRLOGD_DEBBP  BRRLOGD_DEBUGBP
#define BRRLOGD_DEBBNP BRRLOGD_DEBUGBNP

#define BRRLOG_DIGITS_LAST(...) BRRLOG_LOG_DIGITS_TYPE(gbrrlog_level_last, gbrrlog_format_last, __VA_ARGS__)
#define BRRLOGD_LAST(...)   BRRLOG_DIGITS_LAST(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_LASTN(...)  BRRLOG_DIGITS_LAST(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_LASTP(...)  BRRLOG_DIGITS_LAST(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_LASTNP(...) BRRLOG_DIGITS_LAST(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_LASTB(buffer, ...)   BRRLOG_DIGITS_LAST(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGD_LASTBN(buffer, ...)  BRRLOG_DIGITS_LAST(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGD_LASTBP(buffer, ...)  BRRLOG_DIGITS_LAST(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGD_LASTBNP(buffer, ...) BRRLOG_DIGITS_LAST(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_LOG_DIGITS_FORMAT(gbrrlog_level_last, _fg_, _bg_, _st_, _fn_, __VA_ARGS__)
#define BRRLOGD_FONT(_fg_, _bg_, _st_, _fn_, ...)   BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_FONTN(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_FONTP(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_FONTNP(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_FONTB(_buffer_, _fg_, _bg_, _st_, _fn_, ...)   BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGD_FONTBN(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGD_FONTBP(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGD_FONTBNP(_buffer_, _fg_, _bg_, _st_, _fn_, ...) BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, ...) BRRLOG_DIGITS_FONT(_fg_, _bg_, _st_, 0, __VA_ARGS__)
#define BRRLOGD_STYLE(_fg_, _bg_, _st_, ...)   BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_STYLEN(_fg_, _bg_, _st_, ...)  BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_STYLEP(_fg_, _bg_, _st_, ...)  BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_STYLENP(_fg_, _bg_, _st_, ...) BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_STYLEB(_buffer_, _fg_, _bg_, _st_, ...)   BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGD_STYLEBN(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGD_STYLEBP(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGD_STYLEBNP(_buffer_, _fg_, _bg_, _st_, ...) BRRLOG_DIGITS_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_BACK(_fg_, _bg_, ...) BRRLOG_DIGITS_STYLE(_fg_, _bg_, 0, __VA_ARGS__);
#define BRRLOGD_BACK(_fg_, _bg_, ...)   BRRLOG_DIGITS_BACK(_fg_, _bg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_BACKN(_fg_, _bg_, ...)  BRRLOG_DIGITS_BACK(_fg_, _bg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_BACKP(_fg_, _bg_, ...)  BRRLOG_DIGITS_BACK(_fg_, _bg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_BACKNP(_fg_, _bg_, ...) BRRLOG_DIGITS_BACK(_fg_, _bg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_BACKB(_buffer_, _fg_, _bg_, ...)   BRRLOG_DIGITS_BACK(_fg_, _bg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGD_BACKBN(_buffer_, _fg_, _bg_, ...)  BRRLOG_DIGITS_BACK(_fg_, _bg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGD_BACKBP(_buffer_, _fg_, _bg_, ...)  BRRLOG_DIGITS_BACK(_fg_, _bg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGD_BACKBNP(_buffer_, _fg_, _bg_, ...) BRRLOG_DIGITS_BACK(_fg_, _bg_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FORE(_fg_, ...) BRRLOG_DIGITS_BACK(_fg_, 0, __VA_ARGS__);
#define BRRLOGD_FORE(_fg_, ...)   BRRLOG_DIGITS_FORE(_fg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGD_FOREN(_fg_, ...)  BRRLOG_DIGITS_FORE(_fg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGD_FOREP(_fg_, ...)  BRRLOG_DIGITS_FORE(_fg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGD_FORENP(_fg_, ...) BRRLOG_DIGITS_FORE(_fg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGD_FOREB(_buffer_, _fg_, ...)   BRRLOG_DIGITS_FORE(_fg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGD_FOREBN(_buffer_, _fg_, ...)  BRRLOG_DIGITS_FORE(_fg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGD_FOREBP(_buffer_, _fg_, ...)  BRRLOG_DIGITS_FORE(_fg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGD_FOREBNP(_buffer_, _fg_, ...) BRRLOG_DIGITS_FORE(_fg_, _buffer_, 0, 0, __VA_ARGS__)

/* Explicit level, explicit format */
#define BRRLOG_LOG_BITS(...)          _brrlog_docatn(0, 0, bits, __VA_ARGS__)
#define BRRLOG_BITS(...)              _brrlog_docatn(0, 1, bits, __VA_ARGS__)
#define BRRLOG_BITSN(...)             _brrlog_docatn(0, 2, bits, __VA_ARGS__)
#define BRRLOG_BITSP(...)             _brrlog_docatn(0, 3, bits, __VA_ARGS__)
#define BRRLOG_BITSNP(...)            _brrlog_docatn(0, 4, bits, __VA_ARGS__)
#define BRRLOG_BITSB(...)             _brrlog_docatn(0, 5, bits, __VA_ARGS__)
#define BRRLOG_BITSBN(...)            _brrlog_docatn(0, 6, bits, __VA_ARGS__)
#define BRRLOG_BITSBP(...)            _brrlog_docatn(0, 7, bits, __VA_ARGS__)
#define BRRLOG_BITSBNP(...)           _brrlog_docatn(0, 8, bits, __VA_ARGS__)
/* Struct level, explicit format */
#define BRRLOG_LOG_BITS_FORMAT(...)   _brrlog_docatn(1, 0, bits, __VA_ARGS__)
#define BRRLOG_BITSF(...)             _brrlog_docatn(1, 1, bits, __VA_ARGS__)
#define BRRLOG_BITSFN(...)            _brrlog_docatn(1, 2, bits, __VA_ARGS__)
#define BRRLOG_BITSFP(...)            _brrlog_docatn(1, 3, bits, __VA_ARGS__)
#define BRRLOG_BITSFNP(...)           _brrlog_docatn(1, 4, bits, __VA_ARGS__)
#define BRRLOG_BITSFB(...)            _brrlog_docatn(1, 5, bits, __VA_ARGS__)
#define BRRLOG_BITSFBN(...)           _brrlog_docatn(1, 6, bits, __VA_ARGS__)
#define BRRLOG_BITSFBP(...)           _brrlog_docatn(1, 7, bits, __VA_ARGS__)
#define BRRLOG_BITSFBNP(...)          _brrlog_docatn(1, 8, bits, __VA_ARGS__)
/* Explicit level, struct format */
#define BRRLOG_LOG_BITS_LOCATION(...) _brrlog_docatn(2, 0, bits, __VA_ARGS__)
#define BRRLOG_BITSL(...)             _brrlog_docatn(2, 1, bits, __VA_ARGS__)
#define BRRLOG_BITSLN(...)            _brrlog_docatn(2, 2, bits, __VA_ARGS__)
#define BRRLOG_BITSLP(...)            _brrlog_docatn(2, 3, bits, __VA_ARGS__)
#define BRRLOG_BITSLNP(...)           _brrlog_docatn(2, 4, bits, __VA_ARGS__)
#define BRRLOG_BITSLB(...)            _brrlog_docatn(2, 5, bits, __VA_ARGS__)
#define BRRLOG_BITSLBN(...)           _brrlog_docatn(2, 6, bits, __VA_ARGS__)
#define BRRLOG_BITSLBP(...)           _brrlog_docatn(2, 7, bits, __VA_ARGS__)
#define BRRLOG_BITSLBNP(...)          _brrlog_docatn(2, 8, bits, __VA_ARGS__)
/* Struct level, struct format */
#define BRRLOG_LOG_BITS_TYPE(...)     _brrlog_docatn(3, 0, bits, __VA_ARGS__)
#define BRRLOG_BITST(...)             _brrlog_docatn(3, 1, bits, __VA_ARGS__)
#define BRRLOG_BITSTN(...)            _brrlog_docatn(3, 2, bits, __VA_ARGS__)
#define BRRLOG_BITSTP(...)            _brrlog_docatn(3, 3, bits, __VA_ARGS__)
#define BRRLOG_BITSTNP(...)           _brrlog_docatn(3, 4, bits, __VA_ARGS__)
#define BRRLOG_BITSTB(...)            _brrlog_docatn(3, 5, bits, __VA_ARGS__)
#define BRRLOG_BITSTBN(...)           _brrlog_docatn(3, 6, bits, __VA_ARGS__)
#define BRRLOG_BITSTBP(...)           _brrlog_docatn(3, 7, bits, __VA_ARGS__)
#define BRRLOG_BITSTBNP(...)          _brrlog_docatn(3, 8, bits, __VA_ARGS__)

#define BRRLOG_BITS_CRITICAL(...) BRRLOG_LOG_BITS_TYPE(gbrrlog_level_critical, gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_BITS_ERROR(...)    BRRLOG_LOG_BITS_TYPE(gbrrlog_level_error,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_BITS_NORMAL(...)   BRRLOG_LOG_BITS_TYPE(gbrrlog_level_normal,   gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_BITS_WARNING(...)  BRRLOG_LOG_BITS_TYPE(gbrrlog_level_warning,  gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOG_BITS_DEBUG(...)    BRRLOG_LOG_BITS_TYPE(gbrrlog_level_debug,    gbrrlog_format_normal, __VA_ARGS__)
#define BRRLOGB_CRITICAL(...)   BRRLOG_BITS_CRITICAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALN(...)  BRRLOG_BITS_CRITICAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_CRITICALP(...)  BRRLOG_BITS_CRITICAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALNP(...) BRRLOG_BITS_CRITICAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_CRITICALB(buffer, ...)   BRRLOG_BITS_CRITICAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALBN(buffer, ...)  BRRLOG_BITS_CRITICAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_CRITICALBP(buffer, ...)  BRRLOG_BITS_CRITICAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALBNP(buffer, ...) BRRLOG_BITS_CRITICAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGB_CRI   BRRLOGB_CRITICAL
#define BRRLOGB_CRIN  BRRLOGB_CRITICALN
#define BRRLOGB_CRIP  BRRLOGB_CRITICALP
#define BRRLOGB_CRINP BRRLOGB_CRITICALNP
#define BRRLOGB_CRIB   BRRLOGB_CRITICALB
#define BRRLOGB_CRIBN  BRRLOGB_CRITICALBN
#define BRRLOGB_CRIBP  BRRLOGB_CRITICALBP
#define BRRLOGB_CRIBNP BRRLOGB_CRITICALBNP
#define BRRLOGB_ERROR(...)   BRRLOG_BITS_ERROR(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_ERRORN(...)  BRRLOG_BITS_ERROR(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_ERRORP(...)  BRRLOG_BITS_ERROR(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_ERRORNP(...) BRRLOG_BITS_ERROR(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_ERRORB(buffer, ...)   BRRLOG_BITS_ERROR(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_ERRORBN(buffer, ...)  BRRLOG_BITS_ERROR(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_ERRORBP(buffer, ...)  BRRLOG_BITS_ERROR(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_ERRORBNP(buffer, ...) BRRLOG_BITS_ERROR(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGB_ERR   BRRLOGB_ERROR
#define BRRLOGB_ERRN  BRRLOGB_ERRORN
#define BRRLOGB_ERRP  BRRLOGB_ERRORP
#define BRRLOGB_ERRNP BRRLOGB_ERRORNP
#define BRRLOGB_ERRB   BRRLOGB_ERRORB
#define BRRLOGB_ERRBN  BRRLOGB_ERRORBN
#define BRRLOGB_ERRBP  BRRLOGB_ERRORBP
#define BRRLOGB_ERRBNP BRRLOGB_ERRORBNP
#define BRRLOGB_NORMAL(...)   BRRLOG_BITS_NORMAL(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_NORMALN(...)  BRRLOG_BITS_NORMAL(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_NORMALP(...)  BRRLOG_BITS_NORMAL(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_NORMALNP(...) BRRLOG_BITS_NORMAL(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_NORMALB(buffer, ...)   BRRLOG_BITS_NORMAL(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_NORMALBN(buffer, ...)  BRRLOG_BITS_NORMAL(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_NORMALBP(buffer, ...)  BRRLOG_BITS_NORMAL(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_NORMALBNP(buffer, ...) BRRLOG_BITS_NORMAL(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGB_NOR   BRRLOGB_NORMAL
#define BRRLOGB_NORN  BRRLOGB_NORMALN
#define BRRLOGB_NORP  BRRLOGB_NORMALP
#define BRRLOGB_NORNP BRRLOGB_NORMALNP
#define BRRLOGB_NORB   BRRLOGB_NORMALB
#define BRRLOGB_NORBN  BRRLOGB_NORMALBN
#define BRRLOGB_NORBP  BRRLOGB_NORMALBP
#define BRRLOGB_NORBNP BRRLOGB_NORMALBNP
#define BRRLOGB_WARNING(...)   BRRLOG_BITS_WARNING(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGN(...)  BRRLOG_BITS_WARNING(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_WARNINGP(...)  BRRLOG_BITS_WARNING(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGNP(...) BRRLOG_BITS_WARNING(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_WARNINGB(buffer, ...)   BRRLOG_BITS_WARNING(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGBN(buffer, ...)  BRRLOG_BITS_WARNING(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_WARNINGBP(buffer, ...)  BRRLOG_BITS_WARNING(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGBNP(buffer, ...) BRRLOG_BITS_WARNING(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGB_WAR   BRRLOGB_WARNING
#define BRRLOGB_WARN  BRRLOGB_WARNINGN
#define BRRLOGB_WARP  BRRLOGB_WARNINGP
#define BRRLOGB_WARNP BRRLOGB_WARNINGNP
#define BRRLOGB_WARB   BRRLOGB_WARNINGB
#define BRRLOGB_WARBN  BRRLOGB_WARNINGBN
#define BRRLOGB_WARBP  BRRLOGB_WARNINGBP
#define BRRLOGB_WARBNP BRRLOGB_WARNINGBNP
#define BRRLOGB_DEBUG(...)   BRRLOG_BITS_DEBUG(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGN(...)  BRRLOG_BITS_DEBUG(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_DEBUGP(...)  BRRLOG_BITS_DEBUG(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGNP(...) BRRLOG_BITS_DEBUG(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_DEBUGB(buffer, ...)   BRRLOG_BITS_DEBUG(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGBN(buffer, ...)  BRRLOG_BITS_DEBUG(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_DEBUGBP(buffer, ...)  BRRLOG_BITS_DEBUG(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGBNP(buffer, ...) BRRLOG_BITS_DEBUG(buffer, 0, 0, __VA_ARGS__)
#define BRRLOGB_DEB   BRRLOGB_DEBUG
#define BRRLOGB_DEBN  BRRLOGB_DEBUGN
#define BRRLOGB_DEBP  BRRLOGB_DEBUGP
#define BRRLOGB_DEBNP BRRLOGB_DEBUGNP
#define BRRLOGB_DEBB   BRRLOGB_DEBUGB
#define BRRLOGB_DEBBN  BRRLOGB_DEBUGBN
#define BRRLOGB_DEBBP  BRRLOGB_DEBUGBP
#define BRRLOGB_DEBBNP BRRLOGB_DEBUGBNP

#define BRRLOG_BITS_LAST(...) BRRLOG_LOG_BITS_TYPE(gbrrlog_level_last, gbrrlog_format_last, __VA_ARGS__)
#define BRRLOGB_LAST(...)   BRRLOG_BITS_LAST(NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_LASTN(...)  BRRLOG_BITS_LAST(NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_LASTP(...)  BRRLOG_BITS_LAST(NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_LASTNP(...) BRRLOG_BITS_LAST(NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_LASTB(buffer, ...)   BRRLOG_BITS_LAST(buffer, 1, 1, __VA_ARGS__)
#define BRRLOGB_LASTBN(buffer, ...)  BRRLOG_BITS_LAST(buffer, 1, 0, __VA_ARGS__)
#define BRRLOGB_LASTBP(buffer, ...)  BRRLOG_BITS_LAST(buffer, 0, 1, __VA_ARGS__)
#define BRRLOGB_LASTBNP(buffer, ...) BRRLOG_BITS_LAST(buffer, 0, 0, __VA_ARGS__)
#define BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_LOG_BITS_FORMAT(gbrrlog_level_last, _fg_, _bg_, _st_, _fn_, __VA_ARGS__)
#define BRRLOGB_FONT(_fg_, _bg_, _st_, _fn_, ...)   BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_FONTN(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_FONTP(_fg_, _bg_, _st_, _fn_, ...)  BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_FONTNP(_fg_, _bg_, _st_, _fn_, ...) BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_FONTB(_buffer_, _fg_, _bg_, _st_, _fn_, ...)   BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGB_FONTBN(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGB_FONTBP(_buffer_, _fg_, _bg_, _st_, _fn_, ...)  BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGB_FONTBNP(_buffer_, _fg_, _bg_, _st_, _fn_, ...) BRRLOG_BITS_FONT(_fg_, _bg_, _st_, _fn_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, ...) BRRLOG_BITS_FONT(_fg_, _bg_, _st_, 0, __VA_ARGS__)
#define BRRLOGB_STYLE(_fg_, _bg_, _st_, ...)   BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_STYLEN(_fg_, _bg_, _st_, ...)  BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_STYLEP(_fg_, _bg_, _st_, ...)  BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_STYLENP(_fg_, _bg_, _st_, ...) BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_STYLEB(_buffer_, _fg_, _bg_, _st_, ...)   BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGB_STYLEBN(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGB_STYLEBP(_buffer_, _fg_, _bg_, _st_, ...)  BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGB_STYLEBNP(_buffer_, _fg_, _bg_, _st_, ...) BRRLOG_BITS_STYLE(_fg_, _bg_, _st_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_BITS_BACK(_fg_, _bg_, ...) BRRLOG_BITS_STYLE(_fg_, _bg_, 0, __VA_ARGS__);
#define BRRLOGB_BACK(_fg_, _bg_, ...)   BRRLOG_BITS_BACK(_fg_, _bg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_BACKN(_fg_, _bg_, ...)  BRRLOG_BITS_BACK(_fg_, _bg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_BACKP(_fg_, _bg_, ...)  BRRLOG_BITS_BACK(_fg_, _bg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_BACKNP(_fg_, _bg_, ...) BRRLOG_BITS_BACK(_fg_, _bg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_BACKB(_buffer_, _fg_, _bg_, ...)   BRRLOG_BITS_BACK(_fg_, _bg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGB_BACKBN(_buffer_, _fg_, _bg_, ...)  BRRLOG_BITS_BACK(_fg_, _bg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGB_BACKBP(_buffer_, _fg_, _bg_, ...)  BRRLOG_BITS_BACK(_fg_, _bg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGB_BACKBNP(_buffer_, _fg_, _bg_, ...) BRRLOG_BITS_BACK(_fg_, _bg_, _buffer_, 0, 0, __VA_ARGS__)
#define BRRLOG_BITS_FORE(_fg_, ...) BRRLOG_BITS_BACK(_fg_, 0, __VA_ARGS__);
#define BRRLOGB_FORE(_fg_, ...)   BRRLOG_BITS_FORE(_fg_, NULL, 1, 1, __VA_ARGS__)
#define BRRLOGB_FOREN(_fg_, ...)  BRRLOG_BITS_FORE(_fg_, NULL, 1, 0, __VA_ARGS__)
#define BRRLOGB_FOREP(_fg_, ...)  BRRLOG_BITS_FORE(_fg_, NULL, 0, 1, __VA_ARGS__)
#define BRRLOGB_FORENP(_fg_, ...) BRRLOG_BITS_FORE(_fg_, NULL, 0, 0, __VA_ARGS__)
#define BRRLOGB_FOREB(_buffer_, _fg_, ...)   BRRLOG_BITS_FORE(_fg_, _buffer_, 1, 1, __VA_ARGS__)
#define BRRLOGB_FOREBN(_buffer_, _fg_, ...)  BRRLOG_BITS_FORE(_fg_, _buffer_, 1, 0, __VA_ARGS__)
#define BRRLOGB_FOREBP(_buffer_, _fg_, ...)  BRRLOG_BITS_FORE(_fg_, _buffer_, 0, 1, __VA_ARGS__)
#define BRRLOGB_FOREBNP(_buffer_, _fg_, ...) BRRLOG_BITS_FORE(_fg_, _buffer_, 0, 0, __VA_ARGS__)

_brrcppend

#endif /* BRRTOOLS_BRRLOG_H */
