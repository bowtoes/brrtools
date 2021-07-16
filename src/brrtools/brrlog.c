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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "brrtools/brrlog.h"

#include "brrtools/brrplatform.h"
#include "brrtools/brrtil.h"
#include "brrtools/brrlib.h"
#include "brrtools/brrmem.h"

#if defined(BRRPLATFORMTYPE_WINDOWS)
static const char *const st_clrstyle = "";
static char st_fmtstr[] = "";
#else
static const char *const st_clrstyle = "\x1b[000;039;049;010m";
static char st_fmtstr[] = "\x1b[000;039;049;010m";
#endif

static FILE *st_lastlocation = NULL;
static brrlog_priorityT st_minpri = brrlog_priority_none+1;
static brrlog_priorityT st_maxpri = brrlog_priority_count-1;
static brru8 st_logcount = 0;
static brrsz st_logmax = 2048;
static char *st_buffer = NULL;

#if !defined(BRRPLATFORMTYPE_WINDOWS)
static int BRRCALL
fgid(brrlog_colorT fg)
{
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
static int BRRCALL
bgid(brrlog_colorT bg)
{
	return 10 + fgid(bg);
}
static int BRRCALL
stid(brrlog_styleT st)
{
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
static int BRRCALL
fnid(brrlog_fontT fn)
{
	fn = fn<0?0:fn>=brrlog_font_count?brrlog_font_count-1:fn;
	return fn + 10;
}
#endif
static char *BRRCALL
updatefmtstr(brrlog_colorT fg, brrlog_colorT bg, brrlog_styleT st, brrlog_fontT fn)
{
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	snprintf(st_fmtstr, 19, "\x1b[%03i;%03i;%03i;%03im",
			stid(st),
			fgid(fg),
			bgid(bg),
			fnid(fn));
#endif
	return st_fmtstr;
}

#define PRINT_PROTO brrsz *const wrt, char *const buffer, FILE *const dst, const char *const fmt
static void BRRCALL
vbuffprint(PRINT_PROTO, va_list lptr)
{
	*wrt += vsprintf(buffer + *wrt, fmt, lptr);
}
static void BRRCALL
buffprint(PRINT_PROTO, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	vbuffprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}
static void BRRCALL
vnomaxprint(PRINT_PROTO, va_list lptr)
{
	*wrt += vfprintf(dst, fmt, lptr);
}
static void BRRCALL
nomaxprint(PRINT_PROTO, ...)
{ va_list lptr;
	va_start(lptr, fmt);
	vnomaxprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}
static void BRRCALL
vmaxprint(PRINT_PROTO, va_list lptr)
{
	brrof tmp;
	if (st_logmax < *wrt)
		*wrt = st_logmax;
	tmp = vsnprintf(st_buffer + *wrt, st_logmax - *wrt, fmt, lptr);\
	if (tmp > 0) *wrt += ((brrsz)tmp) < st_logmax ? ((brrsz)tmp) : st_logmax - *wrt - 1;\
}
static void BRRCALL
maxprint(PRINT_PROTO, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	vmaxprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}

static void BRRCALL
uselast(brrlog_priorityT *p, brrlog_destinationT *d,
    brrlog_colorT *fg, brrlog_colorT *bg, brrlog_styleT *s, brrlog_fontT *fn)
{
	if (*p  <= brrlog_priority_last)    *p  = gbrrlog_type_last.level.priority;
	if (*d  <= brrlog_destination_last) *d  = gbrrlog_type_last.level.destination;
	if (*fg <= brrlog_color_last)       *fg = gbrrlog_type_last.format.foreground;
	if (*bg <= brrlog_color_last)       *bg = gbrrlog_type_last.format.background;
	if (*s  <= brrlog_style_last)       *s  = gbrrlog_type_last.format.style;
	if (*fn <= brrlog_font_last)        *fn = gbrrlog_type_last.format.font;
}
static void BRRCALL
setlast(brrlog_priorityT p, brrlog_destinationT d,
    brrlog_colorT fg, brrlog_colorT bg, brrlog_styleT s, brrlog_fontT fn)
{
	gbrrlog_type_last.level.priority = p;
	gbrrlog_type_last.level.destination = d;
	gbrrlog_type_last.format.foreground = fg;
	gbrrlog_type_last.format.background = bg;
	gbrrlog_type_last.format.style = s;
	gbrrlog_type_last.format.font = fn;
}
static brrlog_formatT BRRCALL
pritofmt(brrlog_priorityT p)
{
	if (p == brrlog_priority_critical)
		return gbrrlog_type_critical.format;
	else if (p == brrlog_priority_error)
		return gbrrlog_type_error.format;
	else if (p == brrlog_priority_normal)
		return gbrrlog_type_normal.format;
	else if (p == brrlog_priority_warning)
		return gbrrlog_type_warning.format;
	else if (p == brrlog_priority_debug)
		return gbrrlog_type_debug.format;
	else
		return gbrrlog_type_clear.format;
}

#define DUMCLAMP(_x, _min, _max) ((_x)<(_min)?(_x):(_x)>(_max)?(_max):(_x))
#define LIST_CLAMP(_x, _s) _s[DUMCLAMP(_x, brrlog_##_x##_last, brrlog_##_x##_count) + 1]
const char *BRRCALL
brrlog_color_str(brrlog_colorT color)
{
#define ENUM_COLORS(_TOKEN) \
	_TOKEN(normal) \
	_TOKEN(black)     _TOKEN(red)          _TOKEN(green)      _TOKEN(yellow) \
	_TOKEN(blue)      _TOKEN(magenta)      _TOKEN(cyan)       _TOKEN(white) \
	_TOKEN(darkgrey)  _TOKEN(lightred)     _TOKEN(lightgreen) _TOKEN(lightyellow) \
	_TOKEN(lightblue) _TOKEN(lightmagenta) _TOKEN(lightcyan)  _TOKEN(lightwhite)
#define TOKEN(x) #x,
	static const char *const color_strs[brrlog_color_count + 2] = {
		"last",
		ENUM_COLORS(TOKEN)
		"invalid",
	};
#undef TOKEN
	return LIST_CLAMP(color, color_strs);
}
const char *BRRCALL
brrlog_font_str(brrlog_fontT font)
{
#define ENUM_FONTS(_TOKEN) \
	_TOKEN(normal) \
	_TOKEN(1) _TOKEN(2) _TOKEN(3) _TOKEN(4) \
	_TOKEN(5) _TOKEN(6) _TOKEN(7) _TOKEN(8) \
	_TOKEN(9)
#define TOKEN(x) #x,
	static const char *const font_strs[brrlog_font_count + 2] = {
		"last",
		ENUM_FONTS(TOKEN)
		"invalid",
	};
#undef TOKEN
	return LIST_CLAMP(font, font_strs);
}
const char *BRRCALL
brrlog_style_str(brrlog_styleT style)
{
#define ENUM_STYLES(_TOKEN) \
	_TOKEN(normal) \
	_TOKEN(bold)         _TOKEN(dim)         _TOKEN(italics)     _TOKEN(under) \
	_TOKEN(blink)        _TOKEN(fastblink)   _TOKEN(reverse)     _TOKEN(conceal) \
	_TOKEN(strikeout)    _TOKEN(fraktur)     _TOKEN(nobold)      _TOKEN(nobright) \
	_TOKEN(noitalics)    _TOKEN(nounder)     _TOKEN(noblink)     _TOKEN(noreverse) \
	_TOKEN(reveal)       _TOKEN(nostrikeout) _TOKEN(frame)       _TOKEN(circle) \
	_TOKEN(over)         _TOKEN(noframe)     _TOKEN(noover)      _TOKEN(iunder) \
	_TOKEN(idoubleunder) _TOKEN(iover)       _TOKEN(idoubleover) _TOKEN(istress) \
	_TOKEN(ioff)
#define TOKEN(x) #x,
	static const char *const style_strs[brrlog_style_count + 2] = {
		"last",
		ENUM_STYLES(TOKEN)
		"invalid",
	};
#undef TOKEN
	return LIST_CLAMP(style, style_strs);
}
const char *BRRCALL
brrlog_priority_str(brrlog_priorityT priority)
{
#define ENUM_LEVELS(_TOKEN) \
	_TOKEN(none) \
	_TOKEN(critical) \
	_TOKEN(error) \
	_TOKEN(normal) \
	_TOKEN(warning) \
	_TOKEN(debug)
#define TOKEN(x) #x,
	static const char *const priority_strs[brrlog_priority_count + 2] = {
		"last",
		ENUM_LEVELS(TOKEN)
		"invalid",
	};
#undef TOKEN
	return LIST_CLAMP(priority, priority_strs);
}
const char *BRRCALL
brrlog_priority_dbgstr(brrlog_priorityT priority)
{
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
	return LIST_CLAMP(priority, priority_dbgstrs);
}

#define CRI_LEVEL {brrlog_priority_critical, brrlog_destination_stderr, "CRITICAL: "}
#define ERR_LEVEL {brrlog_priority_error,    brrlog_destination_stderr, "Error: "   }
#define NOR_LEVEL {brrlog_priority_normal,   brrlog_destination_stdout, ""          }
#define WAR_LEVEL {brrlog_priority_warning,  brrlog_destination_stderr, "Warning: " }
#define DEB_LEVEL {brrlog_priority_debug,    brrlog_destination_stderr, "DEBUG: "   }
#define CLR_LEVEL {brrlog_priority_normal,   brrlog_destination_stdout, ""          }
#define LST_LEVEL {brrlog_priority_normal,   brrlog_destination_stdout, ""          }

#define CRI_FORMAT {brrlog_color_red,    brrlog_color_normal, brrlog_style_reverse, brrlog_font_normal}
#define ERR_FORMAT {brrlog_color_red,    brrlog_color_normal, brrlog_style_bold,    brrlog_font_normal}
#define NOR_FORMAT {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}
#define WAR_FORMAT {brrlog_color_yellow, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}
#define DEB_FORMAT {brrlog_color_yellow, brrlog_color_normal, brrlog_style_reverse, brrlog_font_normal}
#define CLR_FORMAT {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}
#define LST_FORMAT {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}

brrlog_typeT gbrrlog_type_critical = {CRI_LEVEL, CRI_FORMAT};
brrlog_typeT gbrrlog_type_error    = {ERR_LEVEL, ERR_FORMAT};
brrlog_typeT gbrrlog_type_normal   = {NOR_LEVEL, NOR_FORMAT};
brrlog_typeT gbrrlog_type_warning  = {WAR_LEVEL, WAR_FORMAT};
brrlog_typeT gbrrlog_type_debug    = {DEB_LEVEL, DEB_FORMAT};
brrlog_typeT gbrrlog_type_clear    = {CLR_LEVEL, CLR_FORMAT};
brrlog_typeT gbrrlog_type_last     = {LST_LEVEL, LST_FORMAT};

struct gbrrlogctl gbrrlogctl =
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	.style_enabled = 0,
#else
	.style_enabled = 1,
#endif
	.debug_enabled = 0,
	.flush_enabled = 1,
	.verbose_enabled = 0,
	.prefixes_enabled = 1,
	.newline_enabled = 1,
};

brrsz BRRCALL
brrlog_logmax(void)
{
	return st_logmax;
}
int BRRCALL
brrlog_setlogmax(brrsz newmax)
{
	if (newmax != st_logmax) {
		if (brrlib_alloc((void **)&st_buffer, newmax, 1)) {
			return -1;
		} else {
			st_logmax = newmax;
		}
	}
	return 0;
}
brru8 BRRCALL
brrlog_count(void)
{
	return st_logcount;
}
void BRRCALL
brrlog_resetcount(void)
{
	st_logcount = 0;
}
brrlog_priorityT BRRCALL
brrlog_minpriority(void)
{
	return st_minpri;
}
void BRRCALL
brrlog_setminpriority(brrlog_priorityT newmin)
{
	st_minpri = newmin > brrlog_priority_none ?
		newmin < brrlog_priority_count ?
			newmin > st_maxpri?st_maxpri:newmin :
		brrlog_priority_count-1 :
	brrlog_priority_none+1;
}
brrlog_priorityT BRRCALL
brrlog_maxpriority(void)
{
	return st_maxpri;
}
void BRRCALL
brrlog_setmaxpriority(brrlog_priorityT newmax)
{
	st_maxpri = newmax > brrlog_priority_none ?
		newmax < brrlog_priority_count ?
			newmax < st_minpri?st_minpri:newmax :
		brrlog_priority_count-1 :
	brrlog_priority_none+1;
}

int BRRCALL
brrlog_init(void)
{
	if (!st_buffer && st_logmax)
		return brrlib_alloc((void **)&st_buffer, st_logmax, 1);
	return 0;
}
void BRRCALL
brrlog_deinit(void)
{
	if (st_buffer) {
		free(st_buffer);
		st_buffer = NULL;
	}
}

#define _brrlog_full_args priority, destination, prefix, \
    foreground, background, style, font, buffer, print_prefix, print_newline, \
	file, function, line

brrsz BRRCALL
brrlog_text(_brrlog_log_params,
    const char *const format, ...)
{
	va_list lptr;
	brrsz write = 0;
	FILE *dst = NULL;
	brrlog_formatT prifmt;
	void (*vprint)(PRINT_PROTO, va_list lptr) = NULL;
	void (*print)(PRINT_PROTO, ...) = NULL;
	uselast(&priority, &destination, &foreground, &background, &style, &font);
	if (!format || priority < st_minpri || priority > st_maxpri)
		return 0;
#ifndef BRRTOOLS_DEBUG
	if (!gbrrlogctl.debug_enabled && priority == brrlog_priority_debug)
		return 0;
#endif
	if (destination != brrlog_destination_null)
		dst = destination == brrlog_destination_stderr?stderr:stdout;
	if (buffer) {
		vprint = vbuffprint;
		print = buffprint;
	} else if (st_logmax) {
		vprint = vmaxprint;
		print = maxprint;
	} else {
		vprint = vnomaxprint;
		print = nomaxprint;
	}
	prifmt = pritofmt(priority);
	if (gbrrlogctl.flush_enabled) {
		if (dst && st_lastlocation != dst) {
			fflush(st_lastlocation);
			st_lastlocation = dst;
		}
	}

	if (gbrrlogctl.prefixes_enabled && print_prefix) {
		if (!prefix)
			prefix = gbrrlog_type_last.level.prefix;
#if !defined(BRRPLATFORMTYPE_WINDOWS)
		if (gbrrlogctl.style_enabled)
			print(&write, buffer, dst, "%s", updatefmtstr(prifmt.foreground, prifmt.background, prifmt.style, prifmt.font));
#endif
		print(&write, buffer, dst, "%s", prefix);
#if !defined(BRRPLATFORMTYPE_WINDOWS)
		if (gbrrlogctl.style_enabled)
			print(&write, buffer, dst, "%s", st_clrstyle);
#endif
	}
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	if (gbrrlogctl.style_enabled)
		print(&write, buffer, dst, "%s", updatefmtstr(foreground, background, style, font));
#endif
	va_start(lptr, format);
	vprint(&write, buffer, dst, format, lptr);
	va_end(lptr);
#if !defined(BRRPLATFORMTYPE_WINDOWS)
	if (gbrrlogctl.style_enabled)
		print(&write, buffer, dst, "%s", st_clrstyle);
#endif
	if (gbrrlogctl.verbose_enabled)
		print(&write, buffer, dst, " : '%s' @ %s:%llu", file, function, line);
	if (gbrrlogctl.newline_enabled && print_newline)
		print(&write, buffer, dst, "\n");

	if (st_logmax && dst && !buffer)
		fprintf(dst, "%s", st_buffer);
	setlast(priority, destination, foreground, background, style, font);
	st_logcount++;
	return write;
}

/* TODO secondary calls to 'text' could be sped up if argument validation and
 * actual logging were split into separate static functions. */
brrsz BRRCALL
brrlog_digits(_brrlog_log_params,
    brru8 number, int base, int is_signed, char digit_separator, brrsz separator_spacing)
{
	static const char bases[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"!@#$%^&*()`-=[]\\;',./~_+{}|:\"<>?";
	static const int maxbase = sizeof(bases) - 1;
	char outstr[129] = {0};
	int outlen = 0, i = 0, isneg = 0;
	imaxdiv_t div = {0, 0};

	if (!number)
		return brrlog_text(_brrlog_full_args, "0");

	base = BRRTIL_CLAMP(base, 2, maxbase);
	if (is_signed && (brrs8)number < 0) {
		number = -(brrs8)number;
		isneg = 1;
	}
	div = (imaxdiv_t){number, 0};
	do {
		div = imaxdiv(div.quot, base);
		if (separator_spacing && i && i % separator_spacing == 0)
			outstr[outlen++] = digit_separator;
		outstr[outlen++] = bases[div.rem];
		++i;
	} while (div.quot);
	if (isneg)
		outstr[outlen++] = '-';
	brrmem_static_reverse(outstr, outlen);
	return brrlog_text(_brrlog_full_args, "%s", outstr);
}

brrsz BRRCALL
brrlog_bits(_brrlog_log_params,
    const void *const data, brrsz bits_to_print,
    int reverse_bytes, int reverse_bits, char bit_separator, brrsz separator_spacing)
{
	static const brrsz byte_width = 8;
	char *outstr = NULL;
	brrsz outlen = 0, nbits = 0, print = 0, md = 0;
	imaxdiv_t div;
	if (!data)
		return 0;
	else if (!bits_to_print)
		return brrlog_text(_brrlog_full_args, "");

	outlen = bits_to_print;
	if (separator_spacing) {
		md = bits_to_print % separator_spacing;
		outlen += (bits_to_print - 1) / separator_spacing;
	}
	if (brrlib_alloc((void **)&outstr, outlen + 1, 1))
		return 0;
	div = imaxdiv(bits_to_print, byte_width);
	outlen = 0;

#define _brrlog_bytebits(_out_, _outlen_, _byte_, _bytelen_, _rbits_, _nbits_, _sep_, _spc_, _mod_) do { \
	for (brrsz _b_=0;_b_<(_bytelen_);++_b_,++_nbits_) { \
		if ((_spc_) && _nbits_ && _nbits_ % (_spc_) == _mod_) \
			(_out_)[_outlen_++] = (_sep_); \
		(_out_)[_outlen_++] = '0' + (((_byte_) >> ((_rbits_)?_b_:((_bytelen_)-_b_-1))) & 1); \
	} \
} while (0)
	if (reverse_bytes) {
		/* If there is no remainder then accessing [div.quot] is invalid */
		if (div.rem) {
			_brrlog_bytebits(outstr, outlen,
			    ((brrby *)data)[div.quot], div.rem,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
		for (brrsz B = 0; B < div.quot; ++B) {
			_brrlog_bytebits(outstr, outlen,
			    ((brrby *)data)[div.quot - B - 1], 8,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
	} else {
		for (brrsz B = 0; B < div.quot; ++B) {
			_brrlog_bytebits(outstr, outlen,
			    ((brrby *)data)[B], 8,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
		if (div.rem) {
			_brrlog_bytebits(outstr, outlen,
			    ((brrby *)data)[div.quot], div.rem,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
	}

	print = brrlog_text(_brrlog_full_args, "%s", outstr);
	brrlib_alloc((void **)&outstr, 0, 0);
	return print;
}
