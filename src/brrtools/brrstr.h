/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrstr_h
#define brrstr_h

#ifdef __cplusplus
extern "C" {
#endif

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

typedef struct brrstr {
	char *s;
	brrsz l;
	int _shallow;
} brrstr_t;

BRRAPI int BRRCALL
brrstr_cmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z);
BRRAPI int BRRCALL
brrstr_icmp(const brrstr_t *restrict const s, const brrstr_t *restrict const z);
/* Compares 'str' to a va_list of 'char *' (cstrs), and returns the first cstr that compares equal, or
 * NULL if no such cstr, terminating at the first NULL cstr. */
BRRAPI char *BRRCALL
brrstr_ncmp(brrstr_t str, ...);
BRRAPI char *BRRCALL
brrstr_incmp(brrstr_t str, ...);

BRRAPI brrsz BRRCALL
brrstr_len(const char *const s, brrsz max);

static BRR_inline brrstr_t BRRCALL
brrstr_literal(const char *const str)
{
	return (brrstr_t){.s=(char*)str,.l=brrstr_len(str, -1), ._shallow=1};
}
#define brrstr_static(_c_) {.s=(_c_),.l=sizeof(_c_)-1, ._shallow=1}

BRRAPI brrstr_t BRRCALL
brrstr_new(const char *const str, brrsz max_len);
BRRAPI int BRRCALL
brrstr_init(brrstr_t *const str, const char *const cstr, brrsz max_len);
BRRAPI void BRRCALL
brrstr_free(brrstr_t *const str);

BRRAPI int BRRCALL
brrstr_copy(brrstr_t *restrict const dst, const brrstr_t *restrict const src);

/* Joins a va_list of brrstr's into 'dst', terminating at the first NULL brrstr ('.s' is NULL). */
BRRAPI int BRRCALL
brrstr_njoin(brrstr_t *dst, ...);

/* TODO maybe some splitting functions? emulated after python 'str.split()' possibly? I originally did
 * 'brrstringr_split' for reasons mostly unknown; maybe curiosity? Either way, there's no way they need to be
 * THAT complicated. */

#ifdef __cplusplus
}
#endif

#endif /* brrstr_h */
