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

#ifndef BRRSTR_H
#define BRRSTR_H

#include <stdarg.h>

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* The length of a c-string, not including the null-terminator. */
typedef brrsz brrstrlen;

/* Represents a string.
 * 'cstr' must never be NULL, and must be allocated on the heap before calling
 * any manipulation functions. */
typedef struct brrstr {
	  char *cstr;
	  brrstrlen length;
} brrstrT;

/* Returns the length of 'cstr', excluding the null-terminator, up to and including 'max_strlen'.
 * If 'cstr' is NULL, 0 is returned.
 * */
BRRAPI brrstrlen BRRCALL brrstr_strlen(const char *const cstr, brrstrlen max_strlen);
/* Allocates a new brrstr from the given 'cstr', with a maximum length of 'max_strlen'.
 * If 'cstr' is NULL, it will be the same as if 'cstr' were the empty string "".
 * */
BRRAPI brrstrT BRRCALL brrstr_new(const char *const cstr, brrstrlen max_strlen);
/* Deallocates the string 'str'. */
BRRAPI void BRRCALL brrstr_delete(brrstrT *const str);
/* Duplicates 'str' and returns the duplicate. */
BRRAPI brrstrT BRRCALL brrstr_duplicate(const brrstrT str);
/* Initializes 'destination' with 'source'.
 * If 'destination' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrstr_copy(const brrstrT source, brrstrT *const destination);
/* Resizes 'str' to have a new cstr length.
 * If 'new_strlen' is greater than the current, then the new bytes will be initialized
 * to 0.
 * If 'str' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrstr_resize(brrstrT *const str, brrstrlen new_strlen);

/* Trims whitespace from the beginning and end of 'str' and returns a copy. */
BRRAPI brrstrT BRRCALL brrstr_trim(const brrstrT str);
/* Trims whitespace from the beginning and end of 'str'.
 * If 'str' is NULL, nothing is done.
 * */
BRRAPI void BRRCALL brrstr_strip(brrstrT *const str);

/* Returns the index of the first string in the arra ythat is equal to 'str', with
 * case-insensitive comparison if 'case_insensitive' is non-zero.
 * The set of strings passed must be terminated with NULL.
 * If there is no equality or 'arr' is NULL, -1 is returned.
 * Comparisons are done with 'strcmp'.
 * Passing 'str' as NULL is valid and will return the index of the NULL terminator.
 * */
BRRAPI int BRRCALL brrstr_astr_compare(const char *const str, int case_insensitive, const char *const *const arr);
/* Returns the index of the first string that is equal to 'str', with
 * case-insensitive comparison if 'case_insensitive' is non-zero.
 * The set of strings passed must be terminated with NULL.
 * If there is no equality, -1 is returned.
 * Comparisons are done with 'strcmp'.
 * Passing 'str' as NULL is valid and will return the index of the NULL terminator.
 * */
BRRAPI int BRRCALL brrstr_vstr_compare(const char *const str, int case_insensitive, va_list lptr);
/* Returns the index of the first string that is equal to 'str', with
 * case-insensitive comparison if 'case_insensitive' is non-zero.
 * The set of strings passed must be terminated with NULL.
 * If there is no equality, -1 is returned.
 * Comparisons are done with 'strcmp'.
 * Passing 'str' as NULL is valid and will return the index of the NULL terminator.
 * */
BRRAPI int BRRCALL brrstr_str_compare(const char *const str, int case_insensitive, ...);
/* Returns 1 if 'str' begins with 'prefix', otherwise returns 0.
 * If 'case_insensitive' is non-zero, comparison is done insensitive of case.
 * If 'prefix' is NULL, 0 is returned.
 * */
BRRAPI int BRRCALL brrstr_starts_with(const brrstrT str, const char *const prefix, int case_insensitive);
/* Returns 1 if 'str' ends with 'suffix', otherwise returns 0.
 * If 'case_insensitive' is non-zero, comparison is done insensitive of case.
 * If 'suffix' is NULL, 0 is returned.
 * */
BRRAPI int BRRCALL brrstr_ends_with(const brrstrT str, const char *const suffix, int case_insensitive);

/* Splits a string 'str' on each occurrence of 'separator' at and after 'offset',
 * and returns a heap-allocated list of 'brrstrT's, whose length is returned in
 * 'count'.
 * If 'str' is empty or is composed of nothing but 'separator', a single-element
 * list will be returned, whose element will be the empty string.
 * If an error occurs, NULL is returned and 'count' is set to 0, if it is not NULL.
 * */
BRRAPI brrstrT *BRRCALL brrstr_split(const brrstrT str, char separator, brrsz offset,
    brrsz max_split_size, brrsz *const count);

#endif /* BRRSTR_H */
