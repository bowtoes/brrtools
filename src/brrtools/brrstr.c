/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrstr.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#if brrplat_unix
# include <strings.h>
# define i_stricmp strcasecmp
# define i_strnicmp strncasecmp
#elif brrplat_dos
# define i_stricmp _stricmp
# define i_strnicmp _strnicmp
#endif

int BRRCALL
brrstr_cmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z)
{
	if (!s || !z)
		return 0;
	return strncmp(s->s, z->s, s->l < z->l ? s->l : z->l);
}
int BRRCALL
brrstr_icmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z)
{
	if (!s || !z)
		return 0;
	return i_strnicmp(s->s, z->s, s->l < z->l ? s->l : z->l);
}
char *BRRCALL
brrstr_ncmp(const brrstr_t str, ...)
{
	if (!str.s)
		return NULL;
	va_list lptr;
	va_start(lptr, str);
	char *a = NULL;
	for (brrsz i = 0; (a = va_arg(lptr, char *)) && strncmp(str.s, a, str.l); ++i);
	va_end(lptr);
	return a;
}
char *BRRCALL
brrstr_incmp(const brrstr_t str, ...)
{
	if (!str.s)
		return NULL;
	va_list lptr;
	va_start(lptr, str);
	char *a = NULL;
	for (brrsz i = 0; (a = va_arg(lptr, char *)) && i_strnicmp(str.s, a, str.l); ++i);
	va_end(lptr);
	return a;
}

brrsz BRRCALL
brrstr_len(const char *const s, brrsz max)
{
	if (max == -1) {
		brrsz i = 0;
		while (s[i])++i;
		return i;
	}
	for (brrsz i = 0; i < max; ++i) {
		if (!s[i])
			return i;
	}
	return max;
}

brrstr_t BRRCALL
brrstr_new(const char *const str, brrsz max_len)
{
	brrstr_t s = {.l=brrstr_len(str, max_len)};
	if (!(s.s = calloc(1 + s.l, 1))) {
		brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate %zu bytes for new brrstr", s.l);
		return (brrstr_t){0};
	}
	memcpy(s.s, str, s.l);
	return s;
}
int BRRCALL
brrstr_init(brrstr_t *restrict const str, const char *restrict const cstr, brrsz max_len)
{
	if (!str) {
		brrapi_error(BRRAPI_E_ARGERR, "Can't initialize brrstr to null 'brrstr_t *str'");
		return -1;
	}
	brrstr_t s = brrstr_new(cstr, max_len);
	if (!s.s)
		return -1;
	*str = s;
	return 0;
}

void BRRCALL
brrstr_free(brrstr_t *const str)
{
	if (!str || str->_s)
		return;
	if (str->s)
		free(str->s);
	memset(str, 0, sizeof(*str));
}

int BRRCALL
brrstr_copy(brrstr_t *restrict const dst, const brrstr_t *restrict const src)
{
	if (!dst) {
		brrapi_error(BRRAPI_E_ARGERR, "Can't copy brrstr to null 'brrstr_t *dst'");
		return -1;
	} else if (!src) {
		return 0;
	}
	if (dst->_s) {
		brrstr_t s = brrstr_new(src->s, src->l);
		if (!s.s)
			return -1;
		*dst = s;
		return 0;
	}
	if (dst->l != src->l) {
		char *n = realloc(dst->s, src->l + 1);
		if (!n)
			return -1;
		dst->s = n;
	}
	memcpy(dst->s, src->s, dst->l);
	return 0;
}

int BRRCALL
brrstr_njoin(brrstr_t *dst, ...)
{
	if (!dst)
		return -1;
	brrstr_t s = {0};
	va_list lptr;
	va_start(lptr, dst);
	{ brrsz i = 0;
		va_list lp2;
		va_copy(lp2, lptr);
		brrstr_t a;
		while (1) {
			a = va_arg(lp2, brrstr_t);
			if (!a.s)
				break;
			s.l += a.l;
			++i;
		}
		va_end(lp2);

		if (!(s.s = calloc(1 + s.l, 1))) {
			va_end(lptr);
			brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate %zu bytes for %zu joined brrstrs", i);
			return -1;
		}
	}
	brrsz w = 0;
	brrstr_t a;
	while (1) {
		a = va_arg(lptr, brrstr_t);
		if (!a.s)
			break;
		if (a.l) {
			memcpy(s.s + w, a.s, a.l);
			w += a.l;
		}
	}
	va_end(lptr);
	*dst = s;
	return 0;
}
