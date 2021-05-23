#include "brrtools/brrlog.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"

#if defined(BRRPLATFORMTYPE_WINDOWS)
static const char *const clrstyle = "";
static char fmtstr[] = "";
#else
static const char *const clrstyle = "\x1b[000;039;049;010m";
static char fmtstr[] = "\x1b[000;039;049;010m";
#endif

#if !defined(BRRPLATFORMTYPE_WINDOWS)
static int BRRCALL fgid(brrlog_colorT fg) {
	switch (fg) {
		case brrlog_color_black     : return 30; case brrlog_color_red         : return 31;
		case brrlog_color_green     : return 32; case brrlog_color_yellow      : return 33;
		case brrlog_color_blue      : return 34; case brrlog_color_magenta     : return 35;
		case brrlog_color_cyan      : return 36; case brrlog_color_white       : return 37;
		case brrlog_color_darkgrey  : return 90; case brrlog_color_lightred    : return 91;
		case brrlog_color_lightgreen: return 92; case brrlog_color_lightyellow : return 93;
		case brrlog_color_lightblue : return 94; case brrlog_color_lightmagenta: return 95;
		case brrlog_color_lightcyan : return 96; case brrlog_color_lightwhite  : return 97;
		default: return 39;
	}
}
static int BRRCALL bgid(brrlog_colorT bg) {
	return 10 + fgid(bg);
}
static int BRRCALL stid(brrlog_styleT st) {
	switch (st) {
		case brrlog_style_normal     : return  0; case brrlog_style_bold        : return  1;
		case brrlog_style_dim        : return  2; case brrlog_style_italics     : return  3;
		case brrlog_style_under      : return  4; case brrlog_style_blink       : return  5;
		case brrlog_style_fastblink  : return  6; case brrlog_style_reverse     : return  7;
		case brrlog_style_conceal    : return  8; case brrlog_style_strikeout   : return  9;
		case brrlog_style_fraktur    : return 20; case brrlog_style_nobold      : return 21;
		case brrlog_style_nobright   : return 22; case brrlog_style_noitalics   : return 23;
		case brrlog_style_nounder    : return 24; case brrlog_style_noblink     : return 25;
		case brrlog_style_noreverse  : return 27; case brrlog_style_reveal      : return 28;
		case brrlog_style_nostrikeout: return 29; case brrlog_style_frame       : return 51;
		case brrlog_style_circle     : return 52; case brrlog_style_over        : return 53;
		case brrlog_style_noframe    : return 54; case brrlog_style_noover      : return 55;
		case brrlog_style_iunder     : return 60; case brrlog_style_idoubleunder: return 61;
		case brrlog_style_iover      : return 62; case brrlog_style_idoubleover : return 63;
		case brrlog_style_istress    : return 64; case brrlog_style_ioff        : return 65;
		default: return 0;
	}
}
static int BRRCALL fnid(brrlog_fontT fn) {
	fn = fn<0?0:fn>=brrlog_font_count?brrlog_font_count-1:fn;
	return fn + 10;
}
#endif
static char *BRRCALL updatefmtstr(brrlog_colorT fg, brrlog_colorT bg, brrlog_styleT st, brrlog_fontT fn) {
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	snprintf(fmtstr, 19, "\x1b[%03i;%03i;%03i;%03im",
			stid(st),
			fgid(fg),
			bgid(bg),
			fnid(fn));
#endif
	return fmtstr;
}

#define ENUM_COLORS \
	TOKEN(normal) \
	TOKEN (black)     TOKEN (red)          TOKEN (green)      TOKEN (yellow) \
	TOKEN (blue)      TOKEN (magenta)      TOKEN (cyan)       TOKEN (white) \
	TOKEN (darkgrey)  TOKEN (lightred)     TOKEN (lightgreen) TOKEN (lightyellow) \
	TOKEN (lightblue) TOKEN (lightmagenta) TOKEN (lightcyan)  TOKEN (lightwhite)
#define ENUM_FONTS \
	TOKEN(normal) \
	TOKEN(1) TOKEN(2) TOKEN(3) TOKEN(4) TOKEN(5) TOKEN(6) TOKEN(7) TOKEN(8) TOKEN(9)
#define ENUM_STYLES \
	TOKEN(normal) \
	TOKEN (bold)         TOKEN (dim)         TOKEN (italics)     TOKEN (under) \
	TOKEN (blink)        TOKEN (fastblink)   TOKEN (reverse)     TOKEN (conceal) \
	TOKEN (strikeout)    TOKEN (fraktur)     TOKEN (nobold)      TOKEN (nobright) \
	TOKEN (noitalics)    TOKEN (nounder)     TOKEN (noblink)     TOKEN (noreverse) \
	TOKEN (reveal)       TOKEN (nostrikeout) TOKEN (frame)       TOKEN (circle) \
	TOKEN (over)         TOKEN (noframe)     TOKEN (noover)      TOKEN (iunder) \
	TOKEN (idoubleunder) TOKEN (iover)       TOKEN (idoubleover) TOKEN (istress) \
	TOKEN(ioff)
#define ENUM_LEVELS \
	TOKEN(none) \
	TOKEN(critical) \
	TOKEN(error) \
	TOKEN(normal) \
	TOKEN(warning) \
	TOKEN(debug)

#define RET_CLP_LST(x, s) return s[BRRLIB_DUM_CLAMP(x, brrlog_##x##_last, brrlog_##x##_count) + 1]
static const char *const fgcolor_strs[brrlog_color_count + 2] = {
	"last",
	#define TOKEN(x) #x,
	ENUM_COLORS
	#undef TOKEN
	"invalid",
};
const char *BRRCALL brrlog_fgcolor_str(brrlog_colorT color)
{RET_CLP_LST(color, fgcolor_strs);}
static const char *const bgcolor_strs[brrlog_color_count + 2] = {
	"last",
	#define TOKEN(x) #x,
	ENUM_COLORS
	#undef TOKEN
	"invalid",
};
const char *BRRCALL brrlog_bgcolor_str(brrlog_colorT color)
{RET_CLP_LST(color, bgcolor_strs);}

static const char *const font_strs[brrlog_font_count + 2] = {
	"last",
	#define TOKEN(x) #x,
	ENUM_FONTS
	#undef TOKEN
	"invalid",
};
const char *BRRCALL brrlog_font_str(brrlog_fontT font)
{RET_CLP_LST(font, font_strs);}

static const char *const style_strs[brrlog_style_count + 2] = {
	"last",
	#define TOKEN(x) #x,
	ENUM_STYLES
	#undef TOKEN
	"invalid",
};
const char *BRRCALL brrlog_style_str(brrlog_styleT style)
{RET_CLP_LST(style, style_strs);}

static const char *const priority_strs[brrlog_priority_count + 2] = {
	"last",
	#define TOKEN(x) #x,
	ENUM_LEVELS
	#undef TOKEN
	"invalid",
};
const char *BRRCALL brrlog_priority_str(brrlog_priorityT priority)
{RET_CLP_LST(priority, priority_strs);}
static const char *const priority_dbgstrs[brrlog_priority_count + 2] = {
	"LST",
	"NON",
	"CRI",
	"ERR",
	"NOR",
	"WAR",
	"DEB",
	"INV",
};
const char *BRRCALL brrlog_priority_dbgstr(brrlog_priorityT priority)
{RET_CLP_LST(priority, priority_dbgstrs);}

#define criformat {{brrlog_priority_critical, brrlog_destination_stderr, "CRITICAL: "}, brrlog_color_red,    brrlog_color_normal, brrlog_style_reverse,   brrlog_font_normal}
#define errformat {{brrlog_priority_error,    brrlog_destination_stderr, "Error: "   }, brrlog_color_red,    brrlog_color_normal, brrlog_style_bold,      brrlog_font_normal}
#define norformat {{brrlog_priority_normal,   brrlog_destination_stdout, ""          }, brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,    brrlog_font_normal}
#define warformat {{brrlog_priority_warning,  brrlog_destination_stderr, "Warning: " }, brrlog_color_yellow, brrlog_color_normal, brrlog_style_normal,    brrlog_font_normal}
#define debformat {{brrlog_priority_debug,    brrlog_destination_stderr, "DEBUG: "   }, brrlog_color_yellow, brrlog_color_normal, brrlog_style_reverse,   brrlog_font_normal}
#define clrformat {{brrlog_priority_normal,   brrlog_destination_stdout, ""          }, brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,    brrlog_font_normal}

brrlog_formatT brrlog_format_critical = criformat;
brrlog_formatT brrlog_format_error    = errformat;
brrlog_formatT brrlog_format_normal   = norformat;
brrlog_formatT brrlog_format_warning  = warformat;
brrlog_formatT brrlog_format_debug    = debformat;
brrlog_formatT brrlog_format_clear    = clrformat;
brrlog_formatT brrlog_format_last     = {{brrlog_priority_normal, brrlog_destination_stdout, "" }, brrlog_color_normal, brrlog_color_normal, brrlog_style_normal, brrlog_font_normal };

#if defined(BRRPLATFORMTYPE_WINDOWS)
brrb1 brrlog_styleon = false;
#else
brrb1 brrlog_styleon = true;
#endif
brrb1 brrlog_debugon = false;
brrb1 brrlog_flushon = true;
brrb1 brrlog_verbose = false;

static FILE *lastlocation = NULL;

static brrlog_priorityT minpri = brrlog_priority_none+1;
static brrlog_priorityT maxpri = brrlog_priority_count-1;
void BRRCALL
brrlog_setminpriority(brrlog_priorityT newmin)
{
	minpri = newmin > brrlog_priority_none ?
		newmin < brrlog_priority_count ?
			newmin > maxpri?maxpri:newmin :
		brrlog_priority_count-1 :
	brrlog_priority_none+1;
}
brrlog_priorityT BRRCALL brrlog_minpriority(void) {return minpri;}

void BRRCALL
brrlog_setmaxpriority(brrlog_priorityT newmax)
{
	maxpri = newmax > brrlog_priority_none ?
		newmax < brrlog_priority_count ?
			newmax < minpri?minpri:newmax :
		brrlog_priority_count-1 :
	brrlog_priority_none+1;
}
brrlog_priorityT BRRCALL
brrlog_maxpriority(void) {return maxpri;}

static brru8 logcount = 0;
brru8 BRRCALL brrlog_count(void) {return logcount;}
void BRRCALL brrlog_resetcount(void) {logcount = 0;}

static brrsz logmax = 2048;
static char *buffer = NULL;

brrsz BRRCALL brrlog_logmax(void) {return logmax;}

brrb1 BRRCALL
brrlog_setlogmax(brrsz newmax)
{
	brrb1 res = true;
	if (newmax != logmax) {
		res = brrlib_alloc((void **)&buffer, newmax, 1);
		if (res)
			logmax = newmax;
	}
	return res;
}

brrb1 BRRCALL
brrlog_init(void)
{
	brrb1 res = true;
	if (!buffer && logmax) {
		res = brrlib_alloc((void **)&buffer, logmax, 1);
	}
	return res;
}

void BRRCALL
brrlog_deinit(void)
{
	if (buffer) {
		free(buffer);
		buffer = NULL;
	}
}

static void BRRCALL vnomaxprint(brrsz *const wrt, FILE *dst, const char *const fmt, va_list lptr) {
	*wrt += vfprintf(dst, fmt, lptr);
}
static void BRRCALL nomaxprint(brrsz *const wrt, FILE *dst, const char *const fmt, ...) {
	va_list lptr;
	va_start(lptr, fmt);
	vnomaxprint(wrt, dst, fmt, lptr);
	va_end(lptr);
}
static void BRRCALL vmaxprint(brrsz *const wrt, FILE *dst, const char *const fmt, va_list lptr) {
	brrof tmp;
	if (logmax < *wrt)
		*wrt = logmax;
	tmp = vsnprintf(buffer + *wrt, logmax - *wrt, fmt, lptr);\
	if (tmp > 0) *wrt += ((brrsz)tmp) < logmax ? ((brrsz)tmp) : logmax - *wrt - 1;\
}
static void BRRCALL maxprint(brrsz *const wrt, FILE *dst, const char *const fmt, ...) {
	va_list lptr;
	va_start(lptr, fmt);
	vmaxprint(wrt, dst, fmt, lptr);
	va_end(lptr);
}

#define LOG_PARAMS \
	brrlog_priorityT priority, brrlog_destinationT destination, const char *prefix, \
    brrlog_colorT foreground, brrlog_colorT background, brrlog_styleT style, brrlog_fontT font, \
    brrb1 print_prefix, brrb1 print_newline, const char *const file, const char *const function, brru8 line
#define LOG_ARGS \
	priority, destination, prefix, foreground, background, style, font, print_prefix, print_newline, file, function, line
brrsz BRRCALL
brrlog_text(LOG_PARAMS, const char *const format, ...)
{
	va_list lptr;
	brrsz write = 0;
	FILE *dst = NULL;
	brrlog_formatT prifmt;
	void (*vprint)(brrsz *const wrt, FILE *dst, const char *const fmt, va_list lptr) = NULL;
	void (*print)(brrsz *const wrt, FILE *dst, const char *const fmt, ...) = NULL;

	if (priority <= brrlog_priority_last)
		priority = brrlog_format_last.level.priority;

	if (!format ||
	    (priority < minpri || priority > maxpri))
		return 0;
#ifndef BRRTOOLS_DEBUG
	if (!brrlog_debugon && priority == brrlog_priority_debug)
		return 0;
#endif // If defined, never quit on debug priority;

	if (destination <= brrlog_destination_last) destination = brrlog_format_last.level.destination;
	if (foreground  <= brrlog_color_last)       foreground  = brrlog_format_last.foreground;
	if (background  <= brrlog_color_last)       background  = brrlog_format_last.background;
	if (style       <= brrlog_style_last)       style       = brrlog_format_last.style;
	if (font        <= brrlog_font_last)        font        = brrlog_format_last.font;

	if (logmax) {
		vprint = vmaxprint;
		print = maxprint;
	} else { vprint = vnomaxprint;
		print = nomaxprint;
	}

	switch (destination) {
		case brrlog_destination_stdout: dst = stdout; break;
		case brrlog_destination_stderr: dst = stderr; break;
		default: destination = brrlog_destination_stdout; dst = stdout;
	}
	switch (priority) {
		case brrlog_priority_critical: prifmt = brrlog_format_critical; break;
		case brrlog_priority_error: prifmt = brrlog_format_error; break;
		case brrlog_priority_normal: prifmt = brrlog_format_normal; break;
		case brrlog_priority_warning: prifmt = brrlog_format_warning; break;
		case brrlog_priority_debug: prifmt = brrlog_format_debug; break;
		default:prifmt = brrlog_format_clear;
	}

	if (brrlog_flushon) {
		if (lastlocation != dst) {
			fflush(lastlocation);
			lastlocation = dst;
		}
	}

	if (print_prefix) {
		if (prefix == NULL)
			prefix = brrlog_format_last.level.prefix;
#if !defined(BRRPLATFORMTYPE_WINDOWS)
		if (brrlog_styleon)
			print(&write, dst, "%s", updatefmtstr(prifmt.foreground, prifmt.background, prifmt.style, prifmt.font));
#endif
		print(&write, dst, "%s", prefix);
#if !defined(BRRPLATFORMTYPE_WINDOWS)
		if (brrlog_styleon)
			print(&write, dst, "%s", clrstyle);
#endif
	}
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	if (brrlog_styleon)
		print(&write, dst, "%s", updatefmtstr(foreground, background, style, font));
#endif
	va_start(lptr, format);
	vprint(&write, dst, format, lptr);
	va_end(lptr);
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	if (brrlog_styleon)
		print(&write, dst, "%s", clrstyle);
#endif
	if (brrlog_verbose) {
		print(&write, dst, " : '%s' @ %s:%llu", file, function, line);
	}
	if (print_newline) {
		print(&write, dst, "\n");
	}

	if (logmax) {
		fprintf(dst, "%s", buffer);
	}

	brrlog_format_last.level.priority = priority;
	brrlog_format_last.level.destination = destination;
	brrlog_format_last.level.prefix = prefix;
	brrlog_format_last.foreground = foreground;
	brrlog_format_last.background = background;
	brrlog_format_last.style = style;
	brrlog_format_last.font = font;

	logcount++;
	return write;
}

brrsz BRRCALL
brrlog_digits(LOG_PARAMS, char digit_separator, brrsz separator_spacing, brrb1 is_signed, brru8 number, brru1 base)
{
	brrsz inputlength = 0, outputlength = 0, sepcount = 0;
	char *stringin = NULL, *stringout = NULL;
	brru1 isneg = 0;
	if (!digit_separator) digit_separator = ',';
	if (!separator_spacing) separator_spacing = -1;

	inputlength = brrlib_ndigits(is_signed, number, base);
	if (is_signed && (brrs8)number < 0) {
		inputlength++;
		isneg = 1;
	}
	if (!brrlib_alloc((void **)&stringin, inputlength + 1, true))
		return 0;
	sepcount = (inputlength - isneg - 1)/separator_spacing;
	outputlength = inputlength + sepcount;
	if (!brrlib_alloc((void **)&stringout, outputlength + 1, true)) {
		brrlib_alloc((void **)&stringin, 0, false);
		return 0;
	}
	brrlib_print_base(stringin, inputlength + 1, number, is_signed, base);
	if (isneg) {
		stringout[0] = stringin[0];
	}

	for(brru8 i = 0, c = 0; i < inputlength-isneg; ++i) {
		if (i && (i%separator_spacing)==0) {
			stringout[outputlength-(i+c++)-1]=digit_separator;
		}
		stringout[outputlength-(i+c)-1]=stringin[inputlength-i-1];
	}

	brrlib_alloc((void **)&stringin, 0, 0);
	outputlength = brrlog_text(LOG_ARGS, "%s", stringout);
	brrlib_alloc((void **)&stringout, 0, 0);
	return outputlength;
}

brrsz BRRCALL
brrlog_bits(LOG_PARAMS, brrb1 reverse_bytes, brrb1 reverse_bits, brrsz bits_to_print, const void *const data,
    char bit_separator, brrsz separator_spacing)
{
	const brrby *const bytes = (const brrby *const)data;
	char *text = NULL;
	static const brrsz byte_width = 8;
	brrsz nbytes = 0, nnormalbytes = 0, residue = 0, seps = 0, bits_parsed = 0;
	if (!data || !bits_to_print) {
		return brrlog_text(LOG_ARGS, "");
	}

	if (!bit_separator) bit_separator = ' ';
	if (!separator_spacing) separator_spacing = -1;
	// TODO one day custom byte widths.
	//if (!byte_width) byte_width = -1;
	{
		brrsz sepcount = (bits_to_print - 1) / separator_spacing;
		if (!brrlib_alloc((void **)&text, bits_to_print + sepcount + 1, true))
			return 0;
	}

	nbytes = bits_to_print / byte_width;
	//nnormalbytes = bits_to_print >> 3;
	residue = bits_to_print - (nbytes * byte_width);

#define addbits(tx, by, rvs, res, ab, bp, sep, spc) \
for (brru1 b=0;b<(res);++b,++bp) { \
	brru1 bit = (1<<((rvs)?(res)-b-1:b)); \
	if ((bp)&&((bp)%(spc)==0)) (tx)[bp+ab++]=sep; \
	(tx)[bp+ab] = (by)&bit?'1':'0'; \
}
	if (reverse_bytes) {
		if (residue != 0) {
			brrby partial = bytes[nbytes];
			addbits(text, partial, reverse_bits, residue, seps, bits_parsed, bit_separator, separator_spacing);
		}
		for (brrsz B = 0; B < nbytes; ++B) {
			brrby byte = bytes[nbytes - 1 - B];
			addbits(text, byte, reverse_bits, byte_width, seps, bits_parsed, bit_separator, separator_spacing);
		}
	} else {
		for (brrsz B = 0; B < nbytes; ++B) {
			brrby byte = bytes[B];
			addbits(text, byte, reverse_bits, byte_width, seps, bits_parsed, bit_separator, separator_spacing);
		}
		if (residue != 0) {
			brrby partial = bytes[nbytes];
			addbits(text, partial, reverse_bits, residue, seps, bits_parsed, bit_separator, separator_spacing);
		}
	}
#undef addbits

	{
		brrsz write = brrlog_text(LOG_ARGS, "%s", text);
		brrlib_alloc((void **)&text, 0, false);
		return write;
	}
}
