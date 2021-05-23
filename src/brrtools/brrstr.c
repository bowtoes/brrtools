#include "brrtools/brrstr.h"

#include <string.h>

#include "brrtools/brrlib.h"

brrsz brrstr_maxlen = 4096;

brrsz BRRCALL
brrstr_len(const char *const s)
{
	if (!s) {
		return 0;
	} else {
		brrsz l = 0;
		for (;s[l] && l < brrstr_maxlen;++l);
		return l;
	}
}

char *BRRCALL
brrstr_dup(const char *const s, brrsz *const size)
{
	if (s) {
		char *r = NULL;
		brrsz l = brrstr_len(s);
		if (brrlib_alloc((void **)&r, l+1, 1)) {
			if (size) *size = l;
			return memcpy(r, s, l);
		}
	}
	return NULL;
}

char *BRRCALL
brrstr_merge(const char *const a, const char *const b, brrsz *const size)
{
	if (!a && !b) {
		return NULL;
	} else if (!a) {
		return brrstr_dup(b, size);
	} else if (!b) {
		return brrstr_dup(a, size);
	} else {
		char *r = NULL;
		brrsz la = brrstr_len(a), lb = brrstr_len(b), l = la+lb;
		if (brrlib_alloc((void **)&r, l+1, 1)) {
			memcpy(r, a, la);
			memcpy(r + la, b, lb);
			return r;
		}
	}

	return NULL;
}
