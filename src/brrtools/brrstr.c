#include "brrtools/brrstr.h"

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

BRRAPI int BRRCALL
brrstr_cmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z)
{
	if (!s || !z)
		return 0;
	return strncmp(s->s, z->s, s->l < z->l ? s->l : z->l);
}
BRRAPI int BRRCALL
brrstr_icmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z)
{
	if (!s || !z)
		return 0;
	return i_strnicmp(s->s, z->s, s->l < z->l ? s->l : z->l);
}

BRRAPI brrsz BRRCALL
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

BRRAPI brrstr_t BRRCALL
brrstr_new(const char *const str, brrsz max_len)
{
	brrstr_t s = {.l=brrstr_len(str, max_len)};
	if (!(s.s = calloc(1 + s.l, 1))) {
		brrapi_sete(BRRAPI_E_MEMERR);
		return (brrstr_t){0};
	}
	memcpy(s.s, str, s.l);
	return s;
}
BRRAPI int BRRCALL
brrstr_init(brrstr_t *const str, const char *const cstr, brrsz max_len)
{
	if (!str) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return -1;
	}
	brrstr_t s = brrstr_new(cstr, max_len);
	if (!s.s)
		return -1;
	*str = s;
	return 0;
}
BRRAPI void BRRCALL
brrstr_free(brrstr_t *const str)
{
	if (!str || str->_shallow)
		return;
	if (str->s)
		free(str->s);
	memset(str, 0, sizeof(*str));
}

BRRAPI int BRRCALL
brrstr_copy(brrstr_t *restrict const dst, const brrstr_t *restrict const src)
{
	if (!dst) {
		brrapi_sete(BRRAPI_E_ARGERR);
		return -1;
	} else if (!src) {
		return 0;
	}
	if (dst->_shallow) {
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

BRRAPI int BRRCALL
brrstr_3join(brrstr_t *dst, const brrstr_t a, const brrstr_t b, const brrstr_t c)
{
	if (!dst)
		return -1;
	brrstr_t s = {.l = a.l + b.l + c.l};
	if (!s.l)
		return brrstr_init(dst, "", 0);
	if (!(s.s = calloc(1 + s.l, 1)))
		return -1;
	brrsz w = 0;
	if (a.l) {
		memcpy(s.s + w, a.s, a.l);
		w += a.l;
	}
	if (b.l) {
		memcpy(s.s + w, b.s, b.l);
		w += b.l;
	}
	if (c.l) {
		memcpy(s.s + w, c.s, c.l);
		w += c.l;
	}
	*dst = s;
	return 0;
}
