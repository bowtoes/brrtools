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

#include "brrtools/brrplatform.h"
#if defined(BRRPLATFORMTYPE_UNIX)
# include <strings.h>
#endif

#include "brrtools/brrlib.h"
#include "brrtools/brrlog.h"
#include "brrtools/brrmem.h"
#include "brrtools/brrstg.h"

#include "brrtools/noinstall/statics.h"

brrsz BRRCALL
brrstg_strlen(const char *const string, brrsz max_length)
{
	brrsz ln = 0;
	if (string && string[0])
		for (;ln<max_length&&string[ln];++ln);
	return ln;
}

int BRRCALL
brrstg_new(brrstgT *const string, const char *const cstr, brrsz max_length)
{
	if (!string)
		return 0;
	string->length = brrstg_strlen(cstr, max_length);
	if (string->opaque) {
		if (!brrsizestr(string, string->length)) {
			string->length = 0;
			return -1;
		}
	} else if (!(string->opaque = malloc(string->length + 1))) {
		string->length = 0;
		return -1;
	} else {
		((char *)string->opaque)[string->length] = 0;
	}
	if (string->length)
		memcpy(string->opaque, cstr, string->length);
	return 0;
}
void BRRCALL
brrstg_delete(brrstgT *const string)
{
	if (string) {
		if (string->opaque) {
			free(string->opaque);
			string->opaque = NULL;
		}
		string->length = 0;
	}
}
int BRRCALL
brrstg_string(const brrstgT *const string, char *const destination)
{
	if (!string || !destination) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else {
		memcpy(destination, string->opaque, string->length+1);
	}
	return 0;
}
const char *BRRCALL
brrstg_raw(const brrstgT *const string)
{
	if (string)
		return (const char *)string->opaque;
	return NULL;
}
int BRRCALL
brrstg_resize(brrstgT *const string, brrsz new_length, char fill)
{
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (string->length == new_length) {
		return 0;
	} else {
		brrsz oldsz = string->length;
		if (!brrsizestr(string, new_length))
			return -1;
		if (new_length > oldsz)
			memset((char *)string->opaque + oldsz, fill?fill:' ', new_length - oldsz);
	}
	return 0;
}

static int BRRCALL
int_print(brrstgT *const string, brrsz max_length, brrsz offset, brrsz *const written,
    const char *const fmt, va_list lptr)
{
	brrsz wt = 0, oldsz = 0;
	/* Fill to offset */
	if (offset > string->length) {
		if (!brrsizestr(string, offset))
			return -1;
		memset((char *)string->opaque + string->length, ' ', offset - string->length);
	}

	if (!max_length || !fmt)
		return 0;
	oldsz = string->length;
	/* Resize to fit maximum new bytes */
	if (offset + max_length > string->length) {
		if (!brrsizestr(string, offset + max_length))
			return -1;
	}
	/* Do print */
	wt = vsnprintf((char *)string->opaque + offset, max_length + 1, fmt, lptr);
	/* Shrink if we had to resize to accommodate 'max_length'. */
	if (offset + max_length > oldsz) {
		if (!brrsizestr(string, offset + wt))
			return -1;
	}
	if (written)
		*written += wt;
	return 0;
}
int BRRCALL
brrstg_vprint(brrstgT *const string, brrsz max_length, brrsz offset,
    brrsz *const written, const char *const fmt, va_list lptr)
{
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else {
		return int_print(string, max_length, offset, written, fmt, lptr);
	}
}
int BRRCALL
brrstg_print(brrstgT *const string, brrsz max_length, brrsz offset,
    brrsz *const written, const char *const fmt, ...)
{
	va_list lptr;
	int i = 0;
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else {
		va_start(lptr, fmt);
		i =  int_print(string, max_length, offset, written, fmt, lptr);
		va_end(lptr);
		return i;
	}
}

int BRRCALL
brrstg_copy(brrstgT *const string, const brrstgT *const source)
{
	if (!string || !source) {
		return 0;
	} else if (!string->opaque || !source->opaque) {
		return 1;
	} else if (!brrsizestr(string, source->length)) {
		return -1;
	}
	memcpy(string->opaque, source->opaque, source->length + 1);
	return 0;
}

static int BRRCALL
int_append(brrstgT *restrict const str, const brrstgT *restrict const sfx)
{
	if (!brrsizestr(str, str->length + sfx->length))
		return 0;
	memcpy((char *)str->opaque + str->length - sfx->length, sfx->opaque, sfx->length);
	return 1;
}
static int BRRCALL
int_prepend(brrstgT *restrict const str, const brrstgT *const pfx)
{
	brrsz os = str->length;
	if (!brrsizestr(str, str->length + pfx->length))
		return 0;
	memmove((char *)str->opaque + pfx->length, str->opaque, os);
	memcpy(str->opaque, pfx->opaque, pfx->length);
	return 1;
}
int BRRCALL
brrstg_merge(brrstgT *restrict const destination,
    const brrstgT *restrict const prefix, const brrstgT *restrict const suffix)
{
	if (!destination) {
		return 0;
	} else if (!destination->opaque) {
		return 1;
	} else if ((prefix && !prefix->opaque) || (suffix && !suffix->opaque)) {
		return 1;
	} else if (!brrsizestr(destination, 0)) {
		return -1;
	}
	if (prefix && prefix->length && !int_append(destination, prefix))
		return -1;
	if (suffix && suffix->length && !int_append(destination, suffix))
		return -1;
	return 0;
}
int BRRCALL
brrstg_append(brrstgT *restrict const string, const brrstgT *restrict const suffix)
{
	if (!string || !suffix) {
		return 0;
	} else if (!string->opaque || !suffix->opaque) {
		return 1;
	} else if (suffix->length) {
		if (!int_append(string, suffix))
			return -1;
	}
	return 0;
}
int BRRCALL
brrstg_prepend(brrstgT *restrict const string, const brrstgT *restrict const prefix)
{
	if (!string || !prefix) {
		return 0;
	} else if (!string->opaque || !prefix->opaque) {
		return 1;
	} else if (prefix->length) {
		if (!int_prepend(string, prefix))
			return -1;
	}
	return 0;
}

int BRRCALL
brrstg_strip(brrstgT *const string, int (*filter)(int chr))
{
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (string->length) {
		brrsz start = 0, end = string->length;
		char *ch = (char *)string->opaque;
		if (!filter)
			filter = isspace;
		for (;end > 0 && filter(ch[end-1]); --end) {}
		for (;start < end && filter(ch[start]); ++start) {}
		{
			brrsz nl = end - start;
			memmove(string->opaque, (char *)string->opaque + start, nl);
			if (!brrsizestr(string, nl))
				return -1;
		}
	}
	return 0;
}
int BRRCALL
brrstg_filter(brrstgT *const string, int (*filter)(char chr, brrsz position, brrsz length))
{
	if (!string || !filter) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (string->length) {
		char *ch = (char *)string->opaque;
		brrsz nl = 0;
		for (brrsz i = 0; i < string->length; ++i) {
			if (filter(ch[i], i, string->length))
				ch[nl++] = ch[i];
		}
		if (!brrsizestr(string, nl))
			return -1;
	}
	return 0;
}
int BRRCALL
brrstg_range(brrstgT *restrict const string, const brrstgT *restrict const source,
    brrsz start, brrsz end, int reverse, int wrap)
{
	const char *chi;
	char *cho;
	brrsz nl;
	if (!string || !source) {
		return 0;
	} else if (!string->opaque || !source->opaque) {
		return 1;
	} else if (!source->length) {
		return 0;
	}
	if (start > source->length) {
		if (wrap)
			start = start % (source->length + 1);
		else
			start = source->length;
	}
	if (end > source->length) {
		if (wrap)
			end = end % (source->length + 1);
		else
			end = source->length;
	}
	if (start > end) {
		brrsz t = start;
		start = end;
		end = t;
	}
	nl = end - start;
	if (!brrsizestr(string, nl))
		return -1;
	chi = (const char *)source->opaque;
	cho = (char *)string->opaque;
	if (reverse) {
		for (brrsz i = 0; i < nl; ++i) {
			cho[nl - 1 - i] = chi[start + i];
		}
	} else {
		for (brrsz i = 0; i < nl; ++i) {
			cho[i] = chi[start + i];
		}
	}
	return 0;
}

static int BRRCALL
int_join(brrstgT *restrict const str, const brrstgT *restrict const sep,
    const brrstgT *restrict const st, int skp, int i)
{
	if (!st->opaque) {
		return 1;
	} else if (!skp || st->length) {
		if (sep && sep->length && i) {
			if (!int_append(str, sep))
				return -1;
		}
		if (st->length) {
			if (!int_append(str, st))
				return -1;
		}
	}
	return 0;
}
static int BRRCALL
int_vjoin(brrstgT *restrict const string, const brrstgT *restrict const separator,
    int skip_empty, brrsz *count, va_list lptr)
{
	int err = 0;
	brrsz ct = 0;
	for (const brrstgT *st = NULL; (st = va_arg(lptr, const brrstgT *));ct++) {
		if (!(err = int_join(string, separator, st, skip_empty, ct)))
			break;
	}
	if (!err && count)
		*count = ct;
	return err;
}
int BRRCALL
brrstg_ajoin(brrstgT *restrict const string, const brrstgT *restrict const separator,
	int skip_empty, brrsz count, const brrstgT *restrict const strings)
{
	int err = 0;
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (separator && !separator->opaque) {
		return 1;
	} else if (!strings || count == 0) {
		return brrsizestr(string, 0) ? 0 : -1;
	} else {
		for (brrsz i = 0; i < count; ++i) {
			const brrstgT *const st = &strings[i];
			if ((err = int_join(string, separator, st, skip_empty, i)))
				break;
		}
	}
	return err;
}
int BRRCALL
brrstg_vjoin(brrstgT *restrict const string, const brrstgT *restrict const separator,
    int skip_empty, brrsz *count, va_list lptr)
{
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (separator && !separator->opaque) {
		return 1;
	} else {
		return int_vjoin(string, separator, skip_empty, count, lptr);
	}
}
int BRRCALL
brrstg_join(brrstgT *restrict const string, const brrstgT *restrict const separator,
	int skip_empty, brrsz *count, ...)
{
	if (!string) {
		return 0;
	} else if (!string->opaque) {
		return 1;
	} else if (separator && !separator->opaque) {
		return 1;
	} else {
		va_list lptr;
		int e;
		va_start(lptr, count);
		e = int_vjoin(string, separator, skip_empty, count, lptr);
		va_end(lptr);
		return e;
	}
}

static void BRRCALL
int_delstrs(brrstgT **const strings, brrsz *count)
{
	for (brrsz i = 0; i < *count; ++i) {
		brrstg_delete(&(*strings)[i]);
	}
	free(*strings);
	*strings = NULL;
	*count = 0;
}
static int BRRCALL
int_addstr(const brrstgT *const string, brrsz last, brrsz offset, brrsz max_split,
    brrstgT **const strings, brrsz *count)
{
	int e;
	brrstgT *t;
	if (!(t = realloc(*strings, (*count + 1) * sizeof(brrstgT)))) {
		return -1;
	}
	*strings = t;
	(*strings)[*count].opaque = NULL;
	if ((e = brrstg_new(&(*strings)[(*count)++], (char *)string->opaque+last,
	    max_split > offset - last ? offset - last : max_split))) {
		int_delstrs(strings, count);
	}
	return e;
}
int BRRCALL
brrstg_split(const brrstgT *restrict const string, const brrstgT *restrict const split,
    int skip_consecutive, int keep_split, brrsz offset, brrsz max_split, brrstgT **const strings, brrsz *count)
{
	brrsz ct = 0;
	int e = 0;
	if (!string || !split) {
		*strings = NULL;
		if (count)
			*count = 0;
		return 0;
	} else if (!string->opaque || !split->opaque) {
		*strings = NULL;
		if (count)
			*count = 0;
		return 1;
	} else if (!string->length || !split->length || split->length > string->length) {
		e = int_addstr(string, 0, string->length, max_split, strings, &ct);
	} else {
		brrsz last = 0;
		for (;string->length > (offset = brrmem_search(string->opaque, string->length, split->opaque, split->length, offset));) {
			if (offset > last || !skip_consecutive) {
				if (0 != (e = int_addstr(string, last, offset, max_split, strings, &ct)))
					break;
				if (keep_split) {
					if (0 != (e = int_addstr(split, 0, split->length, max_split, strings, &ct)))
						break;
				}
			}
			offset += split->length;
			last = offset;
		}
		/* why must it always be repeated */
		if (offset > last || !skip_consecutive) {
			if (0 == (e = int_addstr(string, last, offset, max_split, strings, &ct))) {
				/* Only add last if it ends in split */
				if (keep_split && offset - last == split->length) {
					e = int_addstr(split, 0, split->length, max_split, strings, &ct);
				}
			}
		}
		offset += split->length;
		last = offset;
		if (!e && !ct) {
			/* 'split' was not found, so should just be copy of input */
			e = int_addstr(string, 0, string->length, max_split, strings, &ct);
		}
	}
	if (count)
		*count = ct;
	return e;
}
