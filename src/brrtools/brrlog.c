#include "brrtools/brrlog.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CNF brrlog_global_cfg

#if _brrlog_can_style
static const brrlog_style_t i_clear = {0, ._len=sizeof("\x1b[0;39;49;10m")-1};
static brrlog_style_t i_last_st = {0, ._len=sizeof("\x1b[0;39;49;10m")-1};
static brrlog_dst_t i_last_dst = {.type = -1};
static const brrlog_style_t i_unset = {.st=-1,.fg=-1,.bg=-1,.fn=-1};

static BRR_inline int BRRCALL i_st(brrlog_st_t st)
{
	switch (st) {
		case brrlog_st_normal:       return  0;
		case brrlog_st_bold:         return  1;
		case brrlog_st_dim:          return  2;
		case brrlog_st_italics:      return  3;
		case brrlog_st_under:        return  4;
		case brrlog_st_blink:        return  5;
		case brrlog_st_fastblink:    return  6;
		case brrlog_st_reverse:      return  7;
		case brrlog_st_conceal:      return  8;
		case brrlog_st_strikeout:    return  9;
		case brrlog_st_fraktur:      return 20;
		case brrlog_st_nobold:       return 21;
		case brrlog_st_nobright:     return 22;
		case brrlog_st_noitalics:    return 23;
		case brrlog_st_nounder:      return 24;
		case brrlog_st_noblink:      return 25;
		case brrlog_st_noreverse:    return 27;
		case brrlog_st_reveal:       return 28;
		case brrlog_st_nostrikeout:  return 29;
		case brrlog_st_frame:        return 51;
		case brrlog_st_circle:       return 52;
		case brrlog_st_over:         return 53;
		case brrlog_st_noframe:      return 54;
		case brrlog_st_noover:       return 55;
		case brrlog_st_iunder:       return 60;
		case brrlog_st_idoubleunder: return 61;
		case brrlog_st_iover:        return 62;
		case brrlog_st_idoubleover:  return 63;
		case brrlog_st_istress:      return 64;
		case brrlog_st_ioff:         return 65;
		default: return 0;
	}
}
static BRR_inline int BRRCALL i_fg(brrlog_col_t fg)
{
	switch (fg) {
		case brrlog_col_black:        return 30;
		case brrlog_col_red:          return 31;
		case brrlog_col_green:        return 32;
		case brrlog_col_yellow:       return 33;
		case brrlog_col_blue:         return 34;
		case brrlog_col_magenta:      return 35;
		case brrlog_col_cyan:         return 36;
		case brrlog_col_white:        return 37;
		case brrlog_col_grey:         return 90;
		case brrlog_col_lightred:     return 91;
		case brrlog_col_lightgreen:   return 92;
		case brrlog_col_lightyellow:  return 93;
		case brrlog_col_lightblue:    return 94;
		case brrlog_col_lightmagenta: return 95;
		case brrlog_col_lightcyan:    return 96;
		case brrlog_col_lightwhite:   return 97;
		default: return 39;
	}
}
#define i_bg(_bg_) (10 + i_fg(_bg_))
static BRR_inline int BRRCALL i_fn(brrlog_fn_t fn)
{
	switch (fn) {
		case 1: return 11;
		case 2: return 12;
		case 3: return 13;
		case 4: return 14;
		case 5: return 15;
		case 6: return 16;
		case 7: return 17;
		case 8: return 18;
		case 9: return 19;
		default: return 10;
	}
}

static BRR_inline brrlog_st_t BRRCALL i_stchr(char st)
{
	switch (st) {
		case '-': return brrlog_st_last;
		case '0': return brrlog_st_normal;
		case 'b': return brrlog_st_bold;
		case 'd': return brrlog_st_dim;
		case 'i': return brrlog_st_italics;
		case 'u': return brrlog_st_under;
		case 'k': return brrlog_st_blink;
		case 't': return brrlog_st_fastblink;
		case 'r': return brrlog_st_reverse;
		case 'c': return brrlog_st_conceal;
		case 's': return brrlog_st_strikeout;
		case 'f': return brrlog_st_fraktur;
		case 'B': return brrlog_st_nobold;
		case 'G': return brrlog_st_nobright;
		case 'I': return brrlog_st_noitalics;
		case 'U': return brrlog_st_nounder;
		case 'K': return brrlog_st_noblink;
		case 'R': return brrlog_st_noreverse;
		case 'v': return brrlog_st_reveal;
		case 'S': return brrlog_st_nostrikeout;
		case 'm': return brrlog_st_frame;
		case 'l': return brrlog_st_circle;
		case 'o': return brrlog_st_over;
		case 'M': return brrlog_st_noframe;
		case 'O': return brrlog_st_noover;
		case 'V': return brrlog_st_iunder;
		case 'W': return brrlog_st_idoubleunder;
		case 'X': return brrlog_st_iover;
		case 'Y': return brrlog_st_idoubleover;
		case 'Z': return brrlog_st_istress;
		case 'z': return brrlog_st_ioff;
		default: return -1;
	}
}
static BRR_inline brrlog_col_t BRRCALL i_colchr(char col)
{
	switch (col) {
		case '-': return brrlog_col_last;
		case '0': return brrlog_col_normal;
		case 'k': return brrlog_col_black;
		case 'r': return brrlog_col_red;
		case 'g': return brrlog_col_green;
		case 'y': return brrlog_col_yellow;
		case 'b': return brrlog_col_blue;
		case 'm': return brrlog_col_magenta;
		case 'c': return brrlog_col_cyan;
		case 'w': return brrlog_col_white;
		case 'K': return brrlog_col_grey;
		case 'R': return brrlog_col_lightred;
		case 'G': return brrlog_col_lightgreen;
		case 'Y': return brrlog_col_lightyellow;
		case 'B': return brrlog_col_lightblue;
		case 'M': return brrlog_col_lightmagenta;
		case 'C': return brrlog_col_lightcyan;
		case 'W': return brrlog_col_lightwhite;
		default: return -1;
	}
}
static BRR_inline brrlog_fn_t BRRCALL i_fnchr(char fn)
{
	switch (fn) {
		case '-': return brrlog_fn_last;
		case '0': return brrlog_fn_normal;
		case '1': return brrlog_fn_1;
		case '2': return brrlog_fn_2;
		case '3': return brrlog_fn_3;
		case '4': return brrlog_fn_4;
		case '5': return brrlog_fn_5;
		case '6': return brrlog_fn_6;
		case '7': return brrlog_fn_7;
		case '8': return brrlog_fn_8;
		case '9': return brrlog_fn_9;
		default: return -1;
	}
}
static BRR_inline brrlog_style_t BRRCALL i_style_flat(brrlog_style_t st)
{
	return (brrlog_style_t){
		.st = i_st(st.st),
		.fg = i_fg(st.fg),
		.bg = i_bg(st.bg),
		.fn = i_fn(st.fn),
	};
}

#define STY_CODE_FMT "\x1b[%u;%u;%u;%um"
#define STY_CODE_LEN (sizeof("\x1b[000;039;049;010m")-1)
#define sty_args(_S_) i_st((_S_).st), i_fg((_S_).fg), i_bg((_S_).bg), i_fn((_S_).fn)
#define sty_exp(_S_) (_S_).st, (_S_).fg, (_S_).bg, (_S_).fn
#endif
#define pfx_args(_p_) "%.*s", (int)(_p_)._pfxl, (_p_).pfx

static BRR_inline int i_dst_vld(brrlog_dst_t d)
{
	return d.type >= _brrlog_dst_min && d.type <= _brrlog_dst_max && d._loc;
}

brrlog_style_t BRRCALL
brrlog_style_or(brrlog_style_t a, brrlog_style_t b)
{
	return (brrlog_style_t){
#if _brrlog_can_style
		.st = a.st < 0 ? b.st : a.st,
		.fg = a.fg < 0 ? b.fg : a.fg,
		.bg = a.bg < 0 ? b.bg : a.bg,
		.fn = a.fn < 0 ? b.fn : a.fn,
#else
		0
#endif
	};
}

typedef struct i_stylable
{
	const char *pfx;
	brrsz pfxl;
	const char *cnt;
	brrsz cntl;
} i_stylable_t;

/* Returns 0 when there are no stylables */
static inline int BRRCALL i_next_stylable(const char *message, brrsz msg_len, brrsz *ofs, i_stylable_t *sty)
{
	msg_len -= *ofs;
	if (!CNF._sty_open || !CNF._sty_len) {
		return 0;
	} else if (msg_len <= 2 * CNF._sty_len + 1 /* +1 for prefix sep, always ':' */) {
		*sty = (i_stylable_t){0};
		*ofs += msg_len;
		return 0;
	}
	const char *const start = message + *ofs;
	const brrsz str_max = msg_len + 1 - CNF._sty_len;

	/* (! prefix : content !) */
	for (brrsz i = 0, escaped = 0; i < str_max; ++i) {
		if (escaped) {
			escaped = 0;
		} else if (start[i] == '\\') {
			escaped = 1;
		} else if (!memcmp(start + i, CNF._sty_open, CNF._sty_len)) {
			brrsz cnt = i + CNF._sty_len;
			brrsz pfx = BRRSZ_MAX;
			for (;cnt < str_max && memcmp(start + cnt, CNF._sty_close, CNF._sty_len); ++cnt) {
				if (start[cnt] == ':' && pfx == BRRSZ_MAX)
					pfx = cnt;
			}
			if (cnt == str_max      /* No closing token, false alarm */
				|| pfx == BRRSZ_MAX /* No pfx separator, false alarm */
			) {
				continue;
			}

			/* Found a complete stylable section, with no guarantee that the prefix has any length */
			*sty = (i_stylable_t){
				.pfx = start + i + CNF._sty_len,
				.pfxl = pfx - i - CNF._sty_len,
				.cnt = start + pfx + 1,
				.cntl = cnt - pfx - 1,
			};
			/* Sets the current offset after the closing token */
			*ofs += cnt + CNF._sty_len;
			return 1;
		}
	}
	*ofs += str_max + 1;
	return 0;
}

#if _brrlog_can_style
#define PFXMODE_ST 1
#define PFXMODE_FG 2
#define PFXMODE_BG 3
#define PFXMODE_FN 4
static BRR_inline int BRRCALL i_pfxmode(char pfxchar)
{
	switch (pfxchar) {
		case 's': return PFXMODE_ST;
		case 'f': return PFXMODE_FG;
		case 'b': return PFXMODE_BG;
		case '_': return PFXMODE_FN;
		default: return 0;
	}
}
#endif
static inline brrsz BRRCALL i_parse_style_prefix(brrlog_style_t *const st, const i_stylable_t *const sty)
{
	if (!sty->pfxl)
		return 0;

	/* A prefix looks like this:
	 *   (!f=*b=*s=*_=*:...!)
	 * f: Foreground color as a single character code
	 * b: Background color as a single character code
	 * s: Style as a single character code
	 * _: Font as a single character code
	 *
	 * Any unrecognized characters between assignments are ignored, thus they may be separated however one
	 * chooses.
	 *
	 * For an assignment of any [fbs_]:
	 * * Only the first assignment in a list is used, the rest are ignored.
	 * * Invalid 1ccs are treated as if the aren't specified at all.
	 *
	 * Valid assignments adhere to:
	 * * If the 1cc is '-', this means take the value of the last used style, or the default style in the case
	 *   of no previous style set.
	 * * Else if the 1cc is '0', then the style for that type is temporarily cleared.
	 * * Else if any of [fbs_] is not present, then that style is unchanged
	 * * Otherwise:
	 *   * 'f' or 'b' may be any of:
	 *       'k' for black
	 *       'r' for red
	 *       'g' for green
	 *       'y' for yellow
	 *       'b' for blue
	 *       'm' for magenta
	 *       'c' for cyan
	 *       'w' for white
	 *       or the capital of any of the above for the 'light' variant
	 *
	 *   * 's' may be any of:
	 *       'b' for bold
	 *       'd' for dim
	 *       'i' for italic
	 *       'u' for underline
	 *       'k' for blinking
	 *       't' for fast blinking
	 *       'r' for reverse colors
	 *       'c' for concealed
	 *       's' for strikeout
	 *       'f' for fraktur
	 *       'B' to disable bold
	 *       'G' to disable bright/bold
	 *       'I' to disable italic
	 *       'U' to disable underline
	 *       'K' to disable blinking
	 *       'R' to disable reverse
	 *       'v' for reveal
	 *       'S' to disable strikeout
	 *       'm' for frame
	 *       'l' for circle
	 *       'o' for overline
	 *       'M' to disable frame
	 *       'O' to disable overline
	 *       'V' for ideogram underline
	 *       'W' for ideogram double-underline
	 *       'X' for ideogram overline
	 *       'Y' for ideogram double-overline
	 *       'Z' for ideogram stress
	 *       'z' for ideogram off
	 *
	 *   * '_' may be one of [0-9] for alternate font 0-9, respectively.
	 * */

	const char *S = sty->pfx;
	const char *E = sty->pfx + sty->pfxl;
	while(isspace(*S))S++;if(S==E)return 0;
	while(isspace(*(E-1)))E--;if(E==S)return 0;

	/* woo! */
	#if _brrlog_can_style
	if (st) {
		brrlog_style_t s = i_unset;
		for (int assigning = 0, mode = 0; S!=E; ++S) {
			char c = *S;
			if (isspace(c)) {
				continue;
			} else if (mode) {
				if (c == '=' || isspace(c))
					continue;
				switch (mode) {
					case PFXMODE_ST: if (s.st == -1) s.st = i_stchr(c); break;
					case PFXMODE_FG: if (s.fg == -1) s.fg = i_colchr(c); break;
					case PFXMODE_BG: if (s.bg == -1) s.bg = i_colchr(c); break;
					case PFXMODE_FN: if (s.fn == -1) s.fn = i_fnchr(c); break;
				}
				mode = 0;
			} else {
				mode = i_pfxmode(c);
			}
		}

		if (s.st == -2) s.st = i_last_st.st;
		if (s.fg == -2) s.fg = i_last_st.fg;
		if (s.bg == -2) s.bg = i_last_st.bg;
		if (s.fn == -2) s.fn = i_last_st.fn;
		s = brrlog_style_or(s, i_clear);
		s._len = snprintf(NULL, 0, STY_CODE_FMT, sty_args(s));
		*st = s;
	}
	#endif
	return 1;
}

static BRR_inline brrsz BRRCALL i_pri_index(int label)
{
	for (brrsz i = 0; i < CNF._npri; ++i) if (CNF._lbl[i]==label) return i;
	return CNF._npri;
}
static BRR_inline brrlog_priority_t *BRRCALL i_pri(int label)
{
	brrsz i = i_pri_index(label);
	if (i != CNF._npri)
		return &CNF._pri[i];
	return NULL;
}

static brrsz BRRCALL i_styled_len(const char *const msg, brrsz msgl, brrlog_style_t surround)
{
	i_stylable_t sty = {0};
	brrsz ofs = 0, sty_tkns = 0, sty_bytes = 0;
	while (i_next_stylable(msg, msgl, &ofs, &sty)) {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, &sty)) {
			// Valid prefix
			sty_tkns += sty.pfxl + 1 + 2 * CNF._sty_len;
#if _brrlog_can_style
			if (sty.cntl)
				sty_bytes += st._len + surround._len;
			i_last_st = st;
#endif
		} // else invalid prefix, won't be styled
	}
#if _brrlog_can_style
	i_last_st = i_clear;
#endif
	return ofs - sty_tkns + sty_bytes;
}
static brrsz BRRCALL i_styled(const char *const msg, brrsz msgl, char *const dst, brrlog_style_t surround)
{
	i_stylable_t sty = {0};
	brrsz iofs = 0;

	if (!i_next_stylable(msg, msgl, &iofs, &sty)) {
		/* No styles */
		memcpy(dst, msg, msgl);
		return msgl;
	}
	brrsz oofs = 0, oldofs = 0;
#if _brrlog_can_style
	brrlog_style_t surround_flat = i_style_flat(surround);
#endif
	do {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, &sty)) {
			brrsz O = iofs - sty.pfxl - sty.cntl - 2 * CNF._sty_len - 1;
			brrsz L = O - oldofs;
			memcpy(dst + oofs, msg + oldofs, L);
			oofs += L;
#if _brrlog_can_style
			oofs += snprintf(dst + oofs, st._len, STY_CODE_FMT, sty_args(st));
#endif
			memcpy(dst + oofs, sty.cnt, sty.cntl);
			oofs += sty.cntl;
#if _brrlog_can_style
			oofs += snprintf(dst + oofs, surround._len, STY_CODE_FMT, sty_exp(surround_flat));
			i_last_st = st;
#endif
		} else {
			memcpy(dst + oofs, msg + oldofs, iofs - oldofs);
			oofs += iofs - oldofs;
		}
		oldofs = iofs;
	} while (i_next_stylable(msg, msgl, &iofs, &sty));

	memcpy(dst + oofs, msg + oldofs, iofs - oldofs);
	oofs += iofs - oldofs;
	return oofs;
}

/* Initialize 'pri' by parsing the style of 'pfx' and "canonicalizing" the string, flattening it to be
 * static so that it doesn't also need to be styled when logging every message */
int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_dst_t dst, brrlog_style_t style)
{
	if (!pri) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return 1;
	}
	brrlog_priority_t p = {.style = style, .dst = dst};
	if (!i_dst_vld(p.dst))
		p.dst = (brrlog_dst_t){._loc = stderr, .type = brrlog_dst_stream};

	if (!pfx) {
		*pri = p;
		return 0;
	}

	brrsz ml = brrstringr_length(pfx, CNF.max_prefix);
	if (ml) {
#if _brrlog_can_style
		p.style._len = snprintf(NULL, 0, STY_CODE_FMT, sty_args(p.style));
#endif
		brrsz sl = i_styled_len(pfx, ml, p.style);
		if (!(p.pfx = calloc(1 + sl, 1))) {
			brrapi_sete(BRRAPI_E_ARGERR);
			return 1;
		}
		p._pfxl = sl;
#if _brrlog_can_style
		if (sl != ml)
			i_styled(pfx, sl, (char*)p.pfx, p.style);
		else
#endif
			memcpy((char*)p.pfx, pfx, ml);
	}
	*pri = p;
	return 0;
}

void BRRCALL
brrlog_priority_free(brrlog_priority_t *const pri)
{
	if (!pri)
		return;
	if (pri->pfx)
		free((char*)pri->pfx);
}

const brrlog_cfg_t brrlog_default_cfg =
{
	.max_nest = 32,
	.min_label = -10,
	.max_label = 10,
	.max_priorities = 20,

	.style_enabled = 1,
	.prefixes_enabled = 1,
	.newlines_enabled = 1,
	.verbose = 0,
	.debug = 0,
	.flush_enabled = 1,
	.flush_always = 0,

	.max_log = 2048,
	.max_prefix = 32,
};

brrlog_cfg_t brrlog_global_cfg = {0};

int BRRCALL
brrlog_init(brrlog_cfg_t cfg, const char *const style_open, const char *const style_close)
{
	if (CNF._init)
		return 0;

	brrlog_cfg_t c = {
		.max_nest = cfg.max_nest,
		.min_label = cfg.min_label,
		.max_label = cfg.max_label,
		.max_priorities = cfg.max_priorities,

		.settings = cfg.settings,

		.max_log = cfg.max_log,
		.max_prefix = cfg.max_prefix,
	};

	c._sty_len = brrstringr_length(style_open, BRRLOG_MAX_STYLE_TOKEN);
	if (!c._sty_len || !style_open || !style_close || c._sty_len != brrstringr_length(style_close, BRRLOG_MAX_STYLE_TOKEN)) {
		c.style_enabled = 0;
	} else {
		if (!(c._sty_open = calloc(2 * (1 + c._sty_len), 1))) {
			brrapi_sete(BRRAPI_E_MEMERR);
			return 1;
		}
		c._sty_close = c._sty_open + c._sty_len + 1;
		memcpy((char*)c._sty_open, style_open, c._sty_len);
		memcpy((char*)c._sty_close, style_close, c._sty_len);
	}

	{ brrlog_priority_t p = {0};
		brrlog_cfg_t t = CNF;
		CNF = c;
		if (brrlog_priority_init(&p, cfg.def_pri.pfx, cfg.def_pri.dst, cfg.def_pri.style)) {
			if (c._sty_open)
				free((char*)c._sty_open);
			CNF = t;
			return 1;
		}
		CNF = t;
		c.def_pri = p;
	}

	if (c.max_log) {
		if (!(c._log_buf = calloc(1 + c.max_log, 1))) {
			if (c._sty_open)
				free((char*)c._sty_open);
			brrlog_priority_free(&c.def_pri);
			brrapi_sete(BRRAPI_E_MEMERR);
			return 1;
		}
		c._buf_alloc = c.max_log;
	}
	c._init = 1;
	CNF = c;
	return 0;
}
void BRRCALL
brrlog_deinit(void)
{
	if (CNF._sty_open)
		free((char*)CNF._sty_open);
	if (CNF._pri) {
		for (brrsz i = 0; i < CNF._npri; ++i)
			brrlog_priority_free(&CNF._pri[i]);
		free(CNF._lbl);
	}
	if (CNF._log_buf)
		free(CNF._log_buf);
	memset(&CNF, 0, sizeof(CNF));
}

brrsz BRRCALL
brrlog_priority_index(int label)
{
	return i_pri_index(label);
}
int BRRCALL
brrlog_priority_mod(int label, brrlog_priority_t new)
{
	brrsz i = i_pri_index(label);
	if (i == CNF.max_priorities) {
		brrapi_sete(BRRAPI_E_ARRFULL);
		return 1;
	}

	brrlog_priority_t p = {0};
	if (brrlog_priority_init(&p, new.pfx, new.dst, new.style))
		return 1;

	if (i == CNF._npri) {
		/* Malloc instead of realloc, because I don't want to change CNF until both this and _pri are reallocated */
		brru1 *l = realloc(CNF._lbl, (sizeof(*CNF._lbl) + sizeof(*CNF._pri)) * (CNF._npri + 1));
		if (!l) {
			brrlog_priority_free(&p);
			brrapi_sete(BRRAPI_E_MEMERR);
			return 1;
		}
		CNF._lbl = (int*)l;
		CNF._pri = (brrlog_priority_t*)(l + sizeof(*CNF._lbl) * (CNF._npri + 1));
		CNF._npri++;
	}

	brrlog_priority_free(&CNF._pri[i]);
	CNF._lbl[i] = label;
	CNF._pri[i] = p;
	return 0;
}
int BRRCALL
brrlog_priority_delete(int label)
{
	brrsz i = i_pri_index(label);
	if (i == CNF._npri) {
		return 0;
	} else if (CNF._npri == 1) {
		/* Removing the last priority, just free the data */
		free(CNF._lbl);
		CNF._lbl = NULL;
		CNF._pri = NULL;
		CNF._npri = 0;
		return 0;
	}

	/*
	 * Because '_lbl' and '_pri' overlap, to remove a priority is to split the memory region into at most 3
	 * parts:
	 *   012  3            456012  3            456
	 *   LLL [L to remove] LLLPPP [P to remove] PPP
	 *   (each L/P represents a single label/priority)
	 *
	 * The goal is to move the middle 'LLLPPP' part to the end of the 'LLL' part, then the 'PPP' part to the
	 * end of that, and finally reallocate
	 *
	 * OR: One could instead allocate a new buffer of the correct size first and copy only the L/Ps to be
	 * preserved, so that in case of allocation failure the original data remains unaffected.
	 *
	 * I use the second option here; it may be slower due to manual reallocation and data copying, but I
	 * don't think it matters that much.
	 *
	 * In the case where 'i' is the last priority, only the third part disappears and doesn't need to be
	 * copied, as that's the data in '_pri' that needs to be removed anyway.
	 * It's a similar situation when 'i' is the first priority, except it's the first part that disappears.
	 * */

	const brrsz new_lbl = sizeof(*CNF._lbl) * (CNF._npri - 1);
	const brrsz new_pri = sizeof(*CNF._pri) * (CNF._npri - 1);
	brru1 *new = malloc(new_lbl + new_pri); /* only point of error */
	if (!new) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return 1;
	}
	brrlog_priority_free(&CNF._pri[i]); /* could be returned to the user instead of freed */

	/* First part */
	const brrsz first_len = sizeof(*CNF._lbl) * i;
	if (first_len) {
		memcpy(new, CNF._lbl, first_len);
	}
	/* Mid part */
	const brrsz mid_len = sizeof(*CNF._lbl) * (CNF._npri - 1 - i) + sizeof(*CNF._pri) * i;
	const brrsz mid_ofs = first_len + sizeof(*CNF._lbl); /* skip the label to remove */
	memcpy(new + first_len, (brru1*)CNF._lbl + mid_ofs, mid_len);
	/* Last part */
	brrsz last_len = sizeof(*CNF._pri) * (CNF._npri - 1 - i);
	if (last_len) {
		brrsz last_ofs = mid_ofs + mid_len + sizeof(*CNF._pri); /* skip the pri to remove */
		memcpy(new + first_len + mid_len, (brru1*)CNF._pri + last_ofs, last_len);
	}

	free(CNF._lbl);
	CNF._npri--;
	CNF._lbl = (void*)new;
	CNF._pri = (void*)(new + new_lbl);

	return 0;
}

/* I'm ugly and I'm mostly proud */

#define pri_unbounded(_pr_, _w_, _d_, ...) do {\
	int i = _pr_((_d_) + (_w_), __VA_ARGS__);\
	if (i < 0) return BRRSZ_MAX;\
	_w_ += i;\
} while (0)
#define xpri_nocheck(_pr_, _w_, _d_, _fmt_, _fmto_, _fmtl_, _args_) do {\
	char *_f = (char*)(_fmt_) + (_fmto_);\
	brrsz _l = (_fmtl_);\
	char c = _f[_l];\
	_f[_l] = 0;\
	int i = v##_pr_((_d_) + (_w_), _f, _args_);\
	_f[_l] = c;\
	if (i < 0) return BRRSZ_MAX;\
	_w_ += i;\
} while (0)
#define pri_bounded(_pr_, _w_, _d_, _M_, ...) do {\
	int i = _pr_((_d_) + (_w_), (_M_) - (_w_) + 1, __VA_ARGS__);\
	if (i < 0) return BRRSZ_MAX;\
	_w_ += i;\
	if (_w_ >= _M_) return _w_;\
} while (0)
#define xpri_check(_pr_, _w_, _d_, _M_, _fmt_, _fmto_, _fmtl_, _args_) do {\
	/* printf's only a section of the 'fmt' by setting the last byte to 0 before printf, then resetting it after */\
	brrsz _l = (_fmtl_);\
	char *_f = (char*)(_fmt_) + (_fmto_);\
	char c = _f[_l];\
	_f[_l] = 0;\
	int i = v##_pr_((_d_) + (_w_), (_M_) - (_w_) + 1, _f, _args_);\
	_f[_l] = c;\
	if (i < 0) return BRRSZ_MAX;\
	_w_ += i;\
	if (_w_ >= _M_) return _w_;\
} while(0)
static BRR_inline brrsz i_log_no_max_stream(brrlog_parms_t P, brrlog_priority_t p, const char *const fmt, brrsz fmtl, FILE *f, va_list args)
{
	brrsz w = 0;

	if (CNF.prefixes_enabled && P.print_prefix)
		pri_unbounded(fprintf, w, f, pfx_args(p));
	if (CNF.verbose && P.verbose) {
		/* TODO print line, func, file in some way */
	}

	brrsz ofs = 0, oldofs = 0;
	i_stylable_t sty;
	brrlog_style_t st;
	if (!i_next_stylable(fmt, fmtl, &ofs, &sty)) {
		/* Nothing to style, just do a normal printf */
		xpri_nocheck(fprintf, w, f, fmt, 0, fmtl, args);
		return w;
	} else {
		do {
			if (!i_parse_style_prefix(&st, &sty)) {
				xpri_nocheck(fprintf, w, f, fmt, oldofs, ofs - oldofs, args);
			} else {
				xpri_nocheck(fprintf, w, f, fmt, oldofs, ofs - 2 * CNF._sty_len - sty.pfxl - 1 - sty.cntl - oldofs, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_unbounded(fprintf, w, f, STY_CODE_FMT, sty_args(st));
				#endif
				xpri_nocheck(fprintf, w, f, sty.cnt, 0, sty.cntl, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_unbounded(fprintf, w, f, STY_CODE_FMT, sty_args(p.style));
				#endif
				i_last_st = st;
			}
			oldofs = ofs;
		} while (i_next_stylable(fmt, fmtl, &ofs, &sty));
		xpri_nocheck(fprintf, w, f, fmt, oldofs, ofs - oldofs, args);
	}

	if (CNF.newlines_enabled && P.print_newline) {
		int i = fprintf(f, "\n");
		if (i < 0)
			return BRRSZ_MAX;
		w++;
	}
	return w;
}
static BRR_inline brrsz i_log_no_max_buffer(brrlog_parms_t P, brrlog_priority_t p, const char *const fmt, brrsz fmtl, char *b, va_list args)
{
	brrsz w = 0;

	if (CNF.prefixes_enabled && P.print_prefix)
		pri_unbounded(sprintf, w, b, pfx_args(p));
	if (CNF.verbose && P.verbose) {
		/* TODO print line, func, file in some way */
	}

	brrsz ofs = 0, oldofs = 0;
	i_stylable_t sty;
	brrlog_style_t st;
	if (!i_next_stylable(fmt, fmtl, &ofs, &sty)) {
		/* Nothing to style, just do a normal printf */
		xpri_nocheck(sprintf, w, b, fmt, 0, fmtl, args);
		return w;
	} else {
		do {
			if (!i_parse_style_prefix(&st, &sty)) {
				xpri_nocheck(sprintf, w, b, fmt, oldofs, ofs - oldofs, args);
			} else {
				xpri_nocheck(sprintf, w, b, fmt, oldofs, ofs - 2 * CNF._sty_len - sty.pfxl - 1 - sty.cntl - oldofs, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_unbounded(sprintf, w, b, STY_CODE_FMT, sty_args(st));
				#endif
				xpri_nocheck(sprintf, w, b, sty.cnt, 0, sty.cntl, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_unbounded(sprintf, w, b, STY_CODE_FMT, sty_args(p.style));
				#endif
				i_last_st = st;
			}
			oldofs = ofs;
		} while (i_next_stylable(fmt, fmtl, &ofs, &sty));
		xpri_nocheck(sprintf, w, b, fmt, oldofs, ofs - oldofs, args);
	}

	if (CNF.newlines_enabled && P.print_newline) {
		b[w++] = '\n';
	}
	return w;
}
static BRR_inline brrsz i_log_max(brrlog_parms_t P, brrlog_priority_t p, const char *const fmt, brrsz fmtl, char *dst, va_list args)
{
	brrsz w = 0;

	if (CNF.prefixes_enabled && P.print_prefix)
		pri_bounded(snprintf, w, dst, CNF.max_log, pfx_args(p));
	if (CNF.verbose && P.verbose) {
		/* TODO print line, func, file in some way */
	}

	brrsz ofs = 0, oldofs = 0;
	i_stylable_t sty;
	brrlog_style_t st;
	if (!i_next_stylable(fmt, fmtl, &ofs, &sty)) {
		/* Nothing to style, just do a normal printf */
		xpri_check(snprintf, w, dst, CNF.max_log, fmt, 0, fmtl, args);
	} else {
		do {
			if (!i_parse_style_prefix(&st, &sty)) {
				xpri_check(snprintf, w, dst, CNF.max_log, fmt, oldofs, ofs - oldofs, args);
			} else {
				xpri_check(snprintf, w, dst, CNF.max_log, fmt, oldofs, ofs - 2 * CNF._sty_len - sty.pfxl - 1 - sty.cntl - oldofs, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_bounded(snprintf, w, dst, CNF.max_log, STY_CODE_FMT, sty_args(st));
				#endif
				xpri_check(snprintf, w, dst, CNF.max_log, sty.cnt, 0, sty.cntl, args);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					pri_bounded(snprintf, w, dst, CNF.max_log, STY_CODE_FMT, sty_args(p.style));
				#endif
				i_last_st = st;
			}
			oldofs = ofs;
		} while (i_next_stylable(fmt, fmtl, &ofs, &sty));
		xpri_check(snprintf, w, dst, CNF.max_log, fmt, oldofs, ofs - oldofs, args);
	}

	if (CNF.newlines_enabled && P.print_newline) {
		if (w + 1 <= CNF.max_log)
			dst[w++] = '\n';
	}
	dst[w] = 0;
	return w;
}

brrsz BRRCALL
brrlogv(brrlog_parms_t P, const char *const fmt, va_list args)
{
	const brrsz fmtl = strlen(fmt);
	if (!fmt) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return BRRSZ_MAX;
	} else if (!fmtl || ((P.label < CNF.min_label || P.label > CNF.max_label) && !CNF.debug)) {
		return 0;
	}
	/* I hate this I hate this I hate this I hate this it's slow and could be abused
	 * But I need it if I want to use any kind of compile-time format string that will end up in the ro
	 * section, because of the way I implemented subsectioning said format string
	 * The most performant way to fix this that I can think of would require me to do either:
	 *   A: Implement my own printf (please God no)
	 *   B: Style the format string before passing it to *printf, which would require me to allocate
	 *   appropriate space too, for the styled format.
	 * The benefit of the latter is that it would greatly simplify this entire module, but it still fails to
	 * avoid malloc, so my problem remains just as unsolved.
	 * How can I tell *printf to only read n bytes of the format string?
	 * TODO: help me. HELP. ME.*/
	char *FMT = calloc(fmtl+1, 1);
	if (!FMT) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return BRRSZ_MAX;
	}
	memcpy(FMT, fmt, fmtl);

	brrlog_priority_t pri;
	{ brrsz i = i_pri_index(P.label);
		if (i == CNF._npri || P.label < CNF.min_label || P.label > CNF.max_label) {
			pri = CNF.def_pri;
		} else {
			pri = CNF._pri[i];
		}
	}

	brrsz w = 0;
	brrsz ofs = 0, oldofs = 0;
	i_stylable_t sty;
	brrlog_style_t st;
	if (pri.dst.type == brrlog_dst_stream) {
		FILE *f = pri.dst._loc;

		/* Flush the toilet please */
		if (i_last_dst.type == brrlog_dst_stream &&
		    (CNF.flush_always ||
		        (CNF.flush_enabled && i_last_dst._loc && i_last_dst._loc != f)))
			fflush(i_last_dst._loc);

		if (!CNF.max_log) {
			w = i_log_no_max_stream(P, pri, FMT, fmtl, f, args);
		} else {
			w = i_log_max(P, pri, FMT, fmtl, CNF._log_buf, args);
			if (w != BRRSZ_MAX) {
				int i = fprintf(f, "%s", CNF._log_buf);
				if (i < 0)
					w = BRRSZ_MAX;
				else
					w = i;
			}
		}
	} else {
		char *b = pri.dst._loc;
		if (!CNF.max_log) {
			w = i_log_no_max_buffer(P, pri, FMT, fmtl, b, args);
		} else {
			w = i_log_max(P, pri, FMT, fmtl, b, args);
		}
	}
	free(FMT);

	if (w != BRRSZ_MAX) { /* BRRSZ_MAX to indicate error */
		i_last_dst = pri.dst;
	} else {
		brrapi_sete(BRRAPI_E_PRINTERR);
		return BRRSZ_MAX;
	}
	return w;
}
brrsz BRRCALL
brrlog(brrlog_parms_t parms, const char *const fmt, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	brrsz mlen = brrlogv(parms, fmt, lptr);
	va_end(lptr);
	return mlen;
}
