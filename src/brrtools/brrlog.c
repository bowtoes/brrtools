#include "brrtools/brrlog.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CNF brrlog_global_cfg

#if _brrlog_can_style
static const brrlog_style_t i_clear = {0, ._len=sizeof("\x1b[0;39;49;10m")-1};
#else
static const brrlog_style_t i_clear = {0};
#endif
static const brrlog_style_t i_unset = {.st=-1,.fg=-1,.bg=-1,.fn=-1};
#if _brrlog_can_style
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
		case brrlog_col_darkgrey:     return 90;
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

#define STY_CODE_FMT "\x1b[%u;%u;%u;%um"
#define STY_CODE_LEN (sizeof("\x1b[000;039;049;010m")-1)

#define sty_len(_S_) (snprintf(NULL, 0, STY_CODE_FMT, i_st((_S_).st), i_fg((_S_).fg), i_bg((_S_).bg), i_fn((_S_).fn)))
#define sty_pri(_m_, _S_) (snprintf((_m_), STY_CODE_LEN + 1, STY_CODE_FMT, i_st((_S_).st), i_fg((_S_).fg), i_bg((_S_).bg), i_fn((_S_).fn)))
#else
#define i_st(...) 0
#define i_fg(...) 0
#define i_bg(_bg_) 0
#define i_fn(_fn_) 0

#define i_stchr(...) 0
#define i_colchr(...) 0
#define i_fnchr(...) 0
#define i_pfxmode(...) 0

#define STY_CODE_FMT ""
#define STY_CODE_LEN 0
#define sty_len(...) 0
#define sty_pri(...) 0
#endif

brrlog_style_t BRRCALL
brrlog_style_or(brrlog_style_t a, brrlog_style_t b)
{
	return (brrlog_style_t){
		.st = a.st < 0 ? b.st : a.st,
		.fg = a.fg < 0 ? b.fg : a.fg,
		.bg = a.bg < 0 ? b.bg : a.bg,
		.fn = a.fn < 0 ? b.fn : a.fn,
	};
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
	.default_priority = {
		.pfx = "[INV]: ",
		.style = {0},
	},
};

brrlog_cfg_t brrlog_global_cfg = {0};

typedef struct i_stylable
{
	const char *pfx;
	brrsz pfxl;
	const char *cnt;
	brrsz cntl;
} i_stylable_t;

/* Returns 0 when there are no stylables */
static inline int BRRCALL
i_next_stylable(const char *message, brrsz msg_len, brrsz *ofs, i_stylable_t *sty)
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

static inline brrsz BRRCALL
i_parse_style_prefix(brrlog_style_t *const st, const i_stylable_t *const sty, brrlog_style_t last)
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
	if (st) {
	#if _brrlog_can_style
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
					case PFXMODE_BG: if (s.bg == -1) s.fg = i_colchr(c); break;
					case PFXMODE_FN: if (s.fn == -1) s.fn = i_fnchr(c); break;
				}
				mode = 0;
			} else {
				mode = i_pfxmode(c);
			}
		}

		s = brrlog_style_or(s, brrlog_style_or(last, CNF.default_priority.style));
		s._len = sty_len(s);
		*st = s;
	#else
		*st = i_clear;
	#endif
	}
	return 1;
}

/* Returns the length of 'msg' with all styling done to it */
static brrsz BRRCALL
i_styled_len(const char *const msg, brrsz msgl, brrlog_style_t surround)
{
	i_stylable_t sty = {0};
	brrsz ofs = 0, sty_tkns = 0, sty_bytes = 0;
	brrlog_style_t last = surround;
	while (i_next_stylable(msg, msgl, &ofs, &sty)) {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, &sty, last)) {
			/* Valid prefix */
			sty_tkns += sty.pfxl + 1 + 2 * CNF._sty_len;
			#if _brrlog_can_style
			if (sty.cntl)
				sty_bytes += st._len + surround._len;
			#endif
			last = st;
		} /* else invalid prefix, won't be styled */
	}
	return ofs - sty_tkns + sty_bytes;
}
static brrsz BRRCALL
i_styled(const char *const msg, brrsz msgl, char *const dst, brrlog_style_t surround)
{
	i_stylable_t sty = {0};
	brrsz iofs = 0;

	if (!i_next_stylable(msg, msgl, &iofs, &sty)) {
		/* No styles */
		memcpy(dst, msg, msgl);
		return msgl;
	}
	brrsz oofs = 0, oldofs = 0;
	brrlog_style_t last = surround;
	do {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, &sty, last)) {
			brrsz O = iofs - sty.pfxl - sty.cntl - 2 * CNF._sty_len - 1;
			brrsz L = O - oldofs;
			memcpy(dst + oofs, msg + oldofs, L);
			oofs += L;

			oofs += sty_pri(dst + oofs, st);

			memcpy(dst + oofs, sty.cnt, sty.cntl);
			oofs += sty.cntl;

			oofs += sty_pri(dst + oofs, surround);
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

static BRR_inline brrsz BRRCALL
i_pri_index(int label)
{
	for (brrsz i = 0; i < CNF._npri; ++i) if (CNF._lbl[i]==label) return i;
	return CNF._npri;
}
static BRR_inline brrlog_priority_t *BRRCALL
i_pri(int label)
{
	brrsz i = i_pri_index(label);
	if (i != CNF._npri)
		return &CNF._pri[i];
	return NULL;
}

/* Initialize 'pri' by parsing the style of 'pfx' and "canonicalizing" the string, flattening it to be
 * static so that it doesn't also need to be styled when logging every message */
int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_style_t style, brrsz max_prefix)
{
	if (!pri) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return 1;
	}
	brrlog_priority_t p = {.style = brrlog_style_or(style, i_clear)};
	if (!pfx) {
		*pri = p;
		return 0;
	}

	brrsz ml = brrstringr_length(pfx, max_prefix);
	if (ml) {
		p.style._len = sty_len(p.style);
		brrsz sl = i_styled_len(pfx, ml, p.style);
		if (!(p.pfx = calloc(1 + sl, 1))) {
			brrapi_sete(BRRAPI_E_ARGERR);
			return 1;
		}
		p._pfxl = sl;
		if (sl == ml) {
			memcpy((char*)p.pfx, pfx, ml);
		} else {
		i_styled(pfx, sl, (char*)p.pfx, p.style);
		}
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

int BRRCALL
brrlog_init(brrlog_cfg_t cfg, const char *const style_open, const char *const style_close)
{
	if (CNF._init)
		return 0;

	brrlog_cfg_t c = {
		.max_nest         = cfg.max_nest,

		.min_label        = cfg.min_label,
		.max_label        = cfg.max_label,

		.max_log          = cfg.max_log,
		.max_prefix       = cfg.max_prefix,
		.max_priorities   = cfg.max_priorities,

		.settings         = cfg.settings,
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
		if (brrlog_priority_init(&p, cfg.default_priority.pfx, brrlog_style_or(cfg.default_priority.style, i_clear), c.max_prefix)) {
			if (c._sty_open)
				free((char*)c._sty_open);
			CNF = t;
			return 1;
		}
		CNF = t;
		c.default_priority = p;
	}

	if (c.max_log) {
		if (!(c._log_buf = calloc(1 + c.max_log, 1))) {
			if (c._sty_open)
				free((char*)c._sty_open);
			brrlog_priority_free(&c.default_priority);
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
int BRRCALL
brrlog_validate(void)
{
	if (CNF.max_log && CNF._buf_alloc != CNF.max_log) {
		char *new = realloc(CNF._log_buf, CNF.max_log + 1);
		if (!new) {
			brrapi_sete(BRRAPI_E_MEMERR);
			return 1;
		}
		CNF._buf_alloc = CNF.max_log;
	} else if (!CNF.max_log && CNF._log_buf) {
		free(CNF._log_buf);
		CNF._log_buf = NULL;
		CNF._buf_alloc = 0;
	}

	if (!CNF._sty_open || !CNF._sty_close) {
		if (CNF._sty_open /* and !CNF._sty_close */) {
			free((char*)CNF._sty_open);
			CNF._sty_len = 0;
		}
		CNF._sty_open = NULL;
		CNF.style_enabled = 0;
	} else {
		brrsz s = brrstringr_length(CNF._sty_open, BRRLOG_MAX_STYLE_TOKEN);
		if (s != brrstringr_length(CNF._sty_close, BRRLOG_MAX_STYLE_TOKEN)) {
			free((char*)CNF._sty_open);
			CNF.style_enabled = 0;
			CNF._sty_open = CNF._sty_close = NULL;
			CNF._sty_len = 0;
		} else if (CNF._sty_len != s) {
			CNF._sty_len = s;
		}
	}

	if (CNF.min_label > CNF.max_label) {
		int t = CNF.max_label;
		CNF.max_label = CNF.min_label;
		CNF.min_label = t;
	}

	return 0;
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
	if (brrlog_priority_init(&p, new.pfx, new.style, CNF.max_prefix))
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

brrsz BRRCALL
brrlogv(brrlog_parms_t p, const char *const fmt, va_list lptr)
{
	const brrsz mlen = strlen(fmt);
	if (!fmt) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return BRRSZ_MAX;
	} else if (!mlen || ((p.label < CNF.min_label || p.label > CNF.max_label) && !CNF.debug)) {
		return 0;
	}

	brrlog_priority_t pri;
	{ brrsz i = i_pri_index(p.label);
		if (i == CNF._npri || p.label < CNF.min_label || p.label > CNF.max_label) {
			pri = CNF.default_priority;
		} else {
			pri = CNF._pri[i];
		}
	}

	brrsz styled_fmt_len = i_styled_len(fmt, mlen, pri.style);
	if (!styled_fmt_len)
		return 0;
	brrsz ostyled = i_styled(fmt, mlen, CNF._log_buf, pri.style);
	if (ostyled != styled_fmt_len) {
		printf("SSSSSSSSSSSSSSSSSSSSSSSSSSS              STYLED != STYLED_LEN: %zu != %zu\n", ostyled, styled_fmt_len);
	}
	fprintf(stderr, "OUTPUT: %*.*s\n", ostyled, ostyled, CNF._log_buf);

	return styled_fmt_len;
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
