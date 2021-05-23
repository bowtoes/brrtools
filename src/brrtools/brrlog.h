#ifndef BRRTOOLS_BRRLOG_H
#define BRRTOOLS_BRRLOG_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* ANSI colors for logging. */
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
/* Get a string representation of the given log color. */
BRRAPI const char *BRRCALL brrlog_fgcolor_str(brrlog_colorT foreground);
BRRAPI const char *BRRCALL brrlog_bgcolor_str(brrlog_colorT background);

/* Font used for log. */
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
/* Get a string representation of the given log font. */
BRRAPI const char *BRRCALL brrlog_font_str(brrlog_fontT font);

/* Text style used for log. */
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
/* Get a string representation of the given log style. */
BRRAPI const char *BRRCALL brrlog_style_str(brrlog_styleT style);

/* Log level used to determine whether the message should be logged. */
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
/* Get a string representation of the given log level. */
BRRAPI const char *BRRCALL brrlog_priority_str(brrlog_priorityT level);
BRRAPI const char *BRRCALL brrlog_priority_dbgstr(brrlog_priorityT level);

typedef enum brrlog_destination {
	brrlog_destination_last = -1,
	brrlog_destination_stdout,
	brrlog_destination_stderr,
} brrlog_destinationT;
typedef struct brrlog_level {
	brrlog_priorityT priority;
	brrlog_destinationT destination;
	const char *prefix;
} brrlog_levelT;

/* Struct for representing different logging text formats. */
typedef struct brrlog_format {
	brrlog_levelT level;      /* Format log level. */
	brrlog_colorT foreground; /* Format foreground color. */
	brrlog_colorT background; /* Format background color. */
	brrlog_styleT style;      /* Format text style. */
	brrlog_fontT  font;       /* Format format font. */
} brrlog_formatT;

/* Format for critical logs. */
extern BRRAPI brrlog_formatT brrlog_format_critical;
/* Format for error logs. */
extern BRRAPI brrlog_formatT brrlog_format_error;
/* Format for normal logs. */
extern BRRAPI brrlog_formatT brrlog_format_normal;
/* Format for warning logs. */
extern BRRAPI brrlog_formatT brrlog_format_warning;
/* Format for debug logs. */
extern BRRAPI brrlog_formatT brrlog_format_debug;
/* Clear format; unstyled log text. */
extern BRRAPI brrlog_formatT brrlog_format_clear;
/* Format storing last-used log style settings. */
extern BRRAPI brrlog_formatT brrlog_format_last;

/* Whether log output is styled. */
extern BRRAPI brrb1 brrlog_styleon;
/* Whether debug logs are enabled. */
extern BRRAPI brrb1 brrlog_debugon;
/* Whether automatic log flushing is done. */
extern BRRAPI brrb1 brrlog_flushon;
/* Whether file, function, and line information should also be logged. */
extern BRRAPI brrb1 brrlog_verbose;

/* Get maximum number of bytes that can be logged at once. */
BRRAPI brrsz BRRCALL brrlog_logmax(void);
/* Set maximum number of bytes that can be logged at once.
 * Set to 0 to disable maximum.
 * Returns 'true' if the max was successfully set, 'false' otherwise. */
BRRAPI brrb1 BRRCALL brrlog_setlogmax(brrsz newmax);
/* Sets the minimum priority that will be logged. Any priority less will not be logged. */
BRRAPI void BRRCALL brrlog_setminpriority(brrlog_priorityT newmin);
/* Returns the current minimum loggable priority. */
BRRAPI brrlog_priorityT BRRCALL brrlog_minpriority(void);
/* Sets the maximum priority that will be logged. Any priority greater will not be logged. */
BRRAPI void BRRCALL brrlog_setmaxpriority(brrlog_priorityT newmax);
/* Returns the current maximum loggable priority. */
BRRAPI brrlog_priorityT BRRCALL brrlog_maxpriority(void);
/* Returns number of times logged. */
BRRAPI brru8 BRRCALL brrlog_count(void);
/* Resets log count to 0. */
BRRAPI void BRRCALL brrlog_resetcount(void);

BRRAPI brrb1 BRRCALL brrlog_init(void);
BRRAPI void BRRCALL brrlog_deinit(void);
#define LOG_PARAMS \
	brrlog_priorityT priority, brrlog_destinationT destination, const char *const prefix, \
	brrlog_colorT foreground, brrlog_colorT background, brrlog_styleT style, brrlog_fontT font, \
	brrb1 print_prefix, brrb1 print_newline, const char *const file, const char *const function, brru8 line
BRRAPI brrsz BRRCALL brrlog_text(LOG_PARAMS, const char *const format, ...);
BRRAPI brrsz BRRCALL brrlog_digits(LOG_PARAMS, char digit_separator, brrsz separator_spacing,
    brrb1 is_signed, brru8 number, brru1 base);
BRRAPI brrsz BRRCALL brrlog_bits(LOG_PARAMS, brrb1 reverse_bytes, brrb1 reverse_bits,
    brrsz bits_to_print, const void *const data, char bit_separator, brrsz separator_spacing);

#undef LOG_PARAMS

/* ********************* */
/* MESSAGE */
/* ********************* */
#define BRRLOG_LOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, ...) \
	brrlog_text(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, \
	__FILE__, __func__, __LINE__, __VA_ARGS__)
#define BRRLOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_MESSAGEN(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_MESSAGEP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_MESSAGENP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_MESSAGE(priority, destination, prefix, foreground, background, style, font, 0, 0, __VA_ARGS__)
#define BRRLOG_LOG_MESSAGE_FORMAT(level, format, print_prefix, print_newline, ...) BRRLOG_LOG_MESSAGE(\
	level.priority, level.destination, level.prefix, \
	format.foreground, format.background, format.style, format.font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_MESSAGEF(level, format, ...)   BRRLOG_LOG_MESSAGE_FORMAT(level, format, 1, 1, __VA_ARGS__)
#define BRRLOG_MESSAGEFN(level, format, ...)  BRRLOG_LOG_MESSAGE_FORMAT(level, format, 1, 0, __VA_ARGS__)
#define BRRLOG_MESSAGEFP(level, format, ...)  BRRLOG_LOG_MESSAGE_FORMAT(level, format, 0, 1, __VA_ARGS__)
#define BRRLOG_MESSAGEFNP(level, format, ...) BRRLOG_LOG_MESSAGE_FORMAT(level, format, 0, 0, __VA_ARGS__)

#define BRRLOG_LOG_MESSAGE_STYLE(level, foreground, background, style, font, print_prefix, print_newline, ...) BRRLOG_LOG_MESSAGE(\
	level.priority, level.destination, level.prefix, \
	foreground, background, style, font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_MESSAGES(level, foreground, background, style, font, ...)   BRRLOG_LOG_MESSAGE_STYLE(level, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_MESSAGESN(level, foreground, background, style, font, ...)  BRRLOG_LOG_MESSAGE_STYLE(level, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_MESSAGESP(level, foreground, background, style, font, ...)  BRRLOG_LOG_MESSAGE_STYLE(level, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_MESSAGESNP(level, foreground, background, style, font, ...) BRRLOG_LOG_MESSAGE_STYLE(level, foreground, background, style, font, 0, 0, __VA_ARGS__)

#define BRRLOG_MESSAGE_FN(level, foreground, background, style, font, ...)   BRRLOG_MESSAGES(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_MESSAGE_FNN(level, foreground, background, style, font, ...)  BRRLOG_MESSAGESN(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_MESSAGE_FNP(level, foreground, background, style, font, ...)  BRRLOG_MESSAGESP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_MESSAGE_FNNP(level, foreground, background, style, font, ...) BRRLOG_MESSAGESNP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_MESSAGE_ST(level, foreground, background, style, ...)         BRRLOG_MESSAGE_FN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_STN(level, foreground, background, style, ...)        BRRLOG_MESSAGE_FNN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_STP(level, foreground, background, style, ...)        BRRLOG_MESSAGE_FNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_STNP(level, foreground, background, style, ...)       BRRLOG_MESSAGE_FNNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_BG(level, foreground, background, ...)                BRRLOG_MESSAGE_ST(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_BGN(level, foreground, background, ...)               BRRLOG_MESSAGE_STN(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_BGP(level, foreground, background, ...)               BRRLOG_MESSAGE_STP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_BGNP(level, foreground, background, ...)              BRRLOG_MESSAGE_STNP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_FG(level, foreground, ...)                            BRRLOG_MESSAGE_BG(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_FGN(level, foreground, ...)                           BRRLOG_MESSAGE_BGN(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_FGP(level, foreground, ...)                           BRRLOG_MESSAGE_BGP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_FGNP(level, foreground, ...)                          BRRLOG_MESSAGE_BGNP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_EM(level, ...)                                        BRRLOG_MESSAGE_FG(level, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_EMN(level, ...)                                       BRRLOG_MESSAGE_FGN(level, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_EMP(level, ...)                                       BRRLOG_MESSAGE_FGP(level, 0, __VA_ARGS__)
#define BRRLOG_MESSAGE_EMNP(level, ...)                                      BRRLOG_MESSAGE_FGNP(level, 0, __VA_ARGS__)

#define BRRLOG_MESSAGE_CRITICAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_MESSAGE_FORMAT(brrlog_format_critical.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_CRITICAL(...)   BRRLOG_MESSAGE_CRITICAL(1, 1, __VA_ARGS__)
#define BRRLOG_CRITICALN(...)  BRRLOG_MESSAGE_CRITICAL(1, 0, __VA_ARGS__)
#define BRRLOG_CRITICALP(...)  BRRLOG_MESSAGE_CRITICAL(0, 1, __VA_ARGS__)
#define BRRLOG_CRITICALNP(...) BRRLOG_MESSAGE_CRITICAL(0, 0, __VA_ARGS__)
#define BRRLOG_CRI   BRRLOG_CRITICAL
#define BRRLOG_CRIN  BRRLOG_CRITICALN
#define BRRLOG_CRIP  BRRLOG_CRITICALP
#define BRRLOG_CRINP BRRLOG_CRITICALNP
#define BRRLOG_MESSAGE_ERROR(print_prefix, print_newline, ...) \
	BRRLOG_LOG_MESSAGE_FORMAT(brrlog_format_error.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_ERROR(...)   BRRLOG_MESSAGE_ERROR(1, 1, __VA_ARGS__)
#define BRRLOG_ERRORN(...)  BRRLOG_MESSAGE_ERROR(1, 0, __VA_ARGS__)
#define BRRLOG_ERRORP(...)  BRRLOG_MESSAGE_ERROR(0, 1, __VA_ARGS__)
#define BRRLOG_ERRORNP(...) BRRLOG_MESSAGE_ERROR(0, 0, __VA_ARGS__)
#define BRRLOG_ERR   BRRLOG_ERROR
#define BRRLOG_ERRN  BRRLOG_ERRORN
#define BRRLOG_ERRP  BRRLOG_ERRORP
#define BRRLOG_ERRNP BRRLOG_ERRORNP
#define BRRLOG_MESSAGE_NORMAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_MESSAGE_FORMAT(brrlog_format_normal.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_NORMAL(...)   BRRLOG_MESSAGE_NORMAL(1, 1, __VA_ARGS__)
#define BRRLOG_NORMALN(...)  BRRLOG_MESSAGE_NORMAL(1, 0, __VA_ARGS__)
#define BRRLOG_NORMALP(...)  BRRLOG_MESSAGE_NORMAL(0, 1, __VA_ARGS__)
#define BRRLOG_NORMALNP(...) BRRLOG_MESSAGE_NORMAL(0, 0, __VA_ARGS__)
#define BRRLOG_NOR   BRRLOG_NORMAL
#define BRRLOG_NORN  BRRLOG_NORMALN
#define BRRLOG_NORP  BRRLOG_NORMALP
#define BRRLOG_NORNP BRRLOG_NORMALNP
#define BRRLOG_MESSAGE_WARNING(print_prefix, print_newline, ...) \
	BRRLOG_LOG_MESSAGE_FORMAT(brrlog_format_warning.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_WARNING(...)   BRRLOG_MESSAGE_WARNING(1, 1, __VA_ARGS__)
#define BRRLOG_WARNINGN(...)  BRRLOG_MESSAGE_WARNING(1, 0, __VA_ARGS__)
#define BRRLOG_WARNINGP(...)  BRRLOG_MESSAGE_WARNING(0, 1, __VA_ARGS__)
#define BRRLOG_WARNINGNP(...) BRRLOG_MESSAGE_WARNING(0, 0, __VA_ARGS__)
#define BRRLOG_WAR   BRRLOG_WARNING
#define BRRLOG_WARN  BRRLOG_WARNINGN
#define BRRLOG_WARP  BRRLOG_WARNINGP
#define BRRLOG_WARNP BRRLOG_WARNINGNP
#define BRRLOG_MESSAGE_DEBUG(print_prefix, print_newline, ...) \
	BRRLOG_LOG_MESSAGE_FORMAT(brrlog_format_debug.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_DEBUG(...)   BRRLOG_MESSAGE_DEBUG(1, 1, __VA_ARGS__)
#define BRRLOG_DEBUGN(...)  BRRLOG_MESSAGE_DEBUG(1, 0, __VA_ARGS__)
#define BRRLOG_DEBUGP(...)  BRRLOG_MESSAGE_DEBUG(0, 1, __VA_ARGS__)
#define BRRLOG_DEBUGNP(...) BRRLOG_MESSAGE_DEBUG(0, 0, __VA_ARGS__)
#define BRRLOG_DEB   BRRLOG_DEBUG
#define BRRLOG_DEBN  BRRLOG_DEBUGN
#define BRRLOG_DEBP  BRRLOG_DEBUGP
#define BRRLOG_DEBNP BRRLOG_DEBUGNP
/* ********************* */
/* DIGITS */
/* ********************* */
#define BRRLOG_LOG_DIGITS(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, ...) \
	brrlog_digits(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, \
	__FILE__, __func__, __LINE__, __VA_ARGS__)
#define BRRLOG_DIGITS(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_DIGITS(priority, destination, prefix, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_DIGITSN(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_DIGITS(priority, destination, prefix, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_DIGITSP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_DIGITS(priority, destination, prefix, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_DIGITSNP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_DIGITS(priority, destination, prefix, foreground, background, style, font, 0, 0, __VA_ARGS__)
#define BRRLOG_LOG_DIGITS_FORMAT(level, format, print_prefix, print_newline, ...) BRRLOG_LOG_DIGITS(\
	level.priority, level.destination, level.prefix, \
	format.foreground, format.background, format.style, format.font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_DIGITSF(level, format, ...)   BRRLOG_LOG_DIGITS_FORMAT(level, format, 1, 1, __VA_ARGS__)
#define BRRLOG_DIGITSFN(level, format, ...)  BRRLOG_LOG_DIGITS_FORMAT(level, format, 1, 0, __VA_ARGS__)
#define BRRLOG_DIGITSFP(level, format, ...)  BRRLOG_LOG_DIGITS_FORMAT(level, format, 0, 1, __VA_ARGS__)
#define BRRLOG_DIGITSFNP(level, format, ...) BRRLOG_LOG_DIGITS_FORMAT(level, format, 0, 0, __VA_ARGS__)

#define BRRLOG_LOG_DIGITS_STYLE(level, foreground, background, style, font, print_prefix, print_newline, ...) BRRLOG_LOG_DIGITS(\
	level.priority, level.destination, level.prefix, \
	foreground, background, style, font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_DIGITSS(level, foreground, background, style, font, ...)   BRRLOG_LOG_DIGITS_STYLE(level, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_DIGITSSN(level, foreground, background, style, font, ...)  BRRLOG_LOG_DIGITS_STYLE(level, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_DIGITSSP(level, foreground, background, style, font, ...)  BRRLOG_LOG_DIGITS_STYLE(level, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_DIGITSSNP(level, foreground, background, style, font, ...) BRRLOG_LOG_DIGITS_STYLE(level, foreground, background, style, font, 0, 0, __VA_ARGS__)

#define BRRLOG_DIGITS_FN(level, foreground, background, style, font, ...)   BRRLOG_DIGITSS(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_DIGITS_FNN(level, foreground, background, style, font, ...)  BRRLOG_DIGITSSN(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_DIGITS_FNP(level, foreground, background, style, font, ...)  BRRLOG_DIGITSSP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_DIGITS_FNNP(level, foreground, background, style, font, ...) BRRLOG_DIGITSSNP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_DIGITS_ST(level, foreground, background, style, ...)         BRRLOG_DIGITS_FN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_STN(level, foreground, background, style, ...)        BRRLOG_DIGITS_FNN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_STP(level, foreground, background, style, ...)        BRRLOG_DIGITS_FNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_STNP(level, foreground, background, style, ...)       BRRLOG_DIGITS_FNNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_BG(level, foreground, background, ...)                BRRLOG_DIGITS_ST(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_BGN(level, foreground, background, ...)               BRRLOG_DIGITS_STN(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_BGP(level, foreground, background, ...)               BRRLOG_DIGITS_STP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_BGNP(level, foreground, background, ...)              BRRLOG_DIGITS_STNP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FG(level, foreground, ...)                            BRRLOG_DIGITS_BG(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FGN(level, foreground, ...)                           BRRLOG_DIGITS_BGN(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FGP(level, foreground, ...)                           BRRLOG_DIGITS_BGP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_FGNP(level, foreground, ...)                          BRRLOG_DIGITS_BGNP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_EM(level, ...)                                        BRRLOG_DIGITS_FG(level, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_EMN(level, ...)                                       BRRLOG_DIGITS_FGN(level, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_EMP(level, ...)                                       BRRLOG_DIGITS_FGP(level, 0, __VA_ARGS__)
#define BRRLOG_DIGITS_EMNP(level, ...)                                      BRRLOG_DIGITS_FGNP(level, 0, __VA_ARGS__)

#define BRRLOG_DIGITS_CRITICAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_DIGITS_FORMAT(brrlog_format_critical.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGD_CRITICAL(...)   BRRLOG_DIGITS_CRITICAL(1, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALN(...)  BRRLOG_DIGITS_CRITICAL(1, 0, __VA_ARGS__)
#define BRRLOGD_CRITICALP(...)  BRRLOG_DIGITS_CRITICAL(0, 1, __VA_ARGS__)
#define BRRLOGD_CRITICALNP(...) BRRLOG_DIGITS_CRITICAL(0, 0, __VA_ARGS__)
#define BRRLOGD_CRI   BRRLOGD_CRITICAL
#define BRRLOGD_CRIN  BRRLOGD_CRITICALN
#define BRRLOGD_CRIP  BRRLOGD_CRITICALP
#define BRRLOGD_CRINP BRRLOGD_CRITICALNP
#define BRRLOG_DIGITS_ERROR(print_prefix, print_newline, ...) \
	BRRLOG_LOG_DIGITS_FORMAT(brrlog_format_error.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGD_ERROR(...)   BRRLOG_DIGITS_ERROR(1, 1, __VA_ARGS__)
#define BRRLOGD_ERRORN(...)  BRRLOG_DIGITS_ERROR(1, 0, __VA_ARGS__)
#define BRRLOGD_ERRORP(...)  BRRLOG_DIGITS_ERROR(0, 1, __VA_ARGS__)
#define BRRLOGD_ERRORNP(...) BRRLOG_DIGITS_ERROR(0, 0, __VA_ARGS__)
#define BRRLOGD_ERR   BRRLOGD_ERROR
#define BRRLOGD_ERRN  BRRLOGD_ERRORN
#define BRRLOGD_ERRP  BRRLOGD_ERRORP
#define BRRLOGD_ERRNP BRRLOGD_ERRORNP
#define BRRLOG_DIGITS_NORMAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_DIGITS_FORMAT(brrlog_format_normal.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGD_NORMAL(...)   BRRLOG_DIGITS_NORMAL(1, 1, __VA_ARGS__)
#define BRRLOGD_NORMALN(...)  BRRLOG_DIGITS_NORMAL(1, 0, __VA_ARGS__)
#define BRRLOGD_NORMALP(...)  BRRLOG_DIGITS_NORMAL(0, 1, __VA_ARGS__)
#define BRRLOGD_NORMALNP(...) BRRLOG_DIGITS_NORMAL(0, 0, __VA_ARGS__)
#define BRRLOGD_NOR   BRRLOGD_NORMAL
#define BRRLOGD_NORN  BRRLOGD_NORMALN
#define BRRLOGD_NORP  BRRLOGD_NORMALP
#define BRRLOGD_NORNP BRRLOGD_NORMALNP
#define BRRLOG_DIGITS_WARNING(print_prefix, print_newline, ...) \
	BRRLOG_LOG_DIGITS_FORMAT(brrlog_format_warning.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGD_WARNING(...)   BRRLOG_DIGITS_WARNING(1, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGN(...)  BRRLOG_DIGITS_WARNING(1, 0, __VA_ARGS__)
#define BRRLOGD_WARNINGP(...)  BRRLOG_DIGITS_WARNING(0, 1, __VA_ARGS__)
#define BRRLOGD_WARNINGNP(...) BRRLOG_DIGITS_WARNING(0, 0, __VA_ARGS__)
#define BRRLOGD_WAR   BRRLOGD_WARNING
#define BRRLOGD_WARN  BRRLOGD_WARNINGN
#define BRRLOGD_WARP  BRRLOGD_WARNINGP
#define BRRLOGD_WARNP BRRLOGD_WARNINGNP
#define BRRLOG_DIGITS_DEBUG(print_prefix, print_newline, ...) \
	BRRLOG_LOG_DIGITS_FORMAT(brrlog_format_debug.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGD_DEBUG(...)   BRRLOG_DIGITS_DEBUG(1, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGN(...)  BRRLOG_DIGITS_DEBUG(1, 0, __VA_ARGS__)
#define BRRLOGD_DEBUGP(...)  BRRLOG_DIGITS_DEBUG(0, 1, __VA_ARGS__)
#define BRRLOGD_DEBUGNP(...) BRRLOG_DIGITS_DEBUG(0, 0, __VA_ARGS__)
#define BRRLOGD_DEB   BRRLOGD_DEBUG
#define BRRLOGD_DEBN  BRRLOGD_DEBUGN
#define BRRLOGD_DEBP  BRRLOGD_DEBUGP
#define BRRLOGD_DEBNP BRRLOGD_DEBUGNP
/* ********************* */
/* BITS */
/* ********************* */
#define BRRLOG_LOG_BITS(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, ...) \
	brrlog_bits(priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, \
	__FILE__, __func__, __LINE__, __VA_ARGS__)
#define BRRLOG_BITS(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_BITS(priority, destination, prefix, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_BITSN(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_BITS(priority, destination, prefix, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_BITSP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_BITS(priority, destination, prefix, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_BITSNP(priority, destination, prefix, foreground, background, style, font, ...) \
	BRRLOG_LOG_BITS(priority, destination, prefix, foreground, background, style, font, 0, 0, __VA_ARGS__)
#define BRRLOG_LOG_BITS_FORMAT(level, format, print_prefix, print_newline, ...) BRRLOG_LOG_BITS(\
	level.priority, level.destination, level.prefix, \
	format.foreground, format.background, format.style, format.font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_BITSF(level, format, ...)   BRRLOG_LOG_BITS_FORMAT(level, format, 1, 1, __VA_ARGS__)
#define BRRLOG_BITSFN(level, format, ...)  BRRLOG_LOG_BITS_FORMAT(level, format, 1, 0, __VA_ARGS__)
#define BRRLOG_BITSFP(level, format, ...)  BRRLOG_LOG_BITS_FORMAT(level, format, 0, 1, __VA_ARGS__)
#define BRRLOG_BITSFNP(level, format, ...) BRRLOG_LOG_BITS_FORMAT(level, format, 0, 0, __VA_ARGS__)

#define BRRLOG_LOG_BITS_STYLE(level, foreground, background, style, font, print_prefix, print_newline, ...) BRRLOG_LOG_BITS(\
	level.priority, level.destination, level.prefix, \
	foreground, background, style, font, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOG_BITSS(level, foreground, background, style, font, ...)   BRRLOG_LOG_BITS_STYLE(level, foreground, background, style, font, 1, 1, __VA_ARGS__)
#define BRRLOG_BITSSN(level, foreground, background, style, font, ...)  BRRLOG_LOG_BITS_STYLE(level, foreground, background, style, font, 1, 0, __VA_ARGS__)
#define BRRLOG_BITSSP(level, foreground, background, style, font, ...)  BRRLOG_LOG_BITS_STYLE(level, foreground, background, style, font, 0, 1, __VA_ARGS__)
#define BRRLOG_BITSSNP(level, foreground, background, style, font, ...) BRRLOG_LOG_BITS_STYLE(level, foreground, background, style, font, 0, 0, __VA_ARGS__)

#define BRRLOG_BITS_FN(level, foreground, background, style, font, ...)   BRRLOG_BITSS(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_BITS_FNN(level, foreground, background, style, font, ...)  BRRLOG_BITSSN(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_BITS_FNP(level, foreground, background, style, font, ...)  BRRLOG_BITSSP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_BITS_FNNP(level, foreground, background, style, font, ...) BRRLOG_BITSSNP(level, foreground, background, style, font, __VA_ARGS__)
#define BRRLOG_BITS_ST(level, foreground, background, style, ...)         BRRLOG_BITS_FN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_BITS_STN(level, foreground, background, style, ...)        BRRLOG_BITS_FNN(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_BITS_STP(level, foreground, background, style, ...)        BRRLOG_BITS_FNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_BITS_STNP(level, foreground, background, style, ...)       BRRLOG_BITS_FNNP(level, foreground, background, style, 0, __VA_ARGS__)
#define BRRLOG_BITS_BG(level, foreground, background, ...)                BRRLOG_BITS_ST(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_BITS_BGN(level, foreground, background, ...)               BRRLOG_BITS_STN(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_BITS_BGP(level, foreground, background, ...)               BRRLOG_BITS_STP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_BITS_BGNP(level, foreground, background, ...)              BRRLOG_BITS_STNP(level, foreground, background, 0, __VA_ARGS__)
#define BRRLOG_BITS_FG(level, foreground, ...)                            BRRLOG_BITS_BG(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_BITS_FGN(level, foreground, ...)                           BRRLOG_BITS_BGN(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_BITS_FGP(level, foreground, ...)                           BRRLOG_BITS_BGP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_BITS_FGNP(level, foreground, ...)                          BRRLOG_BITS_BGNP(level, foreground, 0, __VA_ARGS__)
#define BRRLOG_BITS_EM(level, ...)                                        BRRLOG_BITS_FG(level, 0, __VA_ARGS__)
#define BRRLOG_BITS_EMN(level, ...)                                       BRRLOG_BITS_FGN(level, 0, __VA_ARGS__)
#define BRRLOG_BITS_EMP(level, ...)                                       BRRLOG_BITS_FGP(level, 0, __VA_ARGS__)
#define BRRLOG_BITS_EMNP(level, ...)                                      BRRLOG_BITS_FGNP(level, 0, __VA_ARGS__)

#define BRRLOG_BITS_CRITICAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_BITS_FORMAT(brrlog_format_critical.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGB_CRITICAL(...)   BRRLOG_BITS_CRITICAL(1, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALN(...)  BRRLOG_BITS_CRITICAL(1, 0, __VA_ARGS__)
#define BRRLOGB_CRITICALP(...)  BRRLOG_BITS_CRITICAL(0, 1, __VA_ARGS__)
#define BRRLOGB_CRITICALNP(...) BRRLOG_BITS_CRITICAL(0, 0, __VA_ARGS__)
#define BRRLOGB_CRI   BRRLOGB_CRITICAL
#define BRRLOGB_CRIN  BRRLOGB_CRITICALN
#define BRRLOGB_CRIP  BRRLOGB_CRITICALP
#define BRRLOGB_CRINP BRRLOGB_CRITICALNP
#define BRRLOG_BITS_ERROR(print_prefix, print_newline, ...) \
	BRRLOG_LOG_BITS_FORMAT(brrlog_format_error.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGB_ERROR(...)   BRRLOG_BITS_ERROR(1, 1, __VA_ARGS__)
#define BRRLOGB_ERRORN(...)  BRRLOG_BITS_ERROR(1, 0, __VA_ARGS__)
#define BRRLOGB_ERRORP(...)  BRRLOG_BITS_ERROR(0, 1, __VA_ARGS__)
#define BRRLOGB_ERRORNP(...) BRRLOG_BITS_ERROR(0, 0, __VA_ARGS__)
#define BRRLOGB_ERR   BRRLOGB_ERROR
#define BRRLOGB_ERRN  BRRLOGB_ERRORN
#define BRRLOGB_ERRP  BRRLOGB_ERRORP
#define BRRLOGB_ERRNP BRRLOGB_ERRORNP
#define BRRLOG_BITS_NORMAL(print_prefix, print_newline, ...) \
	BRRLOG_LOG_BITS_FORMAT(brrlog_format_normal.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGB_NORMAL(...)   BRRLOG_BITS_NORMAL(1, 1, __VA_ARGS__)
#define BRRLOGB_NORMALN(...)  BRRLOG_BITS_NORMAL(1, 0, __VA_ARGS__)
#define BRRLOGB_NORMALP(...)  BRRLOG_BITS_NORMAL(0, 1, __VA_ARGS__)
#define BRRLOGB_NORMALNP(...) BRRLOG_BITS_NORMAL(0, 0, __VA_ARGS__)
#define BRRLOGB_NOR   BRRLOGB_NORMAL
#define BRRLOGB_NORN  BRRLOGB_NORMALN
#define BRRLOGB_NORP  BRRLOGB_NORMALP
#define BRRLOGB_NORNP BRRLOGB_NORMALNP
#define BRRLOG_BITS_WARNING(print_prefix, print_newline, ...) \
	BRRLOG_LOG_BITS_FORMAT(brrlog_format_warning.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGB_WARNING(...)   BRRLOG_BITS_WARNING(1, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGN(...)  BRRLOG_BITS_WARNING(1, 0, __VA_ARGS__)
#define BRRLOGB_WARNINGP(...)  BRRLOG_BITS_WARNING(0, 1, __VA_ARGS__)
#define BRRLOGB_WARNINGNP(...) BRRLOG_BITS_WARNING(0, 0, __VA_ARGS__)
#define BRRLOGB_WAR   BRRLOGB_WARNING
#define BRRLOGB_WARN  BRRLOGB_WARNINGN
#define BRRLOGB_WARP  BRRLOGB_WARNINGP
#define BRRLOGB_WARNP BRRLOGB_WARNINGNP
#define BRRLOG_BITS_DEBUG(print_prefix, print_newline, ...) \
	BRRLOG_LOG_BITS_FORMAT(brrlog_format_debug.level, brrlog_format_normal, print_prefix, print_newline, __VA_ARGS__)
#define BRRLOGB_DEBUG(...)   BRRLOG_BITS_DEBUG(1, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGN(...)  BRRLOG_BITS_DEBUG(1, 0, __VA_ARGS__)
#define BRRLOGB_DEBUGP(...)  BRRLOG_BITS_DEBUG(0, 1, __VA_ARGS__)
#define BRRLOGB_DEBUGNP(...) BRRLOG_BITS_DEBUG(0, 0, __VA_ARGS__)
#define BRRLOGB_DEB   BRRLOGB_DEBUG
#define BRRLOGB_DEBN  BRRLOGB_DEBUGN
#define BRRLOGB_DEBP  BRRLOGB_DEBUGP
#define BRRLOGB_DEBNP BRRLOGB_DEBUGNP

BRRCPPEND

#endif /* BRRTOOLS_BRRLOG_H */
