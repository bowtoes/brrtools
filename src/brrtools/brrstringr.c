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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#else
# include <strings.h>
#endif

#include "brrtools/brrlib.h"
#include "brrtools/brrstringr.h"
#include "brrtools/brrheap.h"

typedef int (*i_str_ncmp_t)(const char *const, const char *const b, brrsz);
typedef int (*i_str_cmp_t)(const char *const, const char *const b);

extern inline brrstringr_t brrstringr_shallow(const char *const cstr, brrsz length);
extern inline brrstringr_t brrstringr_cast(const char *const cstr);

brrsz BRRCALL
brrstringr_length(const char *const cstr, brrsz max_length)
{
	brrsz l = 0;
	if (!cstr)
		return 0;
	for (;cstr[l]&&l<max_length;++l);
	return l;
}

int BRRCALL
brrstringr_new(brrstringr_t *const string, const char *const cstr, brrsz length)
{
	brrstringr_t out = {.length = length};
	if (!string)
		return -1;
	if (!(out.cstr = calloc(1 + length, 1)))
		return -1;
	if (cstr)
		memcpy(out.cstr, cstr, length);
	*string = out;
	return 0;
}
void BRRCALL
brrstringr_free(brrstringr_t *const string)
{
	if (string) {
		if (string->cstr && !string->shallow)
			free(string->cstr);
		memset(string, 0, sizeof(*string));
	}
}

int BRRCALL
brrstringr_clear(brrstringr_t *const string)
{
	char *t = NULL;
	if (!string)
		return -1;
	if (!(t = realloc(string->cstr, 1)))
		return -1;
	string->cstr = t;
	string->cstr[0] = 0;
	string->length = 0;
	return 0;
}

int BRRCALL
brrstringr_copy(brrstringr_t *const string, const brrstringr_t *const source)
{
	char *t = NULL;
	if (!string || !source)
		return -1;
	else if (string->shallow)
		return brrstringr_new(string, source->cstr, source->length);
	if (!(t = realloc(string->cstr, source->length + 1)))
		return -1;
	memcpy(t, source->cstr, source->length);
	t[source->length] = 0;
	string->cstr = t;
	string->length = source->length;
	return 0;
}
int BRRCALL
brrstringr_trim_whitespace(brrstringr_t *const string, brrbl leading, brrbl tailing)
{
	if (!string || string->shallow)
		return -1;
	else if (!string->length)
		return 0;
	brrsz s = 0, e = string->length;
	if (tailing) {
		for (;e > 0 && isspace(string->cstr[e-1]); --e);
		if (!leading && e != string->length) {
			char *t = NULL;
			if (!(t = realloc(string->cstr, e)))
				return -1;
			string->cstr = t;
			string->length = e - 1;
			string->cstr[e] = 0;
		}
	}
	if (leading) {
		for(;s < e && isspace(string->cstr[s]); ++s);
		if (s != 0) {
			brrsz nl = e - s;
			char *t = NULL;
			/* 'calloc' instead of 'memmove' and 'realloc' to leave 'string'
			 * unaffected in case of error */
			if (!(t = calloc(1, nl + 1)))
				return -1;
			memcpy(t, string->cstr + s, nl);
			free(string->cstr);
			string->cstr = t;
			string->length = nl;
		}
	}
	return 0;
}

static brrsz BRRCALL
i_vprint(brrstringr_t *const string, brrsz offset, brrsz max_length,
    const char *const format, va_list lptr)
{
	va_list lptr2;
	brrstringr_t s = {0};
	brrsz wrote = 0;
	va_copy(lptr2, lptr);
	wrote = vsnprintf(NULL, 0, format, lptr2);
	va_end(lptr2);
	if (wrote > max_length)
		wrote = max_length;
	s.length = offset + wrote;
	if (s.length <= string->length) {
		/* New string is entirely inside old string, no need for allocation */
		s.cstr = string->cstr;
	} else {
		/* New string lengthens old string, allocate */
		if (!string->cstr)
			s.cstr = malloc(1 + s.length);
		else
			s.cstr = realloc(string->cstr, 1 + s.length);
		if (!s.cstr)
			return BRRSZ_MAX;
		/* New string starts past old string, figll the gap with spaces */
		if (offset > string->length)
			memset(s.cstr + string->length, ' ', s.length - string->length);
	}
	if (!s.cstr)
		return BRRSZ_MAX;
	/* Consume 'lptr' */
	wrote = vsnprintf(s.cstr + offset, wrote + 1, format, lptr);
	*string = s;
	return wrote;
}
brrsz BRRCALL
brrstringr_print(brrstringr_t *const string, brrsz offset, brrsz max_length,
    const char *const format, ...)
{
	va_list lptr;
	brrsz wrote = 0;
	if (!string || string->shallow || !format)
		return BRRSZ_MAX;
	if (!max_length)
		return 0;
	va_start(lptr, format);
	wrote = i_vprint(string, offset, max_length, format, lptr);
	va_end(lptr);
	return wrote;
}
brrsz BRRCALL
brrstringr_vprint(brrstringr_t *const string, brrsz offset, brrsz max_length,
    const char *const format, va_list lptr)
{
	if (!string || string->shallow || !format)
		return BRRSZ_MAX;
	if (!max_length)
		return 0;
	return i_vprint(string, offset, max_length, format, lptr);
}

static brrsz BRRCALL
i_find_next_delimni(const brrstringr_t *const string,
	brrsz current, const brrstringr_t *const delim,
	i_str_ncmp_t comparer)
{
	while (current < string->length && 0 != comparer(string->cstr + current, delim->cstr, delim->length)) {
		if (current+1 > string->length)
			return string->length;
		current++;
	}
	return current;
}
static brrsz BRRCALL
i_find_next_delimm(const brrstringr_t *const string, brrsz current,
    const brrstringr_t *const delims, brrsz n_delims, brrsz *delim,
    i_str_ncmp_t comparer)
{
	brrsz dlm = 0;
	brrsz min = string->length;
	for (brrsz i = 0; i < n_delims; ++i) {
		const brrstringr_t *const d = &delims[i];
		brrsz j = 0;
		if (d->length && d->length < string->length - current) {
			if ((j = i_find_next_delimni(string, current, d, comparer)) < string->length) {
				if (j < min) {
					min = j;
					dlm = i;
				}
			}
		}
	}
	*delim = dlm;
	return string->length;
}
static int BRRCALL
i_add_next_str(brrstringr_t **array, brrsz *n,
    brrbl skip_consecutive,
    const brrstringr_t *const string, brrsz max_split,
    const brrstringr_t *const prev_delim, const brrstringr_t *const cur_delim,
    brrsz previous, brrsz current)
{
	brrstringr_t str = {0};
	brrsz len = current - previous - cur_delim->length;
	len = max_split>len?len:max_split;
	if (skip_consecutive && len <= cur_delim->length)
		return 0;
	else if (brrstringr_new(&str, string->cstr + previous, len))
		return -1;
	else if (brrheap_append((void **)array, n, sizeof(str), &str))
		return -1;
	return 0;
}
static int BRRCALL
i_split_str(brrstringr_t **strings, brrsz *const n_strings,
    brrbl skip_consecutive, brrbl case_sensitive,
    const brrstringr_t *const string, brrsz max_split,
    const brrstringr_t *const delims, brrsz n_delims)
{
	i_str_ncmp_t comparer = case_sensitive?strncmp:strncasecmp;
	brrstringr_t *array = NULL;
	brrsz current = 0, previous = 0;
	brrsz n = 0, j = 0, pj = 0;
	while ((current = i_find_next_delimm(string, current, delims, n_delims, &j, comparer)) < string->length) {
		if (i_add_next_str(&array, &n, skip_consecutive, string, max_split,
			    &delims[pj], &delims[j], previous, current)) {
			brrheap_clear((void **)&array, &n, sizeof(*array), (void (*)(void*))brrstringr_free);
			return -1;
		}
		pj = j;
		previous = current;
	}
	if (i_add_next_str(&array, &n, skip_consecutive, string, max_split,
		    &delims[pj], &delims[j], previous, current)) {
		brrheap_clear((void **)&array, &n, sizeof(*array), (void (*)(void*))brrstringr_free);
		return -1;
	}
	*n_strings = n;
	*strings = array;
	return 0;
}
int BRRCALL
brrstringr_split(brrstringr_t **const strings, brrsz *n_strings,
    const brrstringr_t *const delimiter, brrbl skip_consecutive, brrbl case_sensitive,
    const brrstringr_t *const string, brrsz max_split)
{
	if (!strings || !n_strings)
		return -1;
	if (string) {
		if (!max_split || max_split > string->length)
			max_split = string->length;
		if (i_split_str(strings, n_strings,
			    skip_consecutive, case_sensitive,
			    string, max_split,
			    delimiter, 1))
			return -1;
	}
	return 0;
}
int BRRCALL
brrstringr_split_multiple(brrstringr_t **const strings, brrsz *n_strings,
    const brrstringr_t *const delimiters, brrsz n_delimiters,
    brrbl skip_consecutive, brrbl case_insensitive,
    const brrstringr_t *const string, brrsz max_split)
{
	if (!strings || !n_strings) {
		return -1;
	} else if (string) {
		if (!max_split || max_split > string->length)
			max_split = string->length;
		if (i_split_str(strings, n_strings,
			    skip_consecutive, case_insensitive,
				string, max_split,
				delimiters, n_delimiters))
			return -1;
	}
	return 0;
}
int BRRCALL
brrstringr_join(brrstringr_t *const string,
    const brrstringr_t *const strings, brrsz n_strings,
    const brrstringr_t *const delimiter)
{
	brrstringr_t str = {0};
	brrsz total_len = 0;
	if (!string || !strings)
		return -1;
	if (n_strings) {
		for (brrsz i = 0; i < n_strings; ++i)
			total_len += strings[i].length;
		if (delimiter)
			total_len += (n_strings - 1) * delimiter->length;
		if (!(str.cstr = calloc(1, total_len + 1)))
			return -1;
		if (delimiter && delimiter->length) {
			for (brrsz i = 0; i < n_strings - 1; ++i)
				str.length += snprintf(str.cstr + str.length, total_len + 1, "%s%s", strings[i].cstr, delimiter->cstr);
			str.length += snprintf(str.cstr + str.length, total_len + 1, "%s", strings[n_strings - 1].cstr);
		} else {
			for (brrsz i = 0; i < n_strings; ++i)
				str.length += snprintf(str.cstr + str.length, total_len + 1, "%s", strings[i].cstr);
		}
	}
	*string = str;
	return 0;
}

int BRRCALL
brrstringr_filter_chars(brrstringr_t *const string, int (*filter)(int, int))
{
	if (!string || !filter || string->shallow)
		return -1;
	if (!string->length)
		return 0;
	brrstringr_t s = {0};
	for (brrsz i = 0; i < string->length; ++i) {
		if (!filter(string->cstr[i], string->cstr[i+1]))
			string->cstr[s.length++] = string->cstr[i];
	}
	if (s.length == string->length)
		return 0;
	if (!(s.cstr = realloc(string->cstr, 1 + s.length)))
		return -1;
	*string = s;
	string->cstr[s.length] = 0;
	return 0;
}
int BRRCALL
brrstringr_filter(brrstringr_t *const string, int (*filter)(int, int, brrsz, brrsz))
{
	if (!string || !filter || string->shallow)
		return -1;
	if (!string->length)
		return 0;
	brrstringr_t s = {0};
	for (brrsz i = 0; i < string->length; ++i) {
		if (!filter(string->cstr[i], string->cstr[i+1], i, string->length))
			string->cstr[s.length++] = string->cstr[i];
	}
	if (s.length == string->length)
		return 0;
	if (!(s.cstr = realloc(string->cstr, 1 + s.length)))
		return -1;
	*string = s;
	string->cstr[s.length] = 0;
	return 0;
}

int BRRCALL
brrstringr_compare(const brrstringr_t *const string,
    const brrstringr_t *const other, brrbl case_sensitive)
{
	if (!string || !other || !string->cstr || !other->cstr)
		return 0;
	return case_sensitive?strcmp(string->cstr, other->cstr):strcasecmp(string->cstr, other->cstr);
}
int BRRCALL
brrstringr_ncompare(const brrstringr_t *const string,
    const brrstringr_t *const other, brrbl case_sensitive)
{
	if (!string || !other || !string->cstr || !other->cstr)
		return 0;
	return case_sensitive?
		strncmp(string->cstr, other->cstr, string->length<other->length?string->length:other->length):
		strncasecmp(string->cstr, other->cstr, string->length<other->length?string->length:other->length);
}

char *BRRCALL
brrstringr_cstr_compare(const char *const string, int case_sensitive, ...)
{
	char *r = NULL;
	va_list lptr;
	i_str_cmp_t cmp = case_sensitive?strcmp:strcasecmp;
	if (!string)
		return NULL;
	va_start(lptr, case_sensitive);
	while ((r = va_arg(lptr, char *))) {
		if (0 == cmp(string, r))
			break;
	}
	va_end(lptr);
	return r;
}
