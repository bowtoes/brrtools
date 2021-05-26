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

#include "brrtools/brrplatform.h"

#include <string.h>
#if defined(BRRPLATFORMTYPE_UNIX)
# include <strings.h>
#endif

brrstrlen BRRCALL
brrstr_cstrlen(const char *const cstr, brrstrlen max_strlen)
{
	brrsz s = 0;
	if (cstr && cstr[0]) {
		for (;s < max_strlen && cstr[s]; ++s);
	}
	return s;
}

brrstrlen BRRCALL
brrstr_strlen(const brrstrT *const str)
{
	brrsz l = brrbuffer_size(str);
	if (l) l--;
	return l;
}

brrstrT BRRCALL
brrstr_new(const char *const cstr, brrstrlen max_strlen)
{
	brrsz l = brrstr_cstrlen(cstr, max_strlen);
	brrstrT s = brrbuffer_new(l + 1);
	if (l)
		brrbuffer_write(&s, cstr, l);
	return s;
}

const char *BRRCALL
brrstr_cstr(const brrstrT *const str)
{
	return (const char *)brrbuffer_data(str);
}

static int BRRCALL vcstrcompare(const char *const cmp, int cse, va_list lptr) {
	char *a = NULL;
	int idx = 0;
	int match = 0;
	if (!cmp)
		return -1;
	if (cse) {
		while (1) {
			a = va_arg(lptr, char *);
			if (!a || !*a)
				break;
#if defined(BRRPLATFORMTYPE_WINDOWS)
			if ((idx = _stricmp(cmp, a)) == 0)
#elif defined(BRRPLATFORMTYPE_UNIX)
			if ((idx = strcasecmp(cmp, a)) == 0)
#else
#error How get strcasecmp?
#endif
			{
				match = 1;
				break;
			}
			++idx;
		}
	} else {
		while (1) {
			a = va_arg(lptr, char *);
			if (!a || !*a)
				break;
			if ((idx = strcmp(cmp, a)) == 0) {
				match = 1;
				break;
			}
			++idx;
		}
	}
	return match?idx:-1;
}

int BRRCALL
brrstr_cstr_compare(const char *const cmp, int cse, ...)
{
	va_list lptr;
	int idx = 0;
	va_start(lptr, cse);
	idx = vcstrcompare(cmp, cse, lptr);
	va_end(lptr);
	return idx;
}

int BRRCALL
brrstr_str_compare(const brrstrT *const cmp, int cse, ...)
{
	va_list lptr;
	int idx = 0;
	va_start(lptr, cse);
	idx = vcstrcompare(brrstr_cstr(cmp), cse, lptr);
	va_end(lptr);
	return idx;
}

int BRRCALL
brrstr_starts_with(const brrstrT *const str, const char *const prefix, int case_insensitive)
{
	brrsz sl = brrstr_strlen(str), pl;
	if (!sl || !prefix || !prefix[0])
		return 0;
	pl = brrstr_cstrlen(prefix, sl + 1);
	if (pl > sl)
		return 0;
	if (case_insensitive) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		return 0 == _strnicmp(brrstr_cstr(str), prefix, pl);
#else
		return 0 == strncasecmp(brrstr_cstr(str), prefix, pl);
#endif
	} else {
		return 0 == strncmp(brrstr_cstr(str), prefix, pl);
	}
}

int BRRCALL
brrstr_ends_with(const brrstrT *const str, const char *const suffix, int case_insensitive)
{
	brrsz sl = brrstr_strlen(str), pl;
	if (!sl || !suffix || !suffix[0])
		return 0;
	pl = brrstr_cstrlen(suffix, sl + 1);
	if (pl > sl)
		return 0;
	if (case_insensitive) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		return 0 == _strnicmp(brrstr_cstr(str) + sl - pl, suffix, pl);
#else
		return 0 == strncasecmp(brrstr_cstr(str) + sl - pl, suffix, pl);
#endif
	} else {
		return 0 == strncmp(brrstr_cstr(str) + sl - pl, suffix, pl);
	}
}
