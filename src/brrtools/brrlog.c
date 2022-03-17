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

#define _brrlog_keep_generators
#include "brrtools/brrlog.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"
#include "brrtools/brrheap.h"
#include "brrtools/brrnum.h"

#if defined(BRRPLATFORMTYPE_Windows)
#define CLEAR_STYLE ""
static char st_fmtstr[] = "";
#else
#define CLEAR_STYLE "\x1b[000;039;049;010m"
static char st_fmtstr[] = "\x1b[000;039;049;010m";
#endif

static FILE *st_lastlocation = NULL;
static brrlog_priority_t st_minpri = brrlog_priority_none  + 1;
static brrlog_priority_t st_maxpri = brrlog_priority_count - 1;
static brru8 st_logcount = 0;
static brrsz st_logmax = 2048;
static char *st_buffer = NULL;

#if !defined(BRRPLATFORMTYPE_Windows)
static int BRRCALL
fgid(brrlog_color_t fg)
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
bgid(brrlog_color_t bg)
{
	return 10 + fgid(bg);
}
static int BRRCALL
stid(brrlog_style_t st)
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
fnid(brrlog_font_t fn)
{
	fn = fn<0?0:fn>=brrlog_font_count?brrlog_font_count-1:fn;
	return fn + 10;
}
#endif
static char *BRRCALL
updatefmtstr(brrlog_color_t fg, brrlog_color_t bg, brrlog_style_t st, brrlog_font_t fn)
{
#if !defined(BRRPLATFORMTYPE_Windows)
	snprintf(st_fmtstr, 19, "\x1b[%03i;%03i;%03i;%03im",
			stid(st),
			fgid(fg),
			bgid(bg),
			fnid(fn));
#endif
	return st_fmtstr;
}

#define _print_prototype brrsz *const wrt, char *const buffer, FILE *const dst, const char *const fmt
static void BRRCALL
vbuffprint(_print_prototype, va_list lptr)
{
	*wrt += vsprintf(buffer + *wrt, fmt, lptr);
}
static void BRRCALL
buffprint(_print_prototype, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	vbuffprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}
static void BRRCALL
vnomaxprint(_print_prototype, va_list lptr)
{
	*wrt += vfprintf(dst, fmt, lptr);
}
static void BRRCALL
nomaxprint(_print_prototype, ...)
{ va_list lptr;
	va_start(lptr, fmt);
	vnomaxprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}
static void BRRCALL
vmaxprint(_print_prototype, va_list lptr)
{
	brrof tmp;
	if (st_logmax < *wrt)
		*wrt = st_logmax;
	tmp = vsnprintf(st_buffer + *wrt, st_logmax - *wrt, fmt, lptr);\
	if (tmp > 0) *wrt += ((brrsz)tmp) < st_logmax ? ((brrsz)tmp) : st_logmax - *wrt - 1;\
}
static void BRRCALL
maxprint(_print_prototype, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	vmaxprint(wrt, buffer, dst, fmt, lptr);
	va_end(lptr);
}

static void BRRCALL
uselast(brrlog_priority_t *p, brrlog_destination_t *d,
    brrlog_color_t *fg, brrlog_color_t *bg, brrlog_style_t *s, brrlog_font_t *fn)
{
	if (*p  <= brrlog_priority_last)    *p  = gbrrlog_type_last.level.priority;
	if (*d  <= brrlog_destination_last) *d  = gbrrlog_type_last.level.destination;
	if (*fg <= brrlog_color_last)       *fg = gbrrlog_type_last.format.foreground;
	if (*bg <= brrlog_color_last)       *bg = gbrrlog_type_last.format.background;
	if (*s  <= brrlog_style_last)       *s  = gbrrlog_type_last.format.style;
	if (*fn <= brrlog_font_last)        *fn = gbrrlog_type_last.format.font;
}
static void BRRCALL
setlast(brrlog_priority_t p, brrlog_destination_t d,
    brrlog_color_t fg, brrlog_color_t bg, brrlog_style_t s, brrlog_font_t fn)
{
	gbrrlog_type_last.level.priority = p;
	gbrrlog_type_last.level.destination = d;
	gbrrlog_type_last.format.foreground = fg;
	gbrrlog_type_last.format.background = bg;
	gbrrlog_type_last.format.style = s;
	gbrrlog_type_last.format.font = fn;
}
static brrlog_format_t BRRCALL
pritofmt(brrlog_priority_t p)
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

#define _ex(_t,_N,_n,_i,_d) #_n,
const char *brrlog_color_names[brrlog_color_count + 1]             = { _brrlog_color_gen(_ex) };
const char *brrlog_font_names[brrlog_font_count + 1]               = { _brrlog_font_gen(_ex) };
const char *brrlog_style_names[brrlog_style_count + 1]             = { _brrlog_style_gen(_ex) };
const char *brrlog_priority_names[brrlog_priority_count + 1]       = { _brrlog_priority_gen(_ex) };
const char *brrlog_destination_names[brrlog_destination_count + 1] = { _brrlog_destination_gen(_ex) };
#undef _ex
#define _ex(_t,_N,_n,_i,_d) #_N,
const char *brrlog_color_debug_names[brrlog_color_count + 1]             = { _brrlog_color_gen(_ex) };
const char *brrlog_font_debug_names[brrlog_font_count + 1]               = { _brrlog_font_gen(_ex) };
const char *brrlog_style_debug_names[brrlog_style_count + 1]             = { _brrlog_style_gen(_ex) };
const char *brrlog_priority_debug_names[brrlog_priority_count + 1]       = { _brrlog_priority_gen(_ex) };
const char *brrlog_destination_debug_names[brrlog_destination_count + 1] = { _brrlog_destination_gen(_ex) };
#undef _ex

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

brrlog_type_t gbrrlog_type_critical = {CRI_LEVEL, CRI_FORMAT};
brrlog_type_t gbrrlog_type_error    = {ERR_LEVEL, ERR_FORMAT};
brrlog_type_t gbrrlog_type_normal   = {NOR_LEVEL, NOR_FORMAT};
brrlog_type_t gbrrlog_type_warning  = {WAR_LEVEL, WAR_FORMAT};
brrlog_type_t gbrrlog_type_debug    = {DEB_LEVEL, DEB_FORMAT};
brrlog_type_t gbrrlog_type_clear    = {CLR_LEVEL, CLR_FORMAT};
brrlog_type_t gbrrlog_type_last     = {LST_LEVEL, LST_FORMAT};

struct gbrrlogctl gbrrlogctl =
{
#if defined(BRRPLATFORMTYPE_Windows)
	.style_enabled = 0,
#else
	.style_enabled = 1,
#endif
	.debug_enabled = 0,
	.flush_enabled = 1,
	.flush_always = 0,
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
		if (brrlib_alloc((void **)&st_buffer, newmax, 1))
			return -1;
		else
			st_logmax = newmax;
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

brrlog_priority_t BRRCALL
brrlog_minpriority(void)
{
	return st_minpri;
}
void BRRCALL
brrlog_setminpriority(brrlog_priority_t newmin)
{
	st_minpri = newmin > brrlog_priority_none ?
		newmin < brrlog_priority_count ?
			newmin > st_maxpri?st_maxpri:newmin :
		brrlog_priority_count-1 :
	brrlog_priority_none+1;
}

brrlog_priority_t BRRCALL
brrlog_maxpriority(void)
{
	return st_maxpri;
}
void BRRCALL
brrlog_setmaxpriority(brrlog_priority_t newmax)
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
brrlog_text(_brrlog_log_params, const char *const format, ...)
{
	uselast(&priority, &destination, &foreground, &background, &style, &font);
	if (!format || priority < st_minpri || priority > st_maxpri) {
		setlast(priority, destination, foreground, background, style, font);
		return 0;
	}
/* If BRRTOOLS_DEBUG was defined, always print debug priority logs */
#if !BRRTOOLS_DEBUG
	if (!gbrrlogctl.debug_enabled && priority == brrlog_priority_debug) {
		setlast(priority, destination, foreground, background, style, font);
		return 0;
	}
#endif

	void (*vprint)(_print_prototype, va_list) = NULL;
	void (*print)(_print_prototype, ...) = NULL;
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

	FILE *dst = NULL;
	/* Defaults to stderr if invalid destination */
	if (destination != brrlog_destination_null)
		dst = destination == brrlog_destination_stdout?stdout:stderr;

	brrlog_format_t prifmt = pritofmt(priority);
	if (gbrrlogctl.flush_enabled) {
		if (dst && st_lastlocation != dst) {
			fflush(st_lastlocation);
			st_lastlocation = dst;
		}
	}

	brrsz write = 0;
	if (gbrrlogctl.prefixes_enabled && print_prefix) {
		if (!prefix)
			prefix = gbrrlog_type_last.level.prefix;
#if !defined(BRRPLATFORMTYPE_Windows)
		if (gbrrlogctl.style_enabled)
			print(&write, buffer, dst, "%s", updatefmtstr(prifmt.foreground, prifmt.background, prifmt.style, prifmt.font));
#endif
		print(&write, buffer, dst, "%s", prefix);
#if !defined(BRRPLATFORMTYPE_Windows)
		if (gbrrlogctl.style_enabled)
			print(&write, buffer, dst, "%s", CLEAR_STYLE);
#endif
	}
#if !defined(BRRPLATFORMTYPE_Windows)
	if (gbrrlogctl.style_enabled)
		print(&write, buffer, dst, "%s", updatefmtstr(foreground, background, style, font));
#endif
	va_list lptr;
	va_start(lptr, format);
	vprint(&write, buffer, dst, format, lptr);
	va_end(lptr);
#if !defined(BRRPLATFORMTYPE_Windows)
	if (gbrrlogctl.style_enabled)
		print(&write, buffer, dst, "%s", CLEAR_STYLE);
#endif
	if (gbrrlogctl.verbose_enabled)
		print(&write, buffer, dst, " : '%s' @ %s:%llu", file, function, line);
	if (gbrrlogctl.newline_enabled && print_newline)
		print(&write, buffer, dst, "\n");

	if (st_logmax && dst && !buffer)
		fprintf(dst, "%s", st_buffer);
	setlast(priority, destination, foreground, background, style, font);
	st_logcount++;
	if (gbrrlogctl.flush_always && dst)
		fflush(dst);
	return write;
}

/* TODO secondary calls to brrlog_text could maybe be sped up if argument
 * validation and actual logging were split into separate static functions.
 * */
brrsz BRRCALL
brrlog_digits(_brrlog_log_params, brru8 number, int base,
    brrbl is_signed, char digit_separator, brrsz separator_spacing)
{
	static const char bases[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"!@#$%^&*()`-=[]\\;',./~_+{}|:\"<>?";
	static const int maxbase = sizeof(bases) - 1;
	char outstr[129] = {0};

	if (!number)
		return brrlog_text(_brrlog_full_args, "0");

	brrbl isneg = brrfalse;
	base = brrnum_uclamp(base, 2, maxbase);
	if (is_signed && (brrs8)number < 0) {
		number = -(brrs8)number;
		isneg = 1;
	}

	int outlen = 0, i = 0;
	imaxdiv_t div = (imaxdiv_t){number, 0};
	do {
		div = imaxdiv(div.quot, base);
		if (separator_spacing && i && i % separator_spacing == 0)
			outstr[outlen++] = digit_separator;
		outstr[outlen++] = bases[div.rem];
		++i;
	} while (div.quot);
	if (isneg)
		outstr[outlen++] = '-';
	brrheap_reverse_bytes(outstr, outlen);
	return brrlog_text(_brrlog_full_args, "%s", outstr);
}

brrsz BRRCALL
brrlog_bits(_brrlog_log_params, const void *const data,
    brrsz bits_to_print, brrbl reverse_bytes, brrbl reverse_bits, char bit_separator,
    brrsz separator_spacing)
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
			    ((brru1 *)data)[div.quot], div.rem,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
		for (brrsz B = 0; B < div.quot; ++B) {
			_brrlog_bytebits(outstr, outlen,
			    ((brru1 *)data)[div.quot - B - 1], 8,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
	} else {
		for (brrsz B = 0; B < div.quot; ++B) {
			_brrlog_bytebits(outstr, outlen,
			    ((brru1 *)data)[B], 8,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
		if (div.rem) {
			_brrlog_bytebits(outstr, outlen,
			    ((brru1 *)data)[div.quot], div.rem,
			    reverse_bits, nbits, bit_separator, separator_spacing, md);
		}
	}

	print = brrlog_text(_brrlog_full_args, "%s", outstr);
	brrlib_alloc((void **)&outstr, 0, 0);
	return print;
}
