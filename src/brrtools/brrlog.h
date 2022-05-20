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

// _t = name prefix (or enum type)
// _N = capital name
// _n = lowercase name
// _i = value
// _d = string description

#define _enum_def(_t,_N,_n,_i,_d) brrlog_##_t##_##_n = _i,
#define _enum_gen(_int_type_, _name_) \
	typedef enum brrlog_##_name_ {\
		_brrlog_##_name_##_gen(_enum_def)\
		brrlog_##_name_##_count,\
	} brrlog_##_name_##_t;\
	typedef _int_type_ brrlog_##_name_##_int;\
	extern const char *brrlog_##_name_##_names[brrlog_##_name_##_count+1];\
	extern const char *brrlog_##_name_##_debug_names[brrlog_##_name_##_count+1];\
	BRRAPI const char *BRRCALL brrlog_##_name_##_str(brrlog_##_name_##_t _name_)

/* ANSI colors used for logging */
#define _brrlog_color_gen(_ex_)\
    _ex_(color, LAST,         last,         -1, "Last used color")\
    _ex_(color, NORM,         normal,        0, "Normal/default color")\
    _ex_(color, BLACK,        black,         1, "Black")\
    _ex_(color, RED,          red,           2, "Red")\
    _ex_(color, GREEN,        green,         3, "Green")\
    _ex_(color, YELLOW,       yellow,        4, "Yellow")\
    _ex_(color, BLUE,         blue,          5, "Blue")\
    _ex_(color, MAGENTA,      magenta,       6, "Magenta")\
    _ex_(color, CYAN,         cyan,          7, "Cyan")\
    _ex_(color, WHITE,        white,         8, "White")\
    _ex_(color, DARKGREY,     darkgrey,      9, "Dark grey/light black")\
    _ex_(color, LIGHTRED,     lightred,     10, "Light red")\
    _ex_(color, LIGHTGREEN,   lightgreen,   11, "Light green")\
    _ex_(color, LIGHTYELLOW,  lightyellow,  12, "Light yellow")\
    _ex_(color, LIGHTBLUE,    lightblue,    13, "Light blue")\
    _ex_(color, LIGHTMAGENTA, lightmagenta, 14, "Light magenta")\
    _ex_(color, LIGHTCYAN,    lightcyan,    15, "Light cyan")\
    _ex_(color, LIGHTWHITE,   lightwhite,   16, "Light white")
_enum_gen(brrs1, color);

/* Font types to used for logging */
#define _brrlog_font_gen(_ex_)\
    _ex_(font, LAST,   last,   -1, "Last used font")\
    _ex_(font, NORMAL, normal,  0, "Use the normal font")\
    _ex_(font, ONE,    1,       1, "Use alternate font 1")\
    _ex_(font, TWO,    2,       2, "Use alternate font 2")\
    _ex_(font, THREE,  3,       3, "Use alternate font 3")\
    _ex_(font, FOUR,   4,       4, "Use alternate font 4")\
    _ex_(font, FIVE,   5,       5, "Use alternate font 5")\
    _ex_(font, SIX,    6,       6, "Use alternate font 6")\
    _ex_(font, SEVEN,  7,       7, "Use alternate font 7")\
    _ex_(font, EIGHT,  8,       8, "Use alternate font 8")\
    _ex_(font, NINE,   9,       9, "Use alternate font 9")
_enum_gen(brrs1, font);

/* Text styles used for logging */
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
    _ex_(style, IOFF,         ioff,         29, "Disable ideograms")
_enum_gen(brrs1, style);

/* Log levels used for logging.
 * These control when a message is logged, based on the currenlty set minimum and maximum log levels.
 * */
#define _brrlog_priority_gen(_ex_)\
    _ex_(priority, LST, last,     -1, "Last used log priority")\
    _ex_(priority, NON, none,      0, "No priority; nothing is logged")\
    _ex_(priority, CRI, critical,  1, "Critical error")\
    _ex_(priority, ERR, error,     2, "Non-fatal error")\
    _ex_(priority, NOR, normal,    3, "Normal log message")\
    _ex_(priority, WAR, warning,   4, "Warning")\
    _ex_(priority, DBG, debug,     5, "Debug message, only logged when debug is enabled")
_enum_gen(brrs1, priority);

/* Destinations for log messages to go.
 * Only 'stdout' and 'stderr' loggable, for now.
 * */
#define _brrlog_destination_gen(_ex_)\
	_ex_(destination, LAST, last,   -1, "Last used output destination")\
	_ex_(destination, NULL, null,    0, "No output destination, i.e. nothing is logged")\
	_ex_(destination, STDO, stdout,  1, "Output to standard-out")\
	_ex_(destination, STDE, stderr,  2, "Output to standard-error")
_enum_gen(brrs1, destination);
#undef _enum_def

// Basic log-level structure, storing a log message's priortiy, destination, and prefix.
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

typedef union _gbrrlog_types {
	struct {
		brrlog_type_t last;     // Last-used log stylings
		brrlog_type_t clear;    // Format used to clear log styles once log output has finished
		brrlog_type_t critical; // Default styling for 'critical' priority logs.
		brrlog_type_t error;    // Default styling for 'error' priority logs.
		brrlog_type_t normal;   // Default styling for 'normal' priority logs.
		brrlog_type_t warning;  // Default styling for 'warning' priority logs.
		brrlog_type_t debug;    // Default styling for 'debug' priority logs.
	};
	brrlog_type_t types[brrlog_priority_count + 1];
} _gbrrlog_types_t;
extern BRRAPI _gbrrlog_types_t gbrrlog_type;
#define gbrrlog_level(_priority_) (gbrrlog_type._priority_.level)
#define gbrrlog_format(_priority_) (gbrrlog_type._priority_.format)

/* Various control settings for the log system */
extern BRRAPI struct gbrrlogctl {
	// Disables log styling all-together; always disabled on Windows.
	// Default: 0
	brru1 style_disabled:1;
	// Enable 'debug' priority logs.
	// Default: 0
	brru1 debug_enabled:1;
	// Automatically flush current stream when destination changes.
	// Default: 1
	brru1 flush_enabled:1;
	// Flush after every log; useful for debugging sometimes.
	// Default: 0
	brru1 flush_always:1;
	// Log file, function, and line information.
	// Default: 0
	brru1 verbose_enabled:1;
	// Log priority prefixes are printed by default.
	// Default: 1
	brru1 prefixes_enabled:1;
	// Logs print a newline by default.
	// Default: 1
	brru1 newline_enabled:1;
} gbrrlogctl;

/* Set maximum number of bytes that can be logged at once.
 * Set to 0 to disable maximum.
 * Returns 0 on success, otherwise -1 is returned and log is unaffected; check errno to see the specific error.
 * MUST be called before using any log functions, acts as an initialization.
 * Default is 2048.
 * */
BRRAPI int BRRCALL brrlog_set_max_log(brrsz new_max);
/* Get maximum number of bytes that can be logged at once.
 * Default is 2048.
 * */
BRRAPI brrsz BRRCALL brrlog_max_log(void);

/* Resets log count to 0 */
BRRAPI void BRRCALL brrlog_reset_count(void);
/* Returns number of times logged */
BRRAPI brru8 BRRCALL brrlog_count(void);

/* Returns the current minimum loggable priority */
BRRAPI brrlog_priority_t BRRCALL brrlog_min_priority(void);
/* Sets the minimum priority that will be logged.
 * Any priority less than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_set_min_priority(brrlog_priority_t new_min);

/* Returns the current maximum loggable priority */
BRRAPI brrlog_priority_t BRRCALL brrlog_max_priority(void);
/* Sets the maximum priority that will be logged.
 * Any priority greater than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_set_max_priority(brrlog_priority_t new_max);

/* Initialize the log buffer.
 * The buffer is only used if 'max_log' isn't 0.
 * Run either this or 'brrlog_set_max_log' before trying to log anything.
 * Returns 0 on success, otherwise returns -1; see errno for the specific error.
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

#include <brrtools/_brrlog_macros.h>

_brrcppend

#endif /* BRRTOOLS_BRRLOG_H */
