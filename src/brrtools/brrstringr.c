/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */
#include "brrtools/brrstringr.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if brrplat_unix
# include <strings.h>
#elif brrplat_dos
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

typedef int (*i_str_ncmp_t)(const char *const, const char *const, brrsz);
typedef int (*i_str_cmp_t)(const char *const, const char *const);

extern inline brrstringr_t brrstringr_shallow(const char *const chars, brrsz length);
extern inline brrstringr_t brrstringr_cast(const char *const chars);

brrsz BRRCALL
brrstringr_length(const char *const chars, brrsz max_length)
{
	if (!chars)
		return 0;

	brrsz l = 0;
	const char *c = chars;
	while (++l < max_length && *(c++));
	return l - 1;
}

int BRRCALL
brrstringr_new(brrstringr_t *const string, const char *const chars, brrsz length)
{
	if (!string)
		return -1;

	brrstringr_t out = {.length = length};
	if (!(out.cstr = calloc(1, 1 + length)))
		return -1;
	if (chars)
		memcpy(out.cstr, chars, length);

	*string = out;
	return 0;
}
void BRRCALL
brrstringr_free(brrstringr_t *const string)
{
	if (!string || string->shallow)
		return;

	if (string->cstr && !string->shallow)
		free(string->cstr);
	memset(string, 0, sizeof(*string));
}

int BRRCALL
brrstringr_clear(brrstringr_t *const string)
{
	if (!string || string->shallow)
		return -1;

	char *t = realloc(string->cstr, 1);
	if (!t)
		return -1;
	string->cstr = t;
	string->cstr[0] = 0;
	string->length = 0;
	return 0;
}

int BRRCALL
brrstringr_shrink_right(brrstringr_t *const string, brrsz new_len)
{
	if (!string)
		return -1;
	if (string->length <= new_len)
		return 0;
	char *t = realloc(string->cstr, new_len + 1);
	if (!t)
		return -1;
	string->cstr = t;
	string->cstr[new_len] = 0;
	string->length = new_len;
	return 0;
}

int BRRCALL
brrstringr_copy(brrstringr_t *const string, const brrstringr_t *const source)
{
	if (!string || !source)
		return -1;

	/* String is shallow, so it's safe to forget the current pointer */
	if (string->shallow)
		return brrstringr_new(string, source->cstr, source->length);

	char *t = realloc(string->cstr, source->length + 1);
	if (!t)
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
	if (!string->length)
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
			char *new = realloc(string->cstr, nl + 1);
			if (!new)
				return -1;
			new[nl] = 0;
			string->cstr = new;
			string->length = nl;
		}
	}
	return 0;
}

static inline brrsz BRRCALL
i_vprint(
    brrstringr_t *const string,
    brrsz offset,
    brrsz max_length,
    const char *const format,
    va_list lptr
)
{
	brrsz wrote = 0;
	{
		va_list lptr2;
		va_copy(lptr2, lptr);
		wrote = vsnprintf(NULL, 0, format, lptr2); // consumes lptr2
		va_end(lptr2);
	}

	if (wrote > max_length)
		wrote = max_length;

	brrstringr_t s = {.length = offset + wrote};
	if (s.length <= string->length) {
		// New string is entirely inside old string, no need for allocation
		s.cstr = string->cstr ? string->cstr : malloc(s.length + 1);
	} else {
		// New string lengthens old string, allocate
		s.cstr = string->cstr ? realloc(string->cstr, 1 + s.length) : malloc(s.length + 1);
	}

	if (!s.cstr)
		return BRRSZ_MAX;

	// New string starts past old string, fill the gap with spaces
	if (offset > string->length)
		memset(s.cstr + string->length, ' ', s.length - string->length);
	wrote = vsnprintf(s.cstr + offset, wrote + 1, format, lptr);
	*string = s;
	return wrote;
}

brrsz BRRCALL
brrstringr_print(
    brrstringr_t *const string,
    brrsz offset,
    brrsz max_length,
    const char *const format,
    ...
)
{
	if (!string || string->shallow || !format)
		return BRRSZ_MAX;
	if (!max_length)
		return 0;

	brrsz wrote = 0;

	va_list lptr;
	va_start(lptr, format);
	wrote = i_vprint(string, offset, max_length, format, lptr);
	va_end(lptr);
	return wrote;
}

brrsz BRRCALL
brrstringr_vprint(
    brrstringr_t *const string,
    brrsz offset,
    brrsz max_length,
    const char *const format,
    va_list lptr
)
{
	if (!string || string->shallow || !format)
		return BRRSZ_MAX;
	if (!max_length)
		return 0;
	return i_vprint(string, offset, max_length, format, lptr);
}

/* TODO currently quarantined, will fix soon tm */
#if 0
static inline brrsz BRRCALL
i_find_next_delimni(
    const brrstringr_t *const string,
    const brrstringr_t *const delim,
    brrsz current,
    i_str_ncmp_t comparer
)
{
	while (current < string->length && 0 != comparer(string->cstr + current, delim->cstr, delim->length)) {
		if (++current >= string->length)
			return string->length;
	}
	return current;
}
static inline brrsz BRRCALL
i_find_next_delimm(
    const brrstringr_t **delim_idx,
    const brrstringr_t *const string,
    const brrstringr_t *const delims,
    brrsz n_delims,
    brrsz current,
    i_str_ncmp_t comparer
)
{
	brrsz min = string->length;

	const brrstringr_t *dlm = delims;
	for (const brrstringr_t* d = delims; d < &delims[n_delims-1]; ++d) {
		if (d->length && d->length < string->length - current) {
			brrsz j = i_find_next_delimni(string, d, current, comparer);
			if (j < min) {
				min = j;
				dlm = d;
			}
		}
	}

	*delim_idx = dlm;
	return min;
}
static inline int BRRCALL
i_add_next_split(
    brrstringr_t **splits,
    brrsz *n_splits,
    const brrstringr_t *const string,
    const brrstringr_t *const delimeter,
    brrsz max_split,
    brrsz start,
    brrsz end,
    brrbl skip_consecutive
)
{
	brrsz len = end - start;
	if (delimeter)
		len -= delimeter->length - 1;
	if (len > max_split)
		len = max_split;

	if (skip_consecutive && delimeter && len < delimeter->length)
		return 0;

	brrstringr_t str = {0};
	if (brrstringr_new(&str, string->cstr + start, len))
		return -1;
	if (brrarray_append((void **)splits, n_splits, sizeof(str), &str)) {
		brrstringr_free(&str);
		return -1;
	}
	return 0;
}
static inline int BRRCALL
i_split_str(
    brrstringr_t **strings,
    brrsz *const n_strings,
    const brrstringr_t *const string,
    const brrstringr_t *const delims,
    brrsz n_delims,
    brrsz max_split,
    brrbl skip_consecutive,
    brrbl case_sensitive
)
{
	i_str_ncmp_t comparer = case_sensitive?strncmp:strncasecmp;
	brrstringr_t *splits = NULL;
	brrsz end = 0, start = 0;
	brrsz n_splits = 0;

	const brrstringr_t *found_delim;
	while ((end = i_find_next_delimm(&found_delim, string, delims, n_delims, end, comparer)) < string->length) {
		if (i_add_next_split(&splits, &n_splits, string, found_delim, max_split, start, end, skip_consecutive)) {
			brrdata_clear((void **)&splits, &n_splits, sizeof(*splits), (void (*)(void*))brrstringr_free);
			return -1;
		}
		end += found_delim->length;
		start = end;
	}

	if (i_add_next_split(&splits, &n_splits, string, NULL, max_split, start, end, skip_consecutive)) {
		brrdata_clear((void **)&splits, &n_splits, sizeof(*splits), (void (*)(void*))brrstringr_free);
		return -1;
	}
	*n_strings = n_splits;
	*strings = splits;
	return 0;
}
int BRRCALL
brrstringr_split(
    brrstringr_t **const strings,
    brrsz *n_strings,
    const brrstringr_t *const string,
    const brrstringr_t *const delimiter,
    brrsz max_split,
    brrbl skip_consecutive,
    brrbl case_sensitive
)
{
	if (!strings || !n_strings)
		return -1;
	if (!string)
		return 0;

	if (!max_split || max_split > string->length)
		max_split = string->length;

	if (i_split_str(strings, n_strings, string, delimiter, 1, max_split, skip_consecutive, case_sensitive))
		return -1;
	return 0;
}
int BRRCALL
brrstringr_split_multiple(
    brrstringr_t **const strings,
    brrsz *n_strings,
    const brrstringr_t *const string,
    const brrstringr_t *const delimiters,
    brrsz n_delimiters,
    brrsz max_split,
    brrbl skip_consecutive,
    brrbl case_sensitive
)
{
	if (!strings || !n_strings)
		return -1;
	if (!string)
		return 0;
	if (!max_split || max_split > string->length)
		max_split = string->length;
	if (i_split_str(strings, n_strings, string, delimiters, n_delimiters, max_split, skip_consecutive, case_sensitive))
		return -1;
	return 0;
}
int BRRCALL
brrstringr_join(
    brrstringr_t *const string,
    const brrstringr_t *const delimiter,
    const brrstringr_t *const strings,
    brrsz n_strings
)
{
	if (!string || !strings)
		return -1;
	if (!n_strings)
		return 0;

	brrsz total_len = 0;
	for (brrsz i = 0; i < n_strings; ++i)
		total_len += strings[i].length;
	if (delimiter)
		total_len += (n_strings - 1) * delimiter->length;

	brrstringr_t str = {0};
	if (!(str.cstr = calloc(1, total_len + 1)))
		return -1;
	if (delimiter && delimiter->length) {
		const brrstringr_t d = *delimiter;
		for (brrsz i = 0; i < n_strings - 1; ++i) {
			const brrstringr_t s = strings[i];
			memcpy(str.cstr + str.length, s.cstr, s.length);
			str.length += s.length;
			memcpy(str.cstr + str.length, d.cstr, d.length);
			str.length += d.length;
		}
		const brrstringr_t s = strings[n_strings-1];
		memcpy(str.cstr + str.length, s.cstr, s.length);
		str.length += s.length;
	} else {
		for (brrsz i = 0; i < n_strings - 1; ++i) {
			const brrstringr_t s = strings[i];
			memcpy(str.cstr + str.length, s.cstr, s.length);
			str.length += s.length;
		}
	}
	*string = str;
	return 0;
}
#endif

int BRRCALL
brrstringr_filter_chars(brrstringr_t *const string, int (*filter)(int, int), brrbl invert)
{
	if (!string || !filter || string->shallow)
		return -1;
	if (!string->length)
		return 0;

	brrstringr_t original = *string;
	brrstringr_t filtered = {0};
	// note that this loop accesses 'string[length+1]' (the null-terminator) on the last iteration.
	if (!invert) {
		for (brrsz i = 0; i < original.length; ++i) {
			if (!filter(original.cstr[i], original.cstr[i+1]))
				original.cstr[filtered.length++] = original.cstr[i];
		}
	} else {
		for (brrsz i = 0; i < original.length; ++i) {
			if (filter(original.cstr[i], original.cstr[i+1]))
				original.cstr[filtered.length++] = original.cstr[i];
		}
	}
	if (filtered.length == original.length)
		return 0;
	if (!(filtered.cstr = realloc(original.cstr, filtered.length + 1)))
		return -1;
	filtered.cstr[filtered.length] = 0;

	*string = filtered;
	return 0;
}
int BRRCALL
brrstringr_filter(brrstringr_t *const string, int (*filter)(int, int, brrsz, brrsz), brrbl invert)
{
	if (!string || !filter || string->shallow)
		return -1;
	if (!string->length)
		return 0;

	brrstringr_t original = *string;
	brrstringr_t filtered = {0};
	// note that this loop accesses 'string[length+1]' (the null-terminator) on the last iteration.
	if (!invert) {
		for (brrsz i = 0; i < original.length; ++i) {
			if (!filter(original.cstr[i], original.cstr[i+1], i, original.length))
				original.cstr[filtered.length++] = original.cstr[i];
		}
	} else {
		for (brrsz i = 0; i < original.length; ++i) {
			if (filter(original.cstr[i], original.cstr[i+1], i, original.length))
				original.cstr[filtered.length++] = original.cstr[i];
		}
	}
	if (filtered.length == original.length)
		return 0;
	if (!(filtered.cstr = realloc(original.cstr, 1 + filtered.length)))
		return -1;
	filtered.cstr[filtered.length] = 0;

	*string = filtered;
	return 0;
}

int BRRCALL
brrstringr_compare(const brrstringr_t *const string, const brrstringr_t *const other, brrbl case_sensitive)
{
	if (!string || !other || !string->cstr || !other->cstr)
		return 0;
	return case_sensitive ? strcmp(string->cstr, other->cstr) : strcasecmp(string->cstr, other->cstr);
}
int BRRCALL
brrstringr_ncompare(const brrstringr_t *const string, const brrstringr_t *const other, brrbl case_sensitive)
{
	if (!string || !other || !string->cstr || !other->cstr)
		return 0;
	return case_sensitive ?
		strncmp    (string->cstr, other->cstr, string->length < other->length ? string->length : other->length):
		strncasecmp(string->cstr, other->cstr, string->length < other->length ? string->length : other->length);
}

char *BRRCALL
brrstringr_cstr_compare(const char *const chars, int case_sensitive, ...)
{
	if (!chars)
		return NULL;

	char *r = NULL;
	i_str_cmp_t cmp = case_sensitive?strcmp:strcasecmp;
	va_list lptr;
	va_start(lptr, case_sensitive);
	while (1) {
		r = va_arg(lptr, char *);
		if (!r || !cmp(chars, r))
			break;
	}
	va_end(lptr);
	return r;
}
