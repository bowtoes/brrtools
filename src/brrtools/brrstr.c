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

#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "brrtools/brrplatform.h"
#if defined(BRRPLATFORMTYPE_UNIX)
# include <strings.h>
#endif

#include "brrtools/brrlib.h"
#include "brrtools/brrlog.h"
#include "brrtools/brrmem.h"
#include "brrtools/brrstr.h"
#include "brrtools/brrtil.h"

#if defined(BRRPLATFORMTYPE_WINDOWS)
# define STRICMP _stricmp
# define STRNICMP _strnicmp
#elif defined(BRRPLATFORMTYPE_UNIX)
# define STRICMP strcasecmp
# define STRNICMP strncasecmp
#else
#error How get strcasecmp?
#endif

brrstrlen BRRCALL
brrstr_strlen(const char *const cstr, brrstrlen max_strlen)
{
	brrsz s = 0;
	if (cstr && cstr[0])
		for (;s < max_strlen && cstr[s]; ++s);
	return s;
}
brrstrT BRRCALL
brrstr_new(const char *const cstr, brrstrlen max_strlen)
{
	brrstrT str = {0};
	if (cstr) {
		str.length = brrstr_strlen(cstr, max_strlen);
		if (brrlib_alloc((void **)&str.cstr, str.length + 1, 1))
			memcpy(str.cstr, cstr, str.length);
		else
			str.length = 0;
	} else {
		str.length = 0;
		brrlib_alloc((void **)&str.cstr, 1, 1);
	}
	return str;
}
void BRRCALL
brrstr_delete(brrstrT *const str)
{
	if (str) {
		brrlib_alloc((void **)&str->cstr, 0, 0);
		str->length = 0;
	}
}
brrstrT BRRCALL
brrstr_duplicate(const brrstrT str)
{
	return (brrstrT){.cstr = brrmem_duplicate(str.cstr, str.length + 1), .length = str.length};
}
void BRRCALL
brrstr_copy(const brrstrT source, brrstrT *const destination)
{
	if (!destination)
		return;
	if (!brrlib_alloc((void **)&destination->cstr, source.length + 1, 1))
		return;
	memcpy(destination->cstr, source.cstr, source.length);
	destination->length = source.length;
}
void BRRCALL
brrstr_resize(brrstrT *const str, brrstrlen new_strlen)
{
	if (str) {
		if (new_strlen != str->length) {
			if (!brrlib_alloc((void **)&str->cstr, new_strlen + 1, 0))
				return;
			if (new_strlen > str->length)
				memset(str->cstr + str->length, ' ', new_strlen - str->length);
			else
				str->cstr[new_strlen] = 0;
			str->length = new_strlen;
		}
	}
}

void BRRCALL
brrstr_strip(brrstrT *str)
{
	if (str && str->length) {
		brrsz ed = str->length, st = 0;
		for (; ed > 0 && isspace(str->cstr[ed-1]); --ed);
		if (ed == 0) {
			/* Entire string is whitespace */
			str->length = 0;
		} else {
			for (; st < ed && isspace(str->cstr[st]); ++st);
			for (brrsz i = st; i < ed; ++i) str->cstr[i-st] = str->cstr[i];
			str->length = ed - st;
		}
		str->cstr[str->length] = 0;
		brrlib_alloc((void **)&str->cstr, str->length+1, 0);
	}
}
brrstrT BRRCALL
brrstr_trim(const brrstrT str)
{
	brrstrT cp = brrstr_duplicate(str);
	brrstr_strip(&cp);
	return cp;
}

int BRRCALL
brrstr_astr_compare(const char *const str, int case_insensitive, const char *const *const arr)
{
	int idx = 0, match = 0;
	if (!arr)
		return -1;
	if (!str) {
		match = 1;
		while (arr[idx]) { ++idx; }
	} else {
		int (*compare)(const char *, const char *) = case_insensitive?STRICMP:strcmp;
		while (arr[idx]) {
			if (compare(str, arr[idx]) == 0) {
				match = 1;
				break;
			}
			++idx;
		}
	}
	return match?idx:-1;
}
int BRRCALL
brrstr_vstr_compare(const char *const str, int case_insensitive, va_list lptr)
{
	char *a = NULL;
	int idx = 0, match = 0;
	if (!str) {
		match = 1;
		while ((a = va_arg(lptr, char *))) { ++idx; }
	} else {
		int (*compare)(const char *, const char *) = case_insensitive?STRICMP:strcmp;
		while ((a = va_arg(lptr, char *))) {
			if (compare(str, a) == 0) {
				match = 1;
				break;
			}
			++idx;
		}
	}
	return match?idx:-1;
}
int BRRCALL
brrstr_str_compare(const char *const str, int case_insensitive, ...)
{
	va_list lptr;
	int idx = 0;
	va_start(lptr, case_insensitive);
	idx = brrstr_vstr_compare(str, case_insensitive, lptr);
	va_end(lptr);
	return idx;
}
int BRRCALL
brrstr_starts_with(const brrstrT str, const char *const prefix, int case_insensitive)
{
	brrsz pl;
	if (!str.length || !prefix || !prefix[0])
		return 0;
	pl = brrstr_strlen(prefix, str.length + 1);
	if (pl > str.length)
		return 0;
	else if (case_insensitive)
		return 0 == STRNICMP(str.cstr, prefix, pl);
	else
		return 0 == strncmp(str.cstr, prefix, pl);
}
int BRRCALL
brrstr_ends_with(const brrstrT str, const char *const suffix, int case_insensitive)
{
	brrstrlen pl = 0;
	if (!str.length || !suffix || !suffix[0])
		return 0;
	pl = brrstr_strlen(suffix, str.length + 1);
	if (pl > str.length)
		return 0;
	else if (case_insensitive)
		return 0 == STRNICMP(str.cstr + str.length - pl, suffix, pl);
	else
		return 0 == strncmp(str.cstr + str.length - pl, suffix, pl);
}

static brrstrT *BRRCALL
addsplit(const char *const src, brrstrT *strs, brrsz *count, brrsz start,
	brrsz end_inclusive, brrsz maxlen, int *err)
{
	if (!brrlib_alloc((void **)&strs, (*count+1)*sizeof(brrstrT), 0)) {
		for (brrct i = 0; i < *count; ++i)
			brrlib_alloc((void **)&(strs[i].cstr), 0, 0);
		brrlib_alloc((void **)strs, 0, 0);
		*count = 0;
		if (err)
			*err = 1;
		return NULL;
	}
	end_inclusive -= start;
	maxlen = BRRTIL_MIN(end_inclusive, maxlen);
	strs[(*count)++] = brrstr_new(src + start, maxlen);
	return strs;
}
static brrstrT *BRRCALL
split(const char *const src, brrstrT *strs, char sep, int cns,
    brrsz *count, brrsz *offset, brrsz *last, brrsz maxlen, int *err)
{
	if (*offset > *last /* || cns */) {
		strs = addsplit(src, strs, count, *last, *offset, maxlen, err);
	}
	*last = ++(*offset);
	return strs;
}
brrstrT *BRRCALL
brrstr_split(const brrstrT str, char separator, brrsz offset,
    brrsz max_split_size, brrsz *const count)
{
	brrstrT *strs = NULL;
	brrsz cnt = 0;
	brrsz last = offset;
	int err = 0;
	if (str.length) {
		while (!err && str.length > (offset = brrmem_next(str.cstr, str.length, separator, offset))) {
			strs = split(str.cstr, strs, separator, 0,
			    &cnt, &offset, &last, max_split_size, &err);
		}
		if (!err && offset > last) {
			/* If 'sep' is not found, adds the whole string from 'offset' to end.
			 * If 'sep' was found (so 'offset' was updated), adds that last piece.
			 * Otherwise, it would be ignored. */
			strs = split(str.cstr, strs, separator, 0,
			    &cnt, &offset, &last, max_split_size, &err);
		}

	}
	if (!cnt && !err) {
		/* The string was nothing but a series of 'sep', or was empty. */
		strs = addsplit("", NULL, &cnt, 0, 0, 0, NULL);
	}
	if (count)
		*count = cnt;
	return strs;
}
