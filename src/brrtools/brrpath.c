/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrpath.h"

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#if brrplat_unix
# include <sys/stat.h>
#elif brrplat_dos
# include <aclapi.h>
# define _win_ulrg(_hi_, _lo_) (((ULARGE_INTEGER){.u={.LowPart=(_lo_),.HighPart=(_hi_)}}).QuadPart)
#endif

#ifdef brrtools_debug
#include "brrdbg.h"
#endif

/* Returns nonzero when dstofs has reached end of write buffer, or srcofs has reached end of read buffer */
static BRR_inline int cpy_limit(void *restrict dst,
	brrsz dstlen,
	brrsz *restrict const dstofs,
	const void *restrict const src,
	brrsz srclen,
	brrsz *restrict const srcofs,
	brrsz n)
{
	/* nd = 'n bytes to write to dst', ns = 'n bytes to read from source' */\

	brrsz o = dstofs ? *dstofs : 0, i = srcofs ? *srcofs : 0;
	if (o >= dstlen || i >= srclen)
		return 1;

	/* dw = 'dst available to write', sr = 'source available to read' */\
	brrsz dw = dstlen - o, sr = srclen - i;
	if (!dw) {
		if (!sr) {
			return n > 0;
		} else {
			brrsz ns = n > sr ? sr : n;
			if (srcofs)
				*srcofs += ns;
			return ns != n;
		}
	} else if (!sr) {
		return n > 0;
	} else {
		brrsz nd = n > sr ? sr : n;
		const brrsz ns = n;
		if (nd > dw)
			nd = dw;
		memcpy((char*)dst + o, (char*)src + i, nd);
		if (dstofs)
			*dstofs += nd;
		if (srcofs)
			*srcofs += ns;
		return nd != n;
	}
}

void BRRCALL
brrpath_free(brrpath_t *const path)
{
	if (!path)
		return;
	if (path->full)
		free(path->full);
	memset(path, 0, sizeof(*path));
}

typedef struct i_pfx {
	const char *s;
	brrsz l;

	union {
		brrpath_inf;
		brrpath_inf_t f;
	};

	#define pfx_none 0
	#define pfx_drive 1
	#define pfx_unc 2
	#define pfx_namespace 3
	#define pfx_global 4
	int pfx_type;
} i_pfx_t;

#if brrplat_unix
#define i_get_pfx_type(_p_, _l_) ((i_pfx_t){.s=NULL, .l=0})
#elif brrplat_dos
static inline i_pfx_t BRRCALL i_get_pfx_type(const char *const p, brrsz l) {
	#define i_caseless(_c_, _x_) (((_c_) == (_x_)) || ((_c_) == (_x_) + ('a' - 'A')))
	#define i_pfx(_s_, ...) ((i_pfx_t){.s=(_s_), .l=sizeof(_s_) - 1, __VA_ARGS__})

	if (l >= 2) {
		if ((p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z')) {
			/* Path starts with an ascii character */
			if (p[1] == ':') {
				/* Path looks like '[A-Za-z]:*' */
				return i_pfx("A:", .type = pfx_drive, .abs = l > 2 && brrpath_issep(p[2]));
			}
			/* Path is something else */
		} else if (brrpath_issep(p[0])) {
			if (brrpath_issep(p[1])) {
				/* Path looks like '\\*' */
				if (brrpath_issep(p[3])) {
					if (p[2] == '?') /* Path looks like '\\?\*' */
						return i_pfx("\\\\?\\", .type = pfx_namespace, .abs=1);
					else if (p[2] == '.') /* Path looks like '\\.\*' */
						return i_pfx("\\\\.\\", .type = pfx_namespace, .abs=1);
				}
				return i_pfx("\\\\", .type = pfx_unc, .abs=1);
			} else {
				/* Path looks like '\*' */
				if (l >= sizeof("\\Global??\\") - 1) {
					if (i_caseless(p[1], 'G') && i_caseless(p[2], 'L')
					 && i_caseless(p[3], 'O') && i_caseless(p[4], 'B')
					 && i_caseless(p[5], 'A') && i_caseless(p[6], 'L')
					 && p[7] == '?' && p[8] == '?' && brrpath_issep(p[9])) {
						/* Path is '\Global??\*' */
						return i_pfx("\\Global??\\", .type = pfx_global, .abs=1);
					}
				} else if (l >= sizeof("\\??\\") - 1) {
					if (p[1] == '?' && p[2] == '?' && brrpath_issep(p[3])) {
						/* Path is '\??\*' */
						return i_pfx("\\??\\", .type = pfx_global, .abs=1);
					}
				}
				/* Path is something else */
			}
		}
	}
	return (i_pfx_t){0};
}
#endif

/* Returns the length of the canonicalized path without the null terminator
 * max_len includes the null terminator
 * */
static inline brrsz BRRCALL
i_canon(const char *restrict const source, brrsz length,
	char *restrict const dst, brrpath_path_len_t max_len,
	brrpath_inf_t *restrict const flags)
{
	brrpath_inf_t f = {0};
	brrsz i = 0, o = 0;
	#if brrplat_dos
	{ const i_pfx_t pfx = i_get_pfx_type(source, length);
		if (pfx.pfx_type == pfx_none) {
			/* Need not concern ourselves with the prefix-less case. */
		} else if (pfx.pfx_type == pfx_drive || pfx.pfx_type == pfx_global) {
			if (cpy_limit(dst, max_len, &o, source, length, &i, pfx.l))
				goto _E;
		} else if (pfx.pfx_type == pfx_unc || pfx.pfx_type == pfx_namespace) {
			/* No processing is needed for unc/namespace paths, they are parsed verbatim */
			cpy_limit(dst, max_len, &o, source, length, &i, length);
			dst[o] = 0;
			*flags = f;
			return o;
		}
		f = pfx.f;
	}
	#endif
	/* Dos-specific prefixes taken care of, now to consume the path proper */
	if (brrpath_issep(source[i])) {
		f.abs = 1;
		while (i < length && brrpath_issep(source[i])) ++i;
		if (o < max_len)
			dst[o++] = brrpath_separator;
	}

	static char comp[brrpath_max_name];
	brru1 n_seps = 0; /* Will wrap over and under appropriately; this setup should only fail if there are more than 256 '../' elements in a row */
	#define max_path_seps 256 /* Does this seem like a reasonable maximum on seps (components) in the output? */
	brrpath_path_len_t seps[max_path_seps] = {o};
	while (i < length && o < max_len) {
		int comp_len = 0;
		/* Read only the valid characters of the current component */
		while (i < length && !brrpath_issep(source[i])) {
			if (comp_len < sizeof(comp) && brrpath_norm(source[i]))
				comp[comp_len++] = source[i++];
		}
		comp[comp_len] = 0;
		/* Skip consecutive separators */
		while (i < length && brrpath_issep(source[i])) ++i;

		if (comp[0] == '.' && comp_len == 1) {
			/* do literally nothing */
		} else if (comp[0] == '.' && comp[1] == '.' && comp_len == 2) {
			if (o != seps[0] && seps[n_seps - (brru1)1] >= seps[0]) {
				/* If we have previous valid components to remove, remove them by simply decrementing the
				 * output write position */
				o = seps[--n_seps]; dst[o] = 0;
			} else if (!f.abs) {
				/* Otherwise, we've run out of valid components and have to add the current to the beginning
				 * of the output */
				/* But if the path is absolute, '/' and '/..' are the same path, so no need to add any '..' */
				if (o + 2 >= max_len)
					break; /* opt not to write an obviously invalid component */
				dst[o++] = '.'; dst[o++] = '.';
				if (o + 1 >= max_len)
					break;
				dst[o++] = brrpath_separator; dst[o] = 0;
				seps[0] = o;
				n_seps = 0;
			}
		} else {
			/* add the component like normal */
			if (cpy_limit(dst, max_len, &o, comp, comp_len, NULL, comp_len))
				break;
			if (o + 1 > max_len)
				break;
			dst[o++] = brrpath_separator; dst[o] = 0;
			seps[++n_seps] = o;
		}
	}
#if brrplat_dos
_E:
#endif
	if (o > 1 && brrpath_issep(dst[o-1])) {
		/* If the last char is a separator, remove it, but only if it's not the only character (i.e. the root
		 * dir) */
		o--;
	}
	dst[o] = 0;
	*flags = f;
	return o;
}

brrpath_t BRRCALL
brrpath_canonicalize(const char *const source, brrsz length)
{
	static char path[brrpath_max_path + 1];
	if (!length)
		return (brrpath_t){0};

	brrpath_t V = {0};
	V.len = i_canon(source, length, path, sizeof(path), (void*)&V.inf);
	if (!(V.full = malloc(V.len + 1))) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return V;
	}
	memcpy(V.full, path, V.len + 1);

	if (V.len && !brrpath_issep(V.full[V.len - 1])) {
		/* Yay! */
		brrsz name_len = V.len;
		while (name_len && !brrpath_issep(V.full[name_len - 1])) --name_len;
		char *name = V.full + name_len;
		name_len = V.len - name_len; /* Length of the whole name component */
		while (V.ext_len < name_len && name[V.ext_len] == '.') ++V.ext_len; // skip any leading dots
		while (V.ext_len < name_len && name[V.ext_len] != '.') ++V.ext_len; // count non-dots until the first
		if (V.ext_len != name_len) {
			V.base_len = V.ext_len;
			V.ext_len = name_len - V.ext_len - 1;
		} else {
			/* No logical dots */
		}
	}

	return V;
}

brrpath_path_len_t BRRCALL
brrpath_dirlen(const brrpath_t *const path)
{
	if (!path)
		return 0;
	return path->len - path->base_len - (path->base_len != path->len) - path->ext_len - (path->ext_len != 0);
}

int BRRCALL
brrpath_cmp(const char *const a, const char* const b, brrsz n)
{
	if (!a || !b)
		return 1;
	if (!n)
		return 0;
	brrsz i = 0;
	while (i < n && a[i] && b[i] && (a[i] == b[i] || (brrpath_issep(a[i]) && brrpath_issep(b[i])))) ++i;
	if (i < n)
		return (int)a[i] - (int)b[i];
	return 0;
}
