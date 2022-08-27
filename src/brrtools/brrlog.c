/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */
#define _brrlog_keep_gens
#include "brrtools/brrlog.h"
#undef _brrlog_keep_gens

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrstringr.h"

#define CNF brrlog_config
typedef struct i_buf
{
	brrsz alloc;
	char *d;
	float factor;
	int heur, h;
} i_buf_t;
static BRR_inline int BRRCALL i_buf_resize(i_buf_t *const B, brrsz sl)
{
/* I dislike having to alloc at all, but this is the cleanest solution I've come up with so far */
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

static i_buf_t _F = { .factor = 1.5, .heur = 10, };

typedef int (*i_print_t)(void *, const char *, ...);
typedef int (*i_vprint_t)(void *, const char *, va_list);

static FILE *i_last_stream = NULL;
#if !_brrlog_can_style
#define I_CLEAR {.st={0}}
#define I_UNSET {.st={0}}
#else
#define I_CLEAR {._s=ST_CLR, ._l=sizeof(ST_CLR)-1}
#define _brrlog_max_sts_op(_i_) -1,
#define I_UNSET {.st={_brrlog_max_sts(_brrlog_max_sts_op)}, -1, -1, -1}

static brrlog_style_t i_last_st = I_CLEAR;

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

#define i_st(_st_) ((_st_) >= 0 && (_st_) < brrlog_st_count ? i_sts[_st_] : i_sts[0])
#define i_fg(_fg_) ((_fg_) >= 0 && (_fg_) < brrlog_col_count ? i_fgs[_fg_] : i_fgs[0])
#define i_bg(_bg_) ((_bg_) >= 0 && (_bg_) < brrlog_col_count ? i_bgs[_bg_] : i_bgs[0])
#define i_fn(_fn_) ((_fn_) >= 0 && (_fn_) < brrlog_fn_count ? i_fns[_fn_] : i_fns[0])
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
		case '0': return brrlog_fn_normal; case '5': return brrlog_fn_5;
		case '1': return brrlog_fn_1;      case '6': return brrlog_fn_6;
		case '2': return brrlog_fn_2;      case '7': return brrlog_fn_7;
		case '3': return brrlog_fn_3;      case '8': return brrlog_fn_8;
		case '4': return brrlog_fn_4;      case '9': return brrlog_fn_9;
		default: return -1;
	}
}
static BRR_inline brrsz BRRCALL i_style_print(char *const dst, brrlog_style_t st)
{
#define _pr_style(_w_, _dst_, _st_, _type_, _field_) do {\
	if ((_st_)._field_ > -1) {\
		i_sf_t sf = i_##_type_((_st_)._field_);\
		memcpy((_dst_) + (_w_), sf.s, sf.l);\
		(_w_) += sf.l;\
	}\
} while (0)
	brrsz w = 0;
	memcpy(dst + w, ST_PFX, sizeof(ST_PFX) - 1);
	w += sizeof(ST_PFX) - 1;
#define _op(_i_) _pr_style(w, dst, st, st, st[_i_]);
	_brrlog_max_sts(_op)
#undef _op
	_pr_style(w, dst, st, fg, fg);
	_pr_style(w, dst, st, bg, bg);
	_pr_style(w, dst, st, fn, fn);
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
	 *   See 'i_colchr' for what 'f' and 'b' values map to
	 *   See 'i_stchr' for what 's' values map to
	 *   See 'i_fnchr' for what '_' values map to
	 * */

	const char *S = pfx;
	const char *E = pfx + pfxl;
	while(isspace(*S))S++;
	if(S==E)return 0;
	while(isspace(*(E-1)))E--;
	if(E==S)return 0;
	//printf("\n\nNICE\n\n");

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
					case 's': case 'S': mode = 1; break;
					case 'f': case 'F': mode = 2; break;
					case 'b': case 'B': mode = 3; break;
					case '_': mode = 4; break;
					default:  mode = 0; break;
				}
			}
		}
		#define _op(_i_) if (s.st[_i_] == -2) s.st[_i_] = i_last_st.st[_i_];
		_brrlog_max_sts(_op);
		#undef _op
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
#if _brrlog_can_style
	p._l = i_style_print(p._s, p);
#endif
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

#define _max_copy(_D_, _S_, _SL_) do {\
	brrsz _M = CNF.max_log - W, _SL = _SL_;\
	if (_SL > _M) _SL = _M;\
	if (_SL) { memcpy(_D_, _S_, _SL); W += _SL; }\
} while (0)
#define _copy(_D_, _S_, _SL_) do {\
	brrsz _SL = _SL_;\
	if (_SL) { memcpy(_D_, _S_, _SL); W += _SL; }\
} while (0)

typedef struct i_stylable
{
	const char *pfx;
	brrsz pfxl;
	const char *cnt;
	brrsz cntl;
} i_stylable_t;
static i_stylable_t sty;
/* Returns 0 when there are no stylables */
static inline int BRRCALL i_next_stylable(const char *const fmt, brrsz fmtl, brrsz *ofs, i_stylable_t *sty)
{
	if (!CNF._sty_open || !CNF._sty_len) {
		return 0;
	} else if (fmtl <= 2 * CNF._sty_len + 1 /* +1 for prefix sep, always ':' */) {
		*sty = (i_stylable_t){0};
		*ofs += fmtl;
		return 0;
	}
	const brrsz str_max = fmtl + 1 - CNF._sty_len;

	/* (! prefix : content !) */
	for (brrsz i = 0, escaped = 0; i < str_max; ++i) {
		if (escaped) {
			escaped = 0;
		} else if (fmt[i] == '\\') {
			escaped = 1;
		} else if (!memcmp(fmt + i, CNF._sty_open, CNF._sty_len)) {
			brrsz cnt = i + CNF._sty_len;
			brrsz pfx = BRRSZ_MAX;
			for (;cnt < str_max && memcmp(fmt + cnt, CNF._sty_close, CNF._sty_len); ++cnt) {
				if (fmt[cnt] == ':' && pfx == BRRSZ_MAX)
					pfx = cnt;
			}
			if (cnt == str_max || pfx == BRRSZ_MAX)
				continue;

			/* Found a complete stylable section, with no guarantee that the prefix has any length */
			*sty = (i_stylable_t){
				.pfx = fmt + i + CNF._sty_len,
				.pfxl = pfx - i - CNF._sty_len,
				.cnt = fmt + pfx + 1,
				.cntl = cnt - pfx - 1,
			};

			/* Set the current offset after the closing token */
			*ofs += cnt + CNF._sty_len;
			return 1;
		}
	}
	*ofs += str_max + 1;
	return 0;
}
/* TODO the maximum formatted length up to ML should be put into ml
 * More specifically, ml should store the styled length up to the last styled section that will fit into the
 * total allowed ML bytes.
 * However, this still leaves in question what to do when the distance between styled sections, or the
 * content of a styled section, is greater than ML?
 * I want to avoid splitting up the format string in a way that would unwittingly split up any format
 * specifier. This is the main trouble I've had, coming up with solutions to that. */
static inline brrsz BRRCALL i_styled_len(const char *const s, brrsz sl, brrsz *ml, brrsz ML, brrlog_style_t surround)
{
	brrsz fofs = 0, sty_tkns = 0, sty_bytes = 0;
	while (i_next_stylable(s + fofs, sl - fofs, &fofs, &sty)) {
		brrlog_style_t st;
		if (i_parse_style_prefix(&st, sty.pfx, sty.pfxl)) {
			sty_tkns += sty.pfxl + 1 + 2 * CNF._sty_len;
#if _brrlog_can_style
			if (sty.cntl) {
				if (CNF.style_enabled)
					sty_bytes += st._l + surround._l;
				i_last_st = st;
			}
#endif
		}
	}
#if _brrlog_can_style
	i_last_st = (brrlog_style_t)I_CLEAR;
#endif
	return fofs - sty_tkns + sty_bytes;
}
static inline brrsz BRRCALL i_styled(char *const D, const char *const s, brrsz sl, brrlog_style_t sur)
{
	brrsz fofs = 0, fofsold = 0;
	brrsz W = 0;
	brrlog_style_t st;
	while (i_next_stylable(s + fofs, sl - fofs, &fofs, &sty)) {
		if (!i_parse_style_prefix(&st, sty.pfx, sty.pfxl)) {
			_copy(D + W, s + fofsold, fofs - fofsold);
		} else {
			brrsz bet_len = fofs - fofsold - CNF._sty_len - sty.pfxl - 1 - sty.cntl - CNF._sty_len;
			_copy(D + W, s + fofsold, bet_len);
			if (sty.cntl) {
				#if _brrlog_can_style
				if (CNF.style_enabled)
					_copy(D + W, st._s, st._l);
				#endif
				_copy(D + W, sty.cnt, sty.cntl);
				#if _brrlog_can_style
				if (CNF.style_enabled)
					_copy(D + W, sur._s, sur._l);
				i_last_st = st;
				#endif
			}
		}
		fofsold = fofs;
	}
	_copy(D + W, s + fofsold, fofs - fofsold);
	D[W] = 0;
	return W;
}

/* Initialize 'pri' by parsing the style of 'pfx' and "canonicalizing" the string, flattening it to be
 * static so that it doesn't also need to be styled when logging every message */
int BRRCALL
brrlog_priority_init(brrlog_priority_t *const pri, const char *const pfx, brrlog_style_t style, brrlog_dst_t dst)
{
	if (!pri) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return 1;
	}

	brrlog_priority_t p = {.style = brrlog_style_init(style), .dst = dst};
	if (!dst.dst || (dst.type != brrlog_dst_stream && dst.type != brrlog_dst_buffer)) {
		if (dst.type == brrlog_dst_stream) {
			p.dst = (brrlog_dst_t){.dst = stderr, .type = brrlog_dst_stream};
		} else {
			p.dst = (brrlog_dst_t){.dst = NULL, .type = 0};
		}
	}
	if (!pfx) {
		*pri = p;
		return 0;
	}

	brrsz ml = brrstringr_length(pfx, CNF.max_prefix);
	if (ml) {
		brrsz xl = 0;
		brrsz sl = i_styled_len(pfx, ml, &xl, 2 * CNF.max_prefix, p.style);
		if (!(p.pfx = calloc(1 + sl, 1))) {
			brrapi_sete(BRRAPI_E_ARGERR);
			return 1;
		}
		p._pfxl = sl;
#if 0
		if (sl == ml)
			memcpy((char*)p.pfx, pfx, ml);
		else
#endif
			i_styled((char*)p.pfx, pfx, ml, p.style);
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

const brrlog_cfg_t brrlog_default_cfg = {
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
		.pfx = "(!f=Bs=r:[INV]!)(!f=-::!) ",
		.style = I_CLEAR,
		.dst = {
			.type=brrlog_dst_stream,
			.dst=NULL,
		},
		._pfxl = 5,
	},

	.max_log = 2048,
	.max_prefix = 32,
};
brrlog_cfg_t brrlog_config = {0};

static BRR_inline brrsz BRRCALL i_pri_index(int label)
{
	for (brrsz i = 0; i < CNF._npri; ++i) if (CNF._lbl[i]==label) return i;
	return CNF._npri;
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
		if (brrlog_priority_init(&p, cfg.def_pri.pfx, cfg.def_pri.style, cfg.def_pri.dst)) {
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
int BRRCALL
brrlog_def_pri(brrlog_priority_t newpri)
{
	brrlog_priority_t p = {0};
	if (brrlog_priority_init(&p, newpri.pfx, newpri.style, newpri.dst))
		return 1;
	brrlog_priority_free(&CNF.def_pri);
	CNF.def_pri = p;
	return 0;
}
int BRRCALL
brrlog_maxlog(brrsz newmax)
{
	if (newmax == CNF.max_log)
		return 0;

	if (!newmax) {
		if (CNF._log_buf)
			free(CNF._log_buf);
		CNF._log_buf = NULL;
	}
	char *new = realloc(CNF._log_buf, newmax);
	if (!new) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return 1;
	}
	CNF._log_buf = new;
	CNF.max_log = newmax;
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
	if (brrlog_priority_init(&p, new.pfx, new.style, new.dst))
		return 1;

	if (i != CNF._npri) {
		/* Priority label was found, update */
		brrlog_priority_free(&CNF._pri[i]);
		CNF._pri[i] = p;
	} else {
		/* Priority label not found and array not full, add new priority */
		brrlog_priority_t *P;
		int *L = malloc((sizeof(*L) + sizeof(*P)) * (CNF._npri + 1));
		if (!L) {
			brrlog_priority_free(&p);
			brrapi_sete(BRRAPI_E_MEMERR);
			return 1;
		}
		memcpy(L, CNF._lbl, sizeof(*L) * CNF._npri);
		L[CNF._npri] = label;

		P = (brrlog_priority_t*)((char*)L + sizeof(*L) * (CNF._npri + 1));
		memcpy(P, CNF._pri, sizeof(*P) * (CNF._npri));
		P[CNF._npri] = p;
		if (CNF._lbl)
			free(CNF._lbl);

		CNF._lbl = L;
		CNF._pri = P;
		CNF._npri++;
	}
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

	const brrsz new_size = (sizeof(*CNF._lbl) + sizeof(*CNF._pri)) * (CNF._npri - 1);
	brru1 *new = malloc(new_size); /* only point of error */
	if (!new) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return 1;
	}
	brrlog_priority_free(&CNF._pri[i]); /* could be returned to the user instead of freed */

	const brrsz first_len = sizeof(*CNF._lbl) * i;
	if (first_len) {
		memcpy(new, CNF._lbl, first_len);
	}
	const brrsz mid_start = first_len + sizeof(*CNF._lbl); /* skip the label to remove */
	const brrsz mid_len = sizeof(*CNF._lbl) * (CNF._npri - 1 - i) + sizeof(*CNF._pri) * i;
	memcpy(new + first_len, (brru1*)CNF._lbl + mid_start, mid_len);
	brrsz last_len = sizeof(*CNF._pri) * (CNF._npri - 1 - i);
	if (last_len) {
		brrsz last_start = mid_start + mid_len + sizeof(*CNF._pri); /* skip the pri to remove */
		memcpy(new + first_len + mid_len, (brru1*)CNF._pri + last_start, last_len);
	}

	free(CNF._lbl);
	CNF._npri--;
	CNF._lbl = (void*)new;
	CNF._pri = (void*)(new + sizeof(*CNF._lbl) * CNF._npri);

	return 0;
}

#define _print(_D_, _type_, ...) do {\
	int _t = _type_##printf(_D_,  __VA_ARGS__);\
	if (_t < 0) { brrapi_sete(BRRAPI_E_LIBC); return BRRSZ_MAX; }\
	W += _t;\
} while (0)
#define _write(_D_, _S_, _SL_) do {\
	int _t = fwrite(_S_, _SL_, 1, _D_);\
	if (_t < 1) { brrapi_sete(BRRAPI_E_LIBC); return BRRSZ_MAX; }\
	W += _t;\
} while (0)
#define _put(_D_, _C_) do { int t = fputc(_C_, _D_); if (t == EOF) { brrapi_sete(BRRAPI_E_LIBC); return BRRSZ_MAX; } W++; } while (0)

#define VBS_FMT "(%s,%s@%3i)", src.func, src.file, src.line
static BRR_inline brrlog_priority_t i_select_pri(int label)
{
	brrsz i = i_pri_index(label);
	if (i == CNF._npri)
		return CNF.def_pri;
	else
		return CNF._pri[i];
}
static BRR_inline brrsz i_min_len(brrlog_priority_t pri, brrlog_source_t src, brrlog_settings_t settings)
{
	brrsz W = 0;
	if (CNF.verbose && settings.verbose)
		_print(NULL, sn, 0, VBS_FMT);
	if (CNF.prefixes_enabled && settings.print_prefix)
		W += pri._pfxl;
	if (CNF.newlines_enabled && settings.print_newline)
		W++;
	return W;
}
static BRR_inline brrsz i_vtext(brrlog_priority_t pri, brrlog_source_t src, brrlog_settings_t settings, const char *const fmt, va_list args)
{
	if (!pri.dst.dst || pri.dst.type <= 0 || pri.dst.type > 2)
		return i_min_len(pri, src, settings) + snprintf(NULL, 0, fmt, args);
	brrsz W = 0;
	if (CNF.max_log) {
		char *L = CNF._log_buf;
		if (CNF.verbose || settings.verbose)
			_print(L + W, sn, CNF.max_log - W + 1, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix)
			_max_copy(L + W, pri.pfx, pri._pfxl);
		if (W < CNF.max_log)
			_print(L + W, vsn, CNF.max_log - W + 1, fmt, args);
		if (CNF.newlines_enabled && settings.print_newline)
			if (W < CNF.max_log)
				L[W++] = '\n';
		L[W] = 0;

		if (pri.dst.type == brrlog_dst_stream) {
			if (i_last_stream && i_last_stream != pri.dst.dst && (CNF.flush_enabled || CNF.flush_always))
				fflush(i_last_stream);
			_write(pri.dst.dst, L, W);
			if (CNF.flush_always)
				fflush(pri.dst.dst);
			i_last_stream = pri.dst.dst;
		} else {
			memcpy(pri.dst.dst, L, W + 1);
		}
	} else if (pri.dst.type == brrlog_dst_stream) {
		FILE *F = pri.dst.dst;
		if (i_last_stream && i_last_stream != pri.dst.dst && (CNF.flush_enabled || CNF.flush_always))
			fflush(i_last_stream);
		if (CNF.verbose || settings.verbose)
			_print(F, f, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix)
			_write(F, pri.pfx, pri._pfxl);
		_print(F, vf, fmt, args);
		if (CNF.newlines_enabled && settings.print_newline)
			_put(F, '\n');
		if (CNF.flush_always)
			fflush(F);
		i_last_stream = F;
	} else {
		char *B = pri.dst.dst;
		if (CNF.verbose || settings.verbose)
			_print(B + W, s, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix) {
			memcpy(B + W, pri.pfx, pri._pfxl);
			W += pri._pfxl;
		}
		_print(B + W, s, fmt, args);
		if (CNF.newlines_enabled && settings.print_newline)
			B[W++] = '\n';
		B[W] = 0;
	}
	return W;
}

/* TODO Reallocating _F will fail for inputs too large, in such cases 'fmt' should be iterated through,
 * styling into a fixed-size _F and printing _F, until all of 'fmt' is consumed or CNF.max_log is reached */
brrsz BRRCALL
brrlogv_text(int label, brrlog_source_t src, brrlog_settings_t settings, const char *const fmt, va_list args)
{
	const brrsz fmtl = strlen(fmt);
	if (!fmt) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return BRRSZ_MAX;
	} else if (!fmtl || (!CNF.debug && (label < CNF.min_label || label > CNF.max_label))) {
		return 0;
	}

	brrlog_priority_t pri = i_select_pri(label);
	brrsz ml = 0;
	brrsz sl = i_styled_len(fmt, fmtl, &ml, 2048, pri.style);
	/* TODO
	if (sl > some maximum allocatable) {

	} else
	 */
#if 0
	if (sl == fmtl) { /* Nothing to style */
		/* I'm not sure this will hold for all possible, valid styled sections?
		 * Please check, it will likely depend on a given _sty_len */
		return i_vtext(pri, src, settings, fmt, args);
	}
#endif
	if (i_buf_resize(&_F, sl)) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return BRRSZ_MAX;
	}
	i_styled(_F.d, fmt, fmtl, pri.style);
	return i_vtext(pri, src, settings, _F.d, args);
}
brrsz BRRCALL
brrlog_text(int label, brrlog_source_t src, brrlog_settings_t settings, const char *const fmt, ...)
{
	va_list lptr;
	va_start(lptr, fmt);
	brrsz mlen = brrlogv_text(label, src, settings, fmt, lptr);
	va_end(lptr);
	return mlen;
}

#define _set_bit(_byte_, _bit_) do { D[o++] = B[_byte_] & (1 << (_bit_)) ? '1' : '0'; } while (0)
#define _set_bit_with_sep(_byte_, _bit_) do {\
	if (o && o % separator_spacing == md)\
		D[o++] = bit_separator;\
	_set_bit(_byte_, _bit_);\
} while (0)
#define _bits(_setter_, _byte_, _maxbit_) do {\
	for (brrsz i = 0; i < _maxbit_; ++i)\
		_setter_(_byte_, i);\
} while (0)
#define _rbits(_setter_, _byte_, _maxbit_) do {\
	for (brrsz i = _maxbit_; i > 0; --i)\
		_setter_(_byte_, i - 1);\
} while (0)
#define _bytes(_bits_, _setter_) do {\
	for (brrsz byte = 0; byte < div.quot; ++byte)\
		_bits_(_setter_, byte, BYTE);\
	_bits_(_setter_, div.quot, div.rem);\
} while (0)
#define _rbytes(_bits_, _setter_) do {\
	_bits_(_setter_, div.quot, BYTE);\
	for (brrsz byte = div.quot; byte > 0; --byte)\
		_bits_(_setter_, byte - 1, BYTE);\
} while (0)
brrsz BRRCALL
brrlog_bits(int label,
	brrlog_source_t src,
	brrlog_settings_t settings,
	const void *const bytes,
	brrsz n_bits,
	brrsz separator_spacing,
	int reverse_bits,
	int reverse_bytes,
	char bit_separator)
{
	if (!bytes) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return BRRSZ_MAX;
	} else if (!n_bits) {
		return 0;
	}
	brrsz olen = n_bits, md = 0;
	if (separator_spacing) {
		md = n_bits % separator_spacing;
		olen += (n_bits - 1) / separator_spacing;
	}

	brrlog_priority_t pri = i_select_pri(label);
	if (!pri.dst.dst || pri.dst.type <= 0 || pri.dst.type > 2)
		return i_min_len(pri, src, settings) + olen;

	if (i_buf_resize(&_F, olen)) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return BRRSZ_MAX;
	}

	const brrsz BYTE = 8;
	imaxdiv_t div = imaxdiv(n_bits, BYTE);
	brrsz o = 0;
	const char *B = bytes;
	char *D = _F.d;
	if (!separator_spacing) {
		if (!reverse_bytes) {
			if (!reverse_bits) _bytes(_bits, _set_bit);
			else _bytes(_rbits, _set_bit);
		} else {
			if (!reverse_bits) _rbytes(_bits, _set_bit);
			else _rbytes(_rbits, _set_bit);
		}
	} else {
		if (!reverse_bytes) {
			if (!reverse_bits) _bytes(_bits, _set_bit_with_sep);
			else _bytes(_rbits, _set_bit_with_sep);
		} else {
			if (!reverse_bits) _rbytes(_bits, _set_bit_with_sep);
			else _rbytes(_rbits, _set_bit_with_sep);
		}
	}

	brrsz W = 0;
	if (CNF.max_log) {
		char *L = CNF._log_buf;
		if (CNF.verbose || settings.verbose)
			_print(L + W, sn, CNF.max_log - W + 1, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix)
			_max_copy(L + W, pri.pfx, pri._pfxl);
		if (W < CNF.max_log)
			_max_copy(L + W, _F.d, o);
		if (CNF.newlines_enabled && settings.print_newline)
			if (W < CNF.max_log)
				L[W++] = '\n';
		L[W] = 0;

		if (pri.dst.type == brrlog_dst_stream) {
			if (i_last_stream && i_last_stream != pri.dst.dst && (CNF.flush_enabled || CNF.flush_always))
				fflush(i_last_stream);
			_write(pri.dst.dst, L, W);
			if (CNF.flush_always)
				fflush(pri.dst.dst);
			i_last_stream = pri.dst.dst;
		} else {
			memcpy(pri.dst.dst, L, W + 1);
		}
	} else if (pri.dst.type == brrlog_dst_stream) {
		FILE *F = pri.dst.dst;
		if (i_last_stream && i_last_stream != F && (CNF.flush_enabled || CNF.flush_always))
			fflush(i_last_stream);
		if (CNF.verbose || settings.verbose)
			_print(F, f, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix)
			_write(F, pri.pfx, pri._pfxl);
		_write(F, _F.d, o);
		if (CNF.newlines_enabled && settings.print_newline)
			_put(F, '\n');
		if (CNF.flush_always)
			fflush(pri.dst.dst);
		i_last_stream = F;
	} else {
		char *B = pri.dst.dst;
		if (CNF.verbose || settings.verbose)
			_print(B + W, s, VBS_FMT);
		if (CNF.prefixes_enabled && settings.print_prefix) {
			memcpy(B + W, pri.pfx, pri._pfxl);
			W += pri._pfxl;
		}
		memcpy(B + W, _F.d, o);
		W += o;
		if (CNF.newlines_enabled && settings.print_newline)
			B[W++] = '\n';
		B[W] = 0;
	}
	return W;
}
