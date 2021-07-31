/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

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

/* I may be stupid, ... */

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* ANSI colors used for logging. */
typedef enum brrlog_color {
	brrlog_color_last = -1,    /*  Last used color. */
	brrlog_color_normal,       /*  Normal/default color. */
	brrlog_color_black,        /*  Black. */
	brrlog_color_red,          /*  Red. */
	brrlog_color_green,        /*  Green. */
	brrlog_color_yellow,       /*  Yellow. */
	brrlog_color_blue,         /*  Blue. */
	brrlog_color_magenta,      /*  Magenta. */
	brrlog_color_cyan,         /*  Cyan. */
	brrlog_color_white,        /*  White. */
	brrlog_color_darkgrey,     /*  Dark grey/light black. */
	brrlog_color_lightred,     /*  Light red. */
	brrlog_color_lightgreen,   /*  Light green. */
	brrlog_color_lightyellow,  /*  Light yellow. */
	brrlog_color_lightblue,    /*  Light blue. */
	brrlog_color_lightmagenta, /*  Light magenta. */
	brrlog_color_lightcyan,    /*  Light cyan. */
	brrlog_color_lightwhite,   /*  Light white. */
	brrlog_color_count,
} brrlog_colorT;

/* Font types to used for logging. */
typedef enum brrlog_font {
	brrlog_font_last = -1, /*  Last used font. */
	brrlog_font_normal,    /*  Use the normal font. */
	brrlog_font_1,         /*  Use alternate font 1. */
	brrlog_font_2,         /*  Use alternate font 2. */
	brrlog_font_3,         /*  Use alternate font 3. */
	brrlog_font_4,         /*  Use alternate font 4. */
	brrlog_font_5,         /*  Use alternate font 5. */
	brrlog_font_6,         /*  Use alternate font 6. */
	brrlog_font_7,         /*  Use alternate font 7. */
	brrlog_font_8,         /*  Use alternate font 8. */
	brrlog_font_9,         /*  Use alternate font 9. */
	brrlog_font_count
} brrlog_fontT;

/* Text styles used for logging. */
typedef enum brrlog_style {
	brrlog_style_last = -1,     /*  Last used style. */
	brrlog_style_normal,        /*  Normal style, text is plain. */
	brrlog_style_bold,          /*  Bold/bright text. */
	brrlog_style_dim,           /*  Dim/darker text. */
	brrlog_style_italics,       /*  Text is italics. */
	brrlog_style_under,         /*  Underlined text. */
	brrlog_style_blink,         /*  Blinking text. */
	brrlog_style_fastblink,     /*  Blinking text, but faster. */
	brrlog_style_reverse,       /*  Text foreground and background colors are swapped. */
	brrlog_style_conceal,       /*  Text isn't drawn. */
	brrlog_style_strikeout,     /*  Text is striked out. */
	brrlog_style_fraktur,       /*  Medieval text. */
	brrlog_style_nobold,        /*  Disable bold text/double underlined text. */
	brrlog_style_nobright,      /*  Disable text brightening/dimming. */
	brrlog_style_noitalics,     /*  Disable italics/fraktur text style. */
	brrlog_style_nounder,       /*  Disable text underline. */
	brrlog_style_noblink,       /*  Disable text blinking. */
	brrlog_style_noreverse,     /*  Disable text reversal. */
	brrlog_style_reveal,        /*  Disable text conceal. */
	brrlog_style_nostrikeout,   /*  Disable text strikeout. */
	brrlog_style_frame,         /*  Text is framed. */
	brrlog_style_circle,        /*  Text is encircled. */
	brrlog_style_over,          /*  Overlined text. */
	brrlog_style_noframe,       /*  Disable text framing/encircling. */
	brrlog_style_noover,        /*  Disable text overline. */
	brrlog_style_iunder,        /*  Ideogram underline. */
	brrlog_style_idoubleunder,  /*  Ideogram double-underline. */
	brrlog_style_iover,         /*  Ideogram overline. */
	brrlog_style_idoubleover,   /*  Ideogram double-overline. */
	brrlog_style_istress,       /*  Ideogram stressing. */
	brrlog_style_ioff,          /*  Disable ideograms. */
	brrlog_style_count,
	brrlog_style_none = brrlog_style_normal,
} brrlog_styleT;

/* Log levels used for logging.
 * These control whether a message is logged, based on the minimum and maximum
 * log levels set.
 * */
typedef enum brrlog_priority {
	brrlog_priority_last = -1, /* Last used log priority. */
	brrlog_priority_none,      /* No priority; nothing is logged. */
	brrlog_priority_critical,  /* Critical error. */
	brrlog_priority_error,     /* Non-fatal error. */
	brrlog_priority_normal,    /* Normal log message. */
	brrlog_priority_warning,   /* Warning. */
	brrlog_priority_debug,     /* Debug message, only logged when debug is enabled. */
	brrlog_priority_count,
} brrlog_priorityT;

/* Destinations for log messages to go.
 * Only 'stdout' and 'stderr' loggable, for now.
 * TODO loggable to generic 'FILE *'.
 * */
typedef enum brrlog_destination {
	brrlog_destination_last = -1,
	brrlog_destination_null,
	brrlog_destination_stdout,
	brrlog_destination_stderr,
} brrlog_destinationT;

/* Basic log-level structure, storing a log message's priortiy, destination, and
 * prefix.
 * */
typedef struct brrlog_level {
	brrlog_priorityT priority;
	brrlog_destinationT destination;
	const char *prefix;
} brrlog_levelT;
/* Settings used for stylizing log output. */
typedef struct brrlog_format {
	brrlog_colorT foreground; /* Format foreground color. */
	brrlog_colorT background; /* Format background color. */
	brrlog_styleT style;      /* Format text style. */
	brrlog_fontT  font;       /* Format format font. */
} brrlog_formatT;
/* There is almost certainly a very tricky way to pack these four macros into one. */
#define BRRLOG_FORMAT_FONT(_f_, _b_, _s_, _F_) ((brrlog_formatT){_f_, _b_, _s_, _F_})
#define BRRLOG_FORMAT_STYLE(_f_, _b_, _s_)     BRRLOG_FORMAT_FONT(_f_, _b_, _s_, brrlog_font_normal)
#define BRRLOG_FORMAT_BACK(_f_, _b_)           BRRLOG_FORMAT_STYLE(_f_, _b_, brrlog_style_normal)
#define BRRLOG_FORMAT_FORE(_f_)                BRRLOG_FORMAT_BACK(_f_, brrlog_color_normal)

typedef struct brrlog_type {
	brrlog_levelT level;
	brrlog_formatT format;
} brrlog_typeT;

extern BRRAPI brrlog_typeT gbrrlog_type_critical; /* Logs of 'critical' priority will be logged with this style by default. */
extern BRRAPI brrlog_typeT gbrrlog_type_error;    /* Logs of 'error' priority will be logged with this style by default. */
extern BRRAPI brrlog_typeT gbrrlog_type_normal;   /* Logs of 'normal' priority will be logged with this style by default. */
extern BRRAPI brrlog_typeT gbrrlog_type_warning;  /* Logs of 'warning' priority will be logged with this style by default. */
extern BRRAPI brrlog_typeT gbrrlog_type_debug;    /* Logs of 'debug' priority will be logged with this style by default. */
extern BRRAPI brrlog_typeT gbrrlog_type_clear;    /* Format used to clear log styles once log output has finished. */
extern BRRAPI brrlog_typeT gbrrlog_type_last;     /* Settings last-used to log a message. w*/

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

/* Various control settings for the log system. */
extern BRRAPI struct gbrrlogctl {
	brru2 style_enabled:1;    /* Whether log styling is enabled at all. */
	brru2 debug_enabled:1;    /* Whether the 'debug' log priority is enabled. */
	brru2 flush_enabled:1;    /* Whether automatic stream flushing is enabled. */
	brru2 verbose_enabled:1;  /* Whether file, function, and line information should also be logged. */
	brru2 prefixes_enabled:1; /* Whether logs should print their prefixes by default. */
	brru2 newline_enabled:1;  /* Whether logs should print a newline by default. */
} gbrrlogctl;

/* Get a string representation of the given color. */
BRRAPI const char *BRRCALL brrlog_color_str(brrlog_colorT color);
/* Get a string representation of the given font. */
BRRAPI const char *BRRCALL brrlog_font_str(brrlog_fontT font);
/* Get a string representation of the given style. */
BRRAPI const char *BRRCALL brrlog_style_str(brrlog_styleT style);
/* Get a string representation of the given level. */
BRRAPI const char *BRRCALL brrlog_priority_str(brrlog_priorityT level);
/* Get a debug string representation of the given level. */
BRRAPI const char *BRRCALL brrlog_priority_dbgstr(brrlog_priorityT level);

/* Get maximum number of bytes that can be logged at once. */
BRRAPI brrsz BRRCALL brrlog_logmax(void);
/* Set maximum number of bytes that can be logged at once.
 * Set to 0 to disable maximum.
 * If no error occurs, returns 0.
 * If an error occurs, returns -1.
 * MUST be called before using any log functions, acts as an initialization.
 * */
BRRAPI int BRRCALL brrlog_setlogmax(brrsz newmax);
/* Returns number of times logged. */
BRRAPI brru8 BRRCALL brrlog_count(void);
/* Resets log count to 0. */
BRRAPI void BRRCALL brrlog_resetcount(void);
/* Returns the current minimum loggable priority. */
BRRAPI brrlog_priorityT BRRCALL brrlog_minpriority(void);
/* Sets the minimum priority that will be logged.
 * Any priority less than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_setminpriority(brrlog_priorityT newmin);
/* Returns the current maximum loggable priority. */
BRRAPI brrlog_priorityT BRRCALL brrlog_maxpriority(void);
/* Sets the maximum priority that will be logged.
 * Any priority greater than the given will not be logged.
 * */
BRRAPI void BRRCALL brrlog_setmaxpriority(brrlog_priorityT newmax);

/* Initialize the log buffer.
 * The buffer is only used if 'logmax' isn't 0.
 * Run either this or 'brrlog_setlogmax' before trying to log.
 * If the log buffer was successfully initialized, 0 is returned.
 * If an error occurs, -1 is returned.
 * */
BRRAPI int BRRCALL brrlog_init(void);
/* Frees the log buffer used by brrlog. */
BRRAPI void BRRCALL brrlog_deinit(void);

#define _brrlog_log_params \
    brrlog_priorityT priority, brrlog_destinationT destination, const char *prefix, \
    brrlog_colorT foreground, brrlog_colorT background, brrlog_styleT style, brrlog_fontT font, \
    char *const buffer, int print_prefix, int print_newline, \
    const char *const file, const char *const function, brru8 line
BRRAPI brrsz BRRCALL brrlog_text(_brrlog_log_params,
    const char *const format, ...);
BRRAPI brrsz BRRCALL brrlog_digits(_brrlog_log_params,
    brru8 number, int base, int is_signed, char digit_separator, brrsz separator_spacing);
BRRAPI brrsz BRRCALL brrlog_bits(_brrlog_log_params,
    const void *const data, brrsz bits_to_print,
    int reverse_bytes, int reverse_bits, char bit_separator, brrsz separator_spacing);

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

BRRCPPEND

#endif /* BRRTOOLS_BRRLOG_H */
