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

#ifndef BRRSTG_H
#define BRRSTG_H

/*
    TODO Microsoft 'wchar_t' on Windows, for proper unicode support there.
	Why Microsoft you make things difficult.
*/

#include <stdarg.h>

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* String representation that stores length of string.
 * Fields:
 * - void *opaque : Pointer storing the 'cstr' represented.
 * - brrsz length : Length of the c-string, excluding NULL terminator.
 * NULL 'opaque' indicates an allocation error and any function that gets passed
 * a string with a NULL 'opaque' will fail.
 * */
typedef struct brrstg {
	void *opaque; /* Opaque pointer storing the 'cstr' represented. */
	brrsz length; /* Length of the c-string, excluding NULL terminator. */
} brrstgT;

#define BRRSTG_ASSTG(_c_) ({.opaque=_c_, .length=_c_?sizeof(_c_)-1:0})

/* Return length of 'string' excluding null-terminator, up to 'max_length'. */
BRRAPI brrsz BRRCALL brrstg_strlen(const char *const string, brrsz max_length);

/* Initialize a new string; can reinitialize a pre-existing/NULL string and returns 0.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'cstr' is NULL or has length 0 or 'max_length' is 0, 'string' will be the
 * empty string and 0 is returned.
 * If an error occurs allocating space for the string, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_new(brrstgT *const string, const char *const cstr, brrsz max_length);
/* Frees memory associated with 'string' and sets it's length to 0.
 * If 'string' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrstg_delete(brrstgT *const string);
/* Copies the cstring from 'string' into destination and returns 0.
 * If 'string' or 'destination' is NULL, nothing is done and 0 is returned.
 * If 'string' is a NULL string, nothing is done and 1 is returned.
 * */
BRRAPI int BRRCALL brrstg_string(const brrstgT *const string, char *const destination);
/* Returns the raw string representation stored in 'string'; does not error.
 * If 'string' is NULL, NULL is returned.
 * */
BRRAPI const char *BRRCALL brrstg_raw(const brrstgT *const string);
/* Resizes 'string' to be 'new_length' bytes long and returns 0.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'new_length' == 'string->length', nothing is done and 0 is returned.
 * If 'new_length' > 'string->length' and 'fill' is non-zero, the new space will
 * be filled with 'fill'. If 'fill' is 0, then the space will be filled with spaces.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_resize(brrstgT *const string, brrsz new_length, char fill);

/* Performs printf-style printing into 'string' at the given 'offset', printing
 * at most 'max_length' bytes and returns 0.
 * Number of bytes written is added to 'written'.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'string' is a NULL string, nothing is done and 1 is returned.
 * If 'offset' is greater than the length of 'string', 'string' will be resized
 * to fit and the gap will be filled with spaces; this resize does not add to
 * number of bytes written.
 * If 'max_length' is 0 or 'fmt' is NULL, nothing else is done and 0 is returned.
 *
 * If printing succeeds, 0 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_vprint(brrstgT *const string, brrsz max_length, brrsz offset,
    brrsz *const written, const char *const fmt, va_list lptr);
/* Performs printf-style printing into 'string' at the given 'offset', printing
 * at most 'max_length' bytes, including the NULL terminator.
 * Number of bytes written is added to 'written'.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'string' is a NULL string, nothing is done and 1 is returned.
 * If 'offset' is greater than the length of 'string', 'string' is resized
 * to fit and the gap is be filled with spaces; this resize does not count towards
 * number of bytes written.
 * If 'max_length' is 0 or 'fmt' is NULL, nothing else is done and 0 is returned.
 *
 * If printing succeeds, 0 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_print(brrstgT *const string, brrsz max_length, brrsz offset,
    brrsz *const written, const char *const fmt, ...);

/* Copies 'source' into 'string' and returns 0.
 * If 'string' or 'source' is NULL, nothing is done and 0 is returned.
 * If or 'source' is a NULL string, nothing is done and 1 is returned.
 * 'string' may be a NULL string.
 * If an error occurs, -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_copy(brrstgT *restrict const string, const brrstgT *restrict const source);
/* Joins 'suffix' onto 'prefix' and stores the result in 'destination'.
 * If 'destination' is NULL, nothing is done and 0 is returned.
 * If 'prefix' and 'suffix' are NULL, 'destination' will be the empty string.
 * If 'destination', 'prefix', or 'suffix' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'destination' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_merge(brrstgT *restrict const destination,
    const brrstgT *restrict const prefix, const brrstgT *restrict const suffix);
/* Appends 'suffix' to the end of 'string' and returns 0.
 * If 'string' or 'suffix' is NULL, nothing is done and 0 is returned.
 * If 'string' or 'suffix' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_append(brrstgT *restrict const string, const brrstgT *restrict const suffix);
/* Prepends 'prefix' at the beginning of 'string' and returns 0.
 * If 'string' or 'prefix' is NULL, nothing is done and 0 is returned.
 * If 'string' or 'prefix' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_prepend(brrstgT *restrict const string, const brrstgT *restrict const prefix);

/* Strip whitespace from the ends of 'string' and returns 0.
 * If 'filter' is non-NULL, 'filter' is used to determine whether a character should
 * be trimmed from 'string'; otherwise, 'isspace()' is used.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'string' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_strip(brrstgT *const string, int (*filter)(int chr));
/* Apply 'filter' on 'string', excluding characters for which 'filter' returns 0
 * and returns 0.
 * If 'filter' is non-NULL, 'filter' is used to determine whether a character should
 * be trimmed from 'string'; otherwise, 'isspace()' is used.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'string' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_filter(brrstgT *const string, int (*filter)(char chr, brrsz position, brrsz length));
/* Pastes the range '[start, end)' into 'string', overwriting it and returns 0.
 * If 'reverse' is non-zero, the copied range is copied in reverse.
 * If 'wrap' is 0, 'start' and 'end' will be clamped to 'source->length-1'
 * If 'wrap' is non-zero, 'start' and 'end' will be wrapped to 'source->length'
 * so that they are less than it.
 * After clamping or wrapping 'start' and 'end', if they are equal, 'string' will
 * be the empty string.
 * If 'string' or 'source' is NULL, nothing is done and 0 is returned.
 * If 'string' or 'source' is a NULL string, nothing is done and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_range(brrstgT *restrict const string, const brrstgT *restrict const source,
    brrsz start, brrsz end, int reverse, int wrap);

/* Joins all of 'strings' into 'string', with 'separator' separating them and returns 0.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'strings' is NULL or 'count' is 0, 'string' will be the empty string and 0
 * is returned.
 * If 'separator' is NULL, then there will be no separators.
 * If 'string' or 'separator' is a NULL string, nothing is done and 1 is returned.
 * If 'skip_empty' is non-zero, then any empty strings in 'strings' will not be
 * added and no separator will be placed for that string.
 * If any of 'strings' is a NULL string, joining is halted and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_ajoin(brrstgT *restrict const string, const brrstgT *restrict const separator,
    int skip_empty, brrsz count, const brrstgT *restrict const strings);
/* Joins all brrstgT pointers in 'lptr' into 'string', with 'separator' separating them and returns 0.
 * 'lptr' MUST be terminated with a NULL pointer.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'string' or 'separator' is a NULL string, nothing is done and 1 is returned.
 * If 'separator' is NULL, then there will be no separators.
 * If 'skip_empty' is non-zero, then any empty strings in 'strings' will not be
 * added and no separator will be placed for that string.
 * If any of strings in 'lptr' is a NULL string, joining is halted and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_vjoin(brrstgT *restrict const string, const brrstgT *restrict const separator,
    int skip_empty, brrsz *count, va_list lptr);
/* Joins a list of brrstgT pointers passed in into 'string', with 'separator' separating them and returns 0.
 * List of string arguments must be terminated with a NULL entry.
 * If 'string' is NULL, nothing is done and 0 is returned.
 * If 'separator' is NULL, then there will be no separators.
 * If 'string' or 'separator' is a NULL string, nothing is done and 1 is returned.
 * If 'skip_empty' is non-zero, then any empty strings in 'strings' will not be
 * added and no separator will be placed for that string.
 * If any string in the list is a NULL string, joining is halted and 1 is returned.
 * If an error occurs, 'string' is deleted and -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_join(brrstgT *restrict const string, const brrstgT *restrict const separator,
    int skip_empty, brrsz *count, ...);

/* Splits 'string' into a list of strings stored in 'strings' with the count stored
 * in 'count'; splitting starts at 'offset' and returns 0.
 * Splits will be at most 'max_split' characters in length.
 * If 'string' or 'split' is NULL, nothing is done and 0 is returned.
 * If 'string' or 'split' is a NULL string, nothing is done and 0 is returned.
 * If either 'string' or 'split' has length 0, 'strings' will contain a single element
 * that is a copy of the input 'string'.
 * If 'split' was not found in 'string', 'string' will contain a single element
 * that is a copy of the input 'string'.
 * If 'skip_consecutive' is non-zero, consecutive occurrences of 'split' will be
 * treated as a single instance.
 * If 'include_split' is non-zero, any single instance of 'split' will also be
 * included in the output.
 * If an error occurs, 'strings' will be NULL and -1 is returned.
 * TODO any operation mode that isn't 'skip_consecutive=1, include_split=0' behaves
 * a little strangely.
 * */
BRRAPI int BRRCALL brrstg_split(const brrstgT *restrict const string, const brrstgT *restrict const split,
    int skip_consecutive, int include_split, brrsz offset, brrsz max_split, brrstgT **const strings, brrsz *count);

/* Compares all arguments passed in to 'string' using standard string comparison
 * functions.
 * List of string arguments must be terminated with a NULL entry.
 * If 'string' is NULL, -1 is returned.
 * If 'case_sensitive' is non-zero, comparisons are done sensitive to case.
 * If the comparison turns up equal, the index of the equal comparison is returned.
 * If no string compared equal, -1 is returned.
 * */
BRRAPI int BRRCALL brrstg_cstr_compare(const char *const string, int case_sensitive, ...);

BRRCPPEND

#endif /* BRRSTG_H */
