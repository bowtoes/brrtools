/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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
