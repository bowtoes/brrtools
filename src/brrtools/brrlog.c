/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#define _brrlog_keep_generators
#include "brrtools/brrlog.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"
#include "brrtools/brrdata.h"
#include "brrtools/brrnum.h"

#if !defined(BRRPLATFORMTYPE_Windows)
#define CLEAR_STYLE "\x1b[000;039;049;010m"
static char s_format_string[] = "\x1b[000;039;049;010m";
#endif

static FILE *s_last_loc = NULL;
static brrlog_priority_t s_min_prt = brrlog_priority_none  + 1;
static brrlog_priority_t s_max_prt = brrlog_priority_count - 1;
static brru8 s_log_count = 0;
static brrsz s_max_log = 2048;
static char *s_log_buffer = NULL;

#if !defined(BRRPLATFORMTYPE_Windows)
static inline int BRRCALL
i_fgid(brrlog_color_t fg)
{
	switch (fg) {
		case brrlog_color_black:        return 30;
		case brrlog_color_red:          return 31;
		case brrlog_color_green:        return 32;
		case brrlog_color_yellow:       return 33;
		case brrlog_color_blue:         return 34;
		case brrlog_color_magenta:      return 35;
		case brrlog_color_cyan:         return 36;
		case brrlog_color_white:        return 37;
		case brrlog_color_darkgrey:     return 90;
		case brrlog_color_lightred:     return 91;
		case brrlog_color_lightgreen:   return 92;
		case brrlog_color_lightyellow:  return 93;
		case brrlog_color_lightblue:    return 94;
		case brrlog_color_lightmagenta: return 95;
		case brrlog_color_lightcyan:    return 96;
		case brrlog_color_lightwhite:   return 97;
		default: return 39;
	}
}
static inline int BRRCALL
i_bgid(brrlog_color_t bg)
{
	return 10 + i_fgid(bg);
}
static inline int BRRCALL
stid(brrlog_style_t st)
{
	switch (st) {
		case brrlog_style_normal:       return  0;
		case brrlog_style_bold:         return  1;
		case brrlog_style_dim:          return  2;
		case brrlog_style_italics:      return  3;
		case brrlog_style_under:        return  4;
		case brrlog_style_blink:        return  5;
		case brrlog_style_fastblink:    return  6;
		case brrlog_style_reverse:      return  7;
		case brrlog_style_conceal:      return  8;
		case brrlog_style_strikeout:    return  9;
		case brrlog_style_fraktur:      return 20;
		case brrlog_style_nobold:       return 21;
		case brrlog_style_nobright:     return 22;
		case brrlog_style_noitalics:    return 23;
		case brrlog_style_nounder:      return 24;
		case brrlog_style_noblink:      return 25;
		case brrlog_style_noreverse:    return 27;
		case brrlog_style_reveal:       return 28;
		case brrlog_style_nostrikeout:  return 29;
		case brrlog_style_frame:        return 51;
		case brrlog_style_circle:       return 52;
		case brrlog_style_over:         return 53;
		case brrlog_style_noframe:      return 54;
		case brrlog_style_noover:       return 55;
		case brrlog_style_iunder:       return 60;
		case brrlog_style_idoubleunder: return 61;
		case brrlog_style_iover:        return 62;
		case brrlog_style_idoubleover:  return 63;
		case brrlog_style_istress:      return 64;
		case brrlog_style_ioff:         return 65;
		default: return 0;
	}
}
static inline int BRRCALL
i_fnid(brrlog_font_t fn)
{
	fn = fn<0?0:fn>=brrlog_font_count?brrlog_font_count-1:fn;
	return fn + 10;
}
static inline char *BRRCALL
i_update_format(brrlog_color_t fg, brrlog_color_t bg, brrlog_style_t st, brrlog_font_t fn)
{
	snprintf(s_format_string, 19, "\x1b[%03i;%03i;%03i;%03im", stid(st), i_fgid(fg), i_bgid(bg), i_fnid(fn));
	return s_format_string;
}
#endif

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

_gbrrlog_types_t gbrrlog_type = {
	.last     = {{brrlog_priority_normal,   brrlog_destination_stdout, ""          }, {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}},
	.clear    = {{brrlog_priority_normal,   brrlog_destination_stdout, ""          }, {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}},
	.critical = {{brrlog_priority_critical, brrlog_destination_stderr, "CRITICAL: "}, {brrlog_color_red,    brrlog_color_normal, brrlog_style_reverse, brrlog_font_normal}},
	.error    = {{brrlog_priority_error,    brrlog_destination_stderr, "Error: "   }, {brrlog_color_red,    brrlog_color_normal, brrlog_style_bold,    brrlog_font_normal}},
	.normal   = {{brrlog_priority_normal,   brrlog_destination_stdout, ""          }, {brrlog_color_normal, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}},
	.warning  = {{brrlog_priority_warning,  brrlog_destination_stderr, "Warning: " }, {brrlog_color_yellow, brrlog_color_normal, brrlog_style_normal,  brrlog_font_normal}},
	.debug    = {{brrlog_priority_debug,    brrlog_destination_stderr, "DEBUG: "   }, {brrlog_color_yellow, brrlog_color_normal, brrlog_style_reverse, brrlog_font_normal}},
};

struct gbrrlogctl gbrrlogctl =
{
#if defined(BRRPLATFORMTYPE_Windows)
	.style_disabled = 1,
#else
	.style_disabled = 0,
#endif
	.debug_enabled = 0,
	.flush_enabled = 1,
	.flush_always = 0,
	.verbose_enabled = 0,
	.prefixes_enabled = 1,
	.newline_enabled = 1,
};

brrsz BRRCALL
brrlog_max_log(void)
{
	return s_max_log;
}
int BRRCALL
brrlog_set_max_log(brrsz newmax)
{
	if (newmax != s_max_log) {
		if (brrlib_alloc((void **)&s_log_buffer, newmax, 1))
			return -1;
		else
			s_max_log = newmax;
	}
	return 0;
}

brru8 BRRCALL
brrlog_count(void)
{
	return s_log_count;
}
void BRRCALL
brrlog_reset_count(void)
{
	s_log_count = 0;
}

brrlog_priority_t BRRCALL
brrlog_min_priority(void)
{
	return s_min_prt;
}
void BRRCALL
brrlog_set_min_priority(brrlog_priority_t new_min)
{
#define _min_set(_nm_) s_min_prt = (_nm_) < s_max_prt?(_nm_):s_max_prt
	if (new_min < brrlog_priority_none + 1)
		_min_set(brrlog_priority_none + 1);
	else if (new_min > brrlog_priority_count - 1)
		_min_set(brrlog_priority_count - 1);
	else
		_min_set(new_min);
#undef _min_set
}

brrlog_priority_t BRRCALL
brrlog_max_priority(void)
{
	return s_max_prt;
}
void BRRCALL
brrlog_set_max_priority(brrlog_priority_t new_max)
{
#define _max_set(_nm_) s_max_prt = (_nm_) > s_min_prt?(_nm_):s_min_prt
	if (new_max < brrlog_priority_none + 1)
		_max_set(brrlog_priority_none + 1);
	else if (new_max > brrlog_priority_count - 1)
		_max_set(brrlog_priority_count - 1);
	else
		_max_set(new_max);
#undef _max_set
}

int BRRCALL
brrlog_init(void)
{
	if (!s_log_buffer && s_max_log)
		return brrlib_alloc((void **)&s_log_buffer, s_max_log, 1);
	return 0;
}
void BRRCALL
brrlog_deinit(void)
{
	if (s_log_buffer) {
		free(s_log_buffer);
		s_log_buffer = NULL;
	}
}

#define s_print_prototype brrsz *const written, char *const buffer, FILE *const destination, const char *const format
typedef void (*s_vprint_t)(s_print_prototype, va_list);
typedef void (*s_print_t)(s_print_prototype, ...);
static inline void BRRCALL
i_vbuff_print(s_print_prototype, va_list lptr)
{
	*written += vsprintf(buffer + *written, format, lptr);
}
static inline void BRRCALL
i_buff_print(s_print_prototype, ...)
{
	va_list lptr;
	va_start(lptr, format);
	i_vbuff_print(written, buffer, destination, format, lptr);
	va_end(lptr);
}

static inline void BRRCALL
i_vno_max_print(s_print_prototype, va_list lptr)
{
	*written += vfprintf(destination, format, lptr);
}
static inline void BRRCALL
i_no_max_print(s_print_prototype, ...)
{
	va_list lptr;
	va_start(lptr, format);
	i_vno_max_print(written, buffer, destination, format, lptr);
	va_end(lptr);
}

static inline void BRRCALL
i_vmax_print(s_print_prototype, va_list lptr)
{
	if (*written > s_max_log)
		*written = s_max_log;
	brrof tmp = vsnprintf(s_log_buffer + *written, s_max_log - *written, format, lptr);
	if (tmp > 0) {
		*written += ((brrsz)tmp) < s_max_log ? ((brrsz)tmp) : s_max_log - *written - 1;
	}
}
static inline void BRRCALL
i_max_print(s_print_prototype, ...)
{
	va_list lptr;
	va_start(lptr, format);
	i_vmax_print(written, buffer, destination, format, lptr);
	va_end(lptr);
}


#define _use_last(_from_, _var_, _src_)\
do {\
	if (_var_ <= brrlog_##_src_##_last) {\
		_var_ = gbrrlog_type.last._from_._var_;\
	}\
} while (0)
#define _set_last(_priority_, _destination_, _foreground_, _background_, _style_, _font_)\
do {\
	gbrrlog_type.last = (brrlog_type_t){\
		.level={.priority=(_priority_), .destination=(_destination_)},\
		.format={.foreground=(_foreground_), .background=(_background_), .style=(_style_), .font=(_font_)}\
	};\
} while (0)

#define _brrlog_full_args priority, destination, prefix, \
    foreground, background, style, font, buffer, print_prefix, print_newline, \
	file, function, line
brrsz BRRCALL
brrlog_text(_brrlog_log_params, const char *const format, ...)
{
	_use_last(level,  priority, priority);
	_use_last(level,  destination, destination);
	_use_last(format, foreground, color);
	_use_last(format, background, color);
	_use_last(format, style, style);
	_use_last(format, font, font);
	if (!format || priority < s_min_prt || priority > s_max_prt) {
		_set_last(priority, destination, foreground, background, style, font);
		return 0;
	}

#if defined(BRR_debug)
	/* Always print DEBUG priority logs when compiled in debug mode */
	if (!gbrrlogctl.debug_enabled && priority == brrlog_priority_debug) {
		_set_last(priority, destination, foreground, background, style, font);
		return 0;
	}
#endif

	s_vprint_t vprint = NULL;
	s_print_t print = NULL;
	if (buffer) {
		vprint = i_vbuff_print;
		print = i_buff_print;
	} else if (s_max_log) {
		vprint = i_vmax_print;
		print = i_max_print;
	} else {
		vprint = i_vno_max_print;
		print = i_no_max_print;
	}

	FILE *dst = NULL;
	/* Defaults to stderr if invalid destination */
	if (destination != brrlog_destination_null)
		dst = destination == brrlog_destination_stdout?stdout:stderr;

	if (gbrrlogctl.flush_enabled) {
		if (dst && s_last_loc != dst) {
			fflush(s_last_loc);
			s_last_loc = dst;
		}
	}

	brrsz write = 0;
	if (gbrrlogctl.prefixes_enabled && print_prefix) {
		if (!prefix)
			prefix = gbrrlog_type.last.level.prefix;

#if !defined(BRRPLATFORMTYPE_Windows)
		brrlog_format_t prifmt = gbrrlog_type.clear.format;
		if (priority >= brrlog_priority_last && priority <= brrlog_priority_debug)
			prifmt = gbrrlog_type.types[priority + 1].format;
		if (!gbrrlogctl.style_disabled)
			print(&write, buffer, dst, "%s", i_update_format(prifmt.foreground, prifmt.background, prifmt.style, prifmt.font));
#endif

		print(&write, buffer, dst, "%s", prefix);

#if !defined(BRRPLATFORMTYPE_Windows)
		if (!gbrrlogctl.style_disabled)
			print(&write, buffer, dst, "%s", CLEAR_STYLE);
#endif

	}

#if !defined(BRRPLATFORMTYPE_Windows)
	if (!gbrrlogctl.style_disabled)
		print(&write, buffer, dst, "%s", i_update_format(foreground, background, style, font));
#endif

	va_list lptr;
	va_start(lptr, format);
	vprint(&write, buffer, dst, format, lptr);
	va_end(lptr);

#if !defined(BRRPLATFORMTYPE_Windows)
	if (!gbrrlogctl.style_disabled)
		print(&write, buffer, dst, "%s", CLEAR_STYLE);
#endif

	if (gbrrlogctl.verbose_enabled)
		print(&write, buffer, dst, " : '%s' @ %s:%llu", file, function, line);
	if (gbrrlogctl.newline_enabled && print_newline)
		print(&write, buffer, dst, "\n");

	if (s_max_log && dst && !buffer)
		fprintf(dst, "%s", s_log_buffer);
	_set_last(priority, destination, foreground, background, style, font);
	s_log_count++;
	if (gbrrlogctl.flush_always && dst)
		fflush(dst);
	return write;
}

/* TODO secondary calls to brrlog_text could maybe be sped up if argument
 * validation and actual logging were split into separate static functions.
 * */
brrsz BRRCALL
brrlog_digits(_brrlog_log_params, brru8 number, int base, brrbl is_signed, char digit_separator, brrsz separator_spacing)
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
	brrdata_reverse_bytes(outstr, outlen);
	return brrlog_text(_brrlog_full_args, "%s", outstr);
}

brrsz BRRCALL
brrlog_bits(_brrlog_log_params, const void *const data, brrsz bits_to_print, brrbl reverse_bytes, brrbl reverse_bits, char bit_separator, brrsz separator_spacing)
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
