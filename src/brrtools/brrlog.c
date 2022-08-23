#define _brrlog_keep_gens
#include "brrtools/brrlog.h"
#undef _brrlog_keep_gens

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CNF brrlog_global_cfg

/* I dislike having to alloc at all, but this is the cleanest solution I've come up with so far */
typedef struct i_buf
{
	brrsz alloc;
	char *d;
	float factor;
	int heur, h;
} i_buf_t;
static BRR_inline void BRRCALL i_buf_init(i_buf_t *const B, float factor, int heuristic)
{
	*B = (i_buf_t){
		.factor = factor,
		.heur = heuristic,
	};
}
static BRR_inline int BRRCALL i_buf_resize(i_buf_t *const B, brrsz sl)
{
	i_buf_t b = *B;
	brrsz ns = b.alloc;
	if (sl > b.alloc) {
		ns = sl * b.factor;
		b.h = b.heur;
	} else if (sl * b.factor < b.alloc) {
		if (b.h) {
			b.h--;
		} else {
			ns = b.alloc / b.factor;
			b.h = b.heur;
		}
	}
	if (ns != b.alloc) {
		if (!(b.d = realloc(b.d, ns + 1)))
			return 1;
		b.d[ns] = 0;
		b.alloc = ns;
	}
	*B = b;
	return 0;
}
static BRR_inline void BRRCALL i_buf_free(i_buf_t *const B)
{
	if (B->d)
		free(B->d);
	memset(B, 0, sizeof(*B));
}

static i_buf_t _F = {
	.factor = 1.5,
	.heur = 10,
};

typedef int (*i_print_t)(void *, const char *, ...);
typedef int (*i_vprint_t)(void *, const char *, va_list);

#if !_brrlog_can_style
#define I_CLEAR {0}
#define I_UNSET {0}
#else
#define I_CLEAR {._s="\x1b[0m", ._l=sizeof("\x1b[0m")-1}
#define _brrlog_max_sts_op(_i_) -1,
#define I_UNSET {.st={_brrlog_max_sts(_brrlog_max_sts_op)}, -1, -1, -1}

static brrlog_style_t i_last_st = I_CLEAR;
static FILE *i_last_stream = NULL;

typedef struct i_sf
{
	const char *s;
	brrsz l;
} i_sf_t;
#define _sf_op(_t_, _n_, _i_, _c_) {.s=_c_";",.l=sizeof(_c_)},
#define _fg_op(_t_, _n_, _i_, _c_, _c2_) _sf_op(_t_, _n_, _i_, _c_)
#define _bg_op(_t_, _n_, _i_, _c_, _c2_) _sf_op(_t_, _n_, _i_, _c2_)
static const i_sf_t i_sts[brrlog_st_count]  = {_brrlog_sts(_sf_op)};
static const i_sf_t i_fgs[brrlog_col_count] = {_brrlog_cols(_fg_op)};
static const i_sf_t i_bgs[brrlog_col_count] = {_brrlog_cols(_bg_op)};
static const i_sf_t i_fns[brrlog_fn_count] = {_brrlog_fns(_sf_op)};
#undef _bg_op
#undef _fg_op
#undef _sf_op

#define i_st(_st_) ((_st_) > 0 && (_st_) < brrlog_st_count ? i_sts[_st_] : i_sts[0])
#define i_fg(_fg_) ((_fg_) > 0 && (_fg_) < brrlog_col_count ? i_fgs[_fg_] : i_fgs[0])
#define i_bg(_bg_) ((_bg_) > 0 && (_bg_) < brrlog_col_count ? i_bgs[_bg_] : i_bgs[0])
#define i_fn(_fn_) ((_fn_) > 0 && (_fn_) < brrlog_fn_count ? i_fns[_fn_] : i_fns[0])
#endif

#if !_brrlog_can_style
#define i_stchr(...) 0
#define i_colchr(...) 0
#define i_fnchr(...) 0
#define i_style_print(...) 0
#else
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
#define i_pr_style(_w_, _dst_, _st_, _type_, _field_) do {\
	if ((_st_)._field_ > -1) {\
		i_sf_t sf = i_##_type_((_st_)._field_);\
		memcpy((_dst_) + (_w_), sf.s, sf.l);\
		(_w_) += sf.l;\
	}\
} while (0)
static BRR_inline brrsz BRRCALL i_style_print(char *const dst, brrlog_style_t st)
{
	brrsz w = 0;
	memcpy(dst + w, ST_PFX, sizeof(ST_PFX) - 1);
	w += sizeof(ST_PFX) - 1;
	for (int i = 0; i < _brrlog_max_st; ++i)
		i_pr_style(w, dst, st, st, st[i]);
	i_pr_style(w, dst, st, fg, fg);
	i_pr_style(w, dst, st, bg, bg);
	i_pr_style(w, dst, st, fn, fn);
	if (w > sizeof(ST_PFX) - 1) /* If actually printed anything, remove the last semicolo */
		w -= 1;
	memcpy(dst + w, ST_SFX, sizeof(ST_SFX) - 1);
	w += sizeof(ST_SFX) - 1;
	dst[w] = 0;
	return w;
}
#endif

static inline brrsz BRRCALL i_parse_style_prefix(brrlog_style_t *const st, const char *const pfx, brrsz pfxl)
{
	if (!pfxl)
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

	const char *S = pfx;
	const char *E = pfx + pfxl;
	while(isspace(*S))S++;if(S==E)return 0;
	while(isspace(*(E-1)))E--;if(E==S)return 0;

	/* woo! */
	#if _brrlog_can_style
	if (st) {
		brrlog_style_t s = I_UNSET;
		int st_idx = 0;

		for (int assigning = 0, mode = 0; S!=E; ++S) {
			char c = *S;
			if (isspace(c)) {
				continue;
			} else if (mode) {
				if (c == '=' || isspace(c))
					continue;
				switch (mode) {
					case 1:
						if (s.st[st_idx] == -1)
							s.st[st_idx] = i_stchr(c);
						st_idx = (st_idx + 1) & (_brrlog_max_st - 1);
						break;
					case 2: if (s.fg == -1) s.fg = i_colchr(c); break;
					case 3: if (s.bg == -1) s.bg = i_colchr(c); break;
					case 4: if (s.fn == -1) s.fn = i_fnchr(c); break;
				}
				mode = 0;
			} else {
				switch (c) {
					case 's':
					case 'S': mode = 1; break;
					case 'f':
					case 'F': mode = 2; break;
					case 'b':
					case 'B': mode = 3; break;
					case '_': mode = 4; break;
					default:  mode = 0; break;
				}
			}
		}

		for (int i = 0; i < _brrlog_max_st; ++i) {
			if (s.st[i] == -2)
				s.st[i] = i_last_st.st[i];
		}
		if (s.fg == -2) s.fg = i_last_st.fg;
		if (s.bg == -2) s.bg = i_last_st.bg;
		if (s.fn == -2) s.fn = i_last_st.fn;
		s._l = i_style_print(s._s, s);
		*st = s;
	}
	#endif
	return 1;
}

brrlog_style_t BRRCALL
brrlog_style_init(brrlog_style_t source)
{
	brrlog_style_t p = I_UNSET;
	for (int i = 0; i < _brrlog_max_st; ++i) {
		int s = 0;
		for (int j = 0; j < i; ++j) {
			if (p.st[j] == source.st[i] || source.st[i] == -1) {
				s = 1;
				break;
			}
		}
		if (!s)
			p.st[i] = source.st[i];
	}
	p._l = i_style_print(p._s, p);
	return p;
}
brrlog_style_t BRRCALL
brrlog_style_or(brrlog_style_t a, brrlog_style_t b)
{
#if !_brrlog_can_style
	return (brrlog_style_t){0};
#else
	brrlog_style_t s = {
		.fg = b.fg == -1 ? a.fg : b.fg,
		.bg = b.bg == -1 ? a.bg : b.bg,
		.fn = b.fn == -1 ? a.fn : b.fn,
	};
	for (int i = 0; i < _brrlog_max_st; ++i)
		s.st[i] = b.st[i] == -1 ? a.st[i] : b.st[i];
	return s;
#endif
}


typedef struct i_stylable
{
	const char *pfx;
	brrsz pfxl;
	const char *cnt;
	brrsz cntl;
} i_stylable_t;
/* Returns 0 when there are no stylables */
static inline int BRRCALL i_next_stylable(const char *const fmt, brrsz fmtl, brrsz *ofs, i_stylable_t *sty)
{
	fmtl -= *ofs;
	if (!CNF._sty_open || !CNF._sty_len) {
		return 0;
	} else if (fmtl <= 2 * CNF._sty_len + 1 /* +1 for prefix sep, always ':' */) {
		*sty = (i_stylable_t){0};
		*ofs += fmtl;
		return 0;
	}
	const char *const start = fmt + *ofs;
	const brrsz str_max = fmtl + 1 - CNF._sty_len;

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
static inline brrsz BRRCALL i_styled_len(const char *const fmt, brrsz fmtl, brrlog_style_t surround)
{
	i_stylable_t sty = {0};
	brrsz ofs = 0, sty_tkns = 0, sty_bytes = 0;
	while (i_next_stylable(fmt, fmtl, &ofs, &sty)) {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, sty.pfx, sty.pfxl)) {
			// Valid prefix
			sty_tkns += sty.pfxl + 1 + 2 * CNF._sty_len;
#if _brrlog_can_style
			if (sty.cntl)
				sty_bytes += st._l + surround._l;
			i_last_st = st;
#endif
		} // else invalid prefix, won't be styled
	}
#if _brrlog_can_style
	i_last_st = (brrlog_style_t)I_CLEAR;
#endif
	return ofs - sty_tkns + sty_bytes;
}

static BRR_inline brrsz BRRCALL i_print(brrsz w, char *restrict const d, const char *restrict const s, brrsz sl)
{
	/*
	if (maxdst) {
		if (*w >= maxdst)
			return 1;
		if (*w + fmtl > maxdst)
			fmtl = maxdst - *w;
	}
	*/
	if (sl)
		memcpy(d + w, s, sl);
	return sl;
}
static inline brrsz BRRCALL i_styled(char *const d, const char *const s, brrsz sl, brrlog_style_t sur)
{
	brrsz fofs = 0, fofsold = 0;
	brrsz w = 0;
	i_stylable_t sty = {0};

	if (!i_next_stylable(s, sl, &fofs, &sty)) {
		w += i_print(w, d, s, sl);
		d[w] = 0;
		return w;
	}
	do {
		brrlog_style_t st;
		if (!i_parse_style_prefix(&st, sty.pfx, sty.pfxl)) {
			w += i_print(w, d, s + fofsold, fofs - fofsold);
		} else {
			brrsz bet_len = fofs - fofsold - CNF._sty_len - sty.pfxl - 1 - sty.cntl - CNF._sty_len;
			w += i_print(w, d, s + fofsold, bet_len);
			#if _brrlog_can_style
			w += i_print(w, d, st._s, st._l);
			#endif
			w += i_print(w, d, sty.cnt, sty.cntl);
			#if _brrlog_can_style
			w += i_print(w, d, sur._s, sur._l);
			#endif
			i_last_st = st;
		}
		fofsold = fofs;
	} while (i_next_stylable(s, sl, &fofs, &sty));
	w += i_print(w, d, s + fofsold, fofs - fofsold);
	d[w] = 0;
	return w;
}

/* Initialize 'pri' by parsing the style of 'pfx' and "canonicalizing" the string, flattening it to be
 * static so that it doesn't also need to be styled when logging every message */
int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_style_t style)
{
	if (!pri) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return 1;
	}

	brrlog_priority_t p = {.style = brrlog_style_init(style)};
	if (!pfx) {
		*pri = p;
		return 0;
	}

	brrsz ml = brrstringr_length(pfx, CNF.max_prefix);
	if (ml) {
		brrsz sl = i_styled_len(pfx, ml, p.style);
		if (!(p.pfx = calloc(1 + sl, 1))) {
			brrapi_sete(BRRAPI_E_ARGERR);
			return 1;
		}
		p._pfxl = sl;
		if (sl != ml) {
			i_styled((char*)p.pfx, pfx, ml, p.style);
		} else {
			memcpy((char*)p.pfx, pfx, ml);
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
	.def_pri = {
		.pfx = "[INV]",
		._pfxl = 5,
		.style = I_CLEAR,
	},

	.max_log = 2048,
	.max_prefix = 32,
};

brrlog_cfg_t brrlog_global_cfg = {0};

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
		if (brrlog_priority_init(&p, cfg.def_pri.pfx, cfg.def_pri.style)) {
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
	i_buf_free(&_F);
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
	if (brrlog_priority_init(&p, new.pfx, new.style))
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
brrlogv(brrlog_out_t dst, brrlog_parms_t parms, const char *const fmt, va_list args)
{
	const brrsz fmtl = strlen(fmt);
	if (!fmt) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return BRRSZ_MAX;
	} else if (!fmtl || (!CNF.debug && (parms.label < CNF.min_label || parms.label > CNF.max_label))) {
		return 0;
	}

	brrlog_priority_t pri;
	{ brrsz i = i_pri_index(parms.label);
		if (i == CNF._npri)
			pri = CNF.def_pri;
		else
			pri = CNF._pri[i];
	}

	if (i_buf_resize(&_F, i_styled_len(fmt, fmtl, pri.style))) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return BRRSZ_MAX;
	}
	i_styled(_F.d, fmt, fmtl, pri.style);

	if (dst.type < _brrlog_out_min || dst.type > _brrlog_out_max || !dst.dst) {
		int t = snprintf(NULL, 0, _F.d, args);
		if (t < 0) {
			brrapi_sete(BRRAPI_E_LIBC);
			return BRRSZ_MAX;
		}
		return t;
	}

	brrsz W = 0;
	if (CNF.max_log) {
		char *L = CNF._log_buf;
		if (CNF.prefixes_enabled && parms.print_prefix) {
			if (pri._pfxl > CNF.max_log)
				pri._pfxl = CNF.max_log;
			memcpy(L + W, pri.pfx, pri._pfxl);
			W += pri._pfxl;
		}
		if (CNF.verbose && parms.verbose) {
			/* TODO */
		}
		{ int w = vsnprintf(L + W, CNF.max_log - W + 1, _F.d, args);
			if (w < 0) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
			W += w;
		}
		if (CNF.newlines_enabled && parms.print_newline) {
			if (W < CNF.max_log) {
				L[W++] = '\n';
			}
		}
		L[W] = 0;

		if (dst.type == brrlog_out_stream) {
			brrsz w = fwrite(L, W, 1, dst.dst);
			if (w < 1) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
		} else {
			memcpy(dst.dst, L, W + 1);
		}
		return W;
	} else if (dst.type == brrlog_out_stream) {
		FILE *F = dst.dst;
		if (CNF.prefixes_enabled && parms.print_prefix) {
			brrsz w = fwrite(pri.pfx, pri._pfxl, 1, F);
			if (w < 1) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
			W += w;
		}

		if (CNF.verbose && parms.verbose) {
			/* TODO */
		}

		{ int w = fprintf(F, _F.d, args);
			if (w < 0) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
			W += w;
		}
		if (CNF.newlines_enabled && parms.print_newline) {
			int r = fputc('\n', F);
			if (r == EOF) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
			W += 1;
		}
		return W;
	} else {
		char *B = dst.dst;
		if (CNF.prefixes_enabled && parms.print_prefix) {
			memcpy(B + W, pri.pfx, pri._pfxl);
			W += pri._pfxl;
		}

		if (CNF.verbose && parms.verbose) {
			/* TODO */
		}

		{ int w = sprintf(B, _F.d, args);
			if (w < 0) {
				brrapi_sete(BRRAPI_E_LIBC);
				return BRRSZ_MAX;
			}
			W += w;
		}

		if (CNF.newlines_enabled && parms.print_newline) {
			B[W++] = '\n';
		}

		B[W] = 0;
		return W;
	}
}
brrsz BRRCALL
brrlog(brrlog_out_t dst, brrlog_parms_t parms, const char *const fmt, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	brrsz mlen = brrlogv(dst, parms, fmt, lptr);
	va_end(lptr);
	return mlen;
}
