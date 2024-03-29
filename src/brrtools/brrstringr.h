/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrstringr_h
#define brrtools_brrstringr_h

/* Major TODO Microsoft 'wchar_t' on Windows, for proper unicode support there. */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* String representation that stores length of string.
 * Fields:
 * - void *cstr    : Pointer storing the 'cstr' represented.
 * - brrsz length  : Length of the c-string, excluding NULL terminator.
 * - brrbl shallow : If it's unsafe for the library to free/realloc 'cstr'.
 * */
typedef struct brrstringr {
	char *cstr;    /* The string itself */
	brrsz length;  /* Length of the c-string, excluding NULL terminator */
	brrbl shallow; /* If it's unsafe to free/realloc 'cstr' */
} brrstringr_t;

/* Computes the length of 'string', up to 'max_length', and returns the result.
 * Does not count the null terminator.
 * */
BRRAPI brrsz BRRCALL
brrstringr_length(const char *const chars, brrsz max_length);

/* Takes a char array or string-literal and casts it as a shallow brrstringr, with correct length.
 * */
#define brrstringr_literal(_s) {.cstr=_s,.length=sizeof(_s)-1,.shallow=brrtrue}

/* Returns a brrstringr version of 'cstr', with length 'length'.
 * Does not copy 'cstr'; is marked shallow.
 * */
inline brrstringr_t brrstringr_shallow(const char *const chars, brrsz length)
{ return (brrstringr_t){.shallow=brrtrue,.cstr=(char*)chars,.length=length}; }

/* Casts 'cstr' to a brrstringr version, computing the length of 'cstr'.
 * Does not copy 'cstr'; is marked shallow.
 * */
inline brrstringr_t brrstringr_cast(const char *const chars)
{ return (brrstringr_t){.shallow=brrtrue,.cstr=(char*)chars,.length=brrstringr_length(chars,-1)}; }

/* Creates a new brrstringr with a copy of 'cstr' of length 'length' and stores the result in 'string'.
 * Returns 0 on success.
 * If allocation fails or 'string' is NULL, 'string' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_new(brrstringr_t *const string, const char *const chars, brrsz length);

/* Frees the data associated with 'string', if it is not shallow.
 * */
BRRAPI void BRRCALL
brrstringr_free(brrstringr_t *const string);

/* Clears 'string' to be of zero length.
 * Returns 0 on success.
 * If an error occurs or 'string' is NULL or shallow, 'string' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_clear(brrstringr_t *const string);

BRRAPI int BRRCALL
brrstringr_shrink_right(brrstringr_t *const string, brrsz new_len);

/* Copies 'source' into 'string', reallocating as necessary, or creates a new string in 'string' if 'string' is shallow.
 * Returns 0 on success.
 * If allocation fails or 'string' or 'source' are NULL, 'string' is left unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_copy(brrstringr_t *restrict const string, const brrstringr_t *restrict const source);
/* Trims either the leading, tailing, or both sets of whitespace from 'string', depending on whether 'leading' and 'tailing' are non-zero.
 * Returns 0 on success.
 * If allocation fails or 'string' is NULL or 'string' is shallow, 'string' is left unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_trim_whitespace(brrstringr_t *const string, brrbl leading, brrbl tailing);

/* Performs 'snprintf' into 'string' at the given offset, printing at most 'max_length + 1' characters with the given 'format'.
 * Returns the number of characters printed on success.
 * If allocation fails or 'string' or 'format' are NULL or 'string' is shallow, 'string' is left unaffected and BRRSZ_MAX is returned.
 * If 'offset' is past the end of 'string', the space in-between is filled with individual spaces.
 * */
BRRAPI brrsz BRRCALL
brrstringr_print(brrstringr_t *const string, brrsz offset, brrsz max_length, const char *const format, ...);

/* Performs 'vsnprintf' on 'lptr' into 'string' at the given offset, printing at most 'max_length + 1' characters with the given 'format'.
 * Returns the number of characters printed on success.
 * If allocation fails or 'string' or 'format' are NULL or 'string' is shallow, 'string' is left unaffected and BRRSZ_MAX is returned.
 * If 'offset' is past the end of 'string', the space in-between is filled with individual spaces.
 * */
BRRAPI brrsz BRRCALL
brrstringr_vprint(brrstringr_t *const string, brrsz offset, brrsz max_length, const char *const format, va_list lptr);

/* TODO currently quarantined, will fix soon tm */
#if 0
/* Splits the input 'string' on 'delimiter' into the array of 'strings' whose length is returned in 'n_strings',
 *   each split at most 'max_split' characters in length.
 * Returns 0 on success.
 * If allocation fails or 'strings' or 'n_strings' are NULL, 'strings' and 'n_strings' are unaffected and -1 is returned.
 * If 'skip_consecutive' is true, then consecutive matches of 'delimiter' in the input 'string' will not produce an
 *   empty split in the output 'strings'.
 * If 'case_sensitive' is true, then matching for 'delimiters' in 'string' is done with respect to case.
 * */
BRRAPI int BRRCALL
brrstringr_split(
    brrstringr_t **const strings,
    brrsz *n_strings,
    const brrstringr_t *const string,
    const brrstringr_t *const delimiter,
    brrsz max_split,
    brrbl skip_consecutive,
    brrbl case_sensitive
);

/* Splits the input 'string' on 'delimiters' of 'n_delimiters' into the array of 'strings' whose length is
 *   returned in 'n_strings', each split at most 'max_split' characters in length.
 * Returns 0 on success.
 * If allocation fails or 'strings' or 'n_strings' are NULL, 'strings' and 'n_strings' are left unaffected and -1 is returned.
 * If 'skip_consecutive' is true, then consecutive matches of 'delimiters' in the input 'string' will not produce
 *   an empty split in the output 'strings'.
 * If 'case_sensitive' is true, then matching for 'delimiters' in 'string' is done with respect to case.
 * */
BRRAPI int BRRCALL
brrstringr_split_multiple(
    brrstringr_t **const strings,
    brrsz *n_strings,
    const brrstringr_t *const string,
    const brrstringr_t *const delimiters,
    brrsz n_delimiters,
    brrsz max_split,
    brrbl skip_consecutive,
    brrbl case_sensitive
);

/* Joins the array of 'strings' of length 'n_strings' into 'string', delimited by 'delimiter'.
 * Returns 0 on success.
 * If allocation fails or 'string' or 'strings' are NULL, 'string' is left unaffected and -1 is returned. 'delimiter' may be NULL.
 * */
BRRAPI int BRRCALL
brrstringr_join(
    brrstringr_t *const string,
    const brrstringr_t *const delimiter,
    const brrstringr_t *const strings,
    brrsz n_strings
);
#endif

/* Filters out characters from 'string' for which 'filter()', taking the input character and the next, returns non-zero;
 *   or, if 'invert' is true, filters out characters for which 'filter()' DOES return 0.
 * Returns 0 on success.
 * If allocation fails, 'string' is NOT guaranteed to be unaffected and -1 is returned; 'string' length however IS unaffected in this case.
 * If 'string' or 'filter' are NULL or 'string' is shallow, 'string' is left unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_filter_chars(brrstringr_t *const string, int (*filter)(int, int), brrbl invert);

/* Filters out characters from 'string' for which 'filter()', which is given the input character and the next, position, and string
 *   length, returns non-zero; or, if 'invert' is true, filters out characters for which 'filter()' DOES return 0.
 * Returns 0 on success.
 * If 'string' or 'filter' are NULL or 'string' is shallow, 'string' is left unaffected and -1 is returned.
 * If allocation fails, 'string' is NOT guaranteed to be unaffected and -1 is returned; 'string' length however IS unaffected in this case.
 * */
BRRAPI int BRRCALL
brrstringr_filter(brrstringr_t *const string, int (*filter)(int, int, brrsz, brrsz), brrbl invert);

/* Compare 'string' and 'other' using 'strcmp' and return the result.
 * Does case-sensitive comparison if 'case_sensitive' is true.
 * If 'string' or 'other' are NULL or their 'cstr's are NULL, 0 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_compare(const brrstringr_t *const string, const brrstringr_t *const other, brrbl case_sensitive);

/* Compare 'string' and 'other' using 'strncmp' and return the result, comparing at most the minimum length of the two characters.
 * Does case-sensitive comparison if 'case_sensitive' is true.
 * If 'string' or 'other' are NULL or their 'cstr's are NULL, 0 is returned.
 * */
BRRAPI int BRRCALL
brrstringr_ncompare(const brrstringr_t *const string, const brrstringr_t *const other, brrbl case_sensitive);

/* Compares 'string' to all of the passed arguments for as long as they are not NULL, and returns the first which matches 'string',
 *   or NULL if no matches are found.
 * If 'string' is NULL, NULL is returned.
 * */
BRRAPI char *BRRCALL
brrstringr_cstr_compare(const char *const chars, int case_sensitive, ...);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrstringr_h */
