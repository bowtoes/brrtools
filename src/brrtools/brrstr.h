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

#include <brrtools/brrapi.h>
#include <brrtools/brrbuffer.h>

typedef struct brrbuffer brrstrT;

/* The length of a c-string, never including the null-terminator.
 * */
typedef brrsz brrstrlen;

/* Returns the string length of 'cstr', excluding the null-terminator, up to a maximum of 'max_strlen'.
 * If 'cstr' is NULL, 0 is returned.
 * */
BRRAPI brrstrlen BRRCALL brrstr_cstrlen(const char *const cstr, brrstrlen max_strlen);
/* Returns the string length of the c-string represented by 'str'.
 * If 'str' is invalid (NULL or 'opaque' is NULL), 0 is returned.
 * */
BRRAPI brrstrlen BRRCALL brrstr_strlen(const brrstrT *const str);

/* Creates a new 'brrstrT' from the given 'cstr', with a maximum string length specified by 'max_strlen'.
 * If 'cstr' is NULL or 'max_strlen' is 0, a string with a single null byte is returned.
 * New strings always have a null-terminator.
 * */
BRRAPI brrstrT BRRCALL brrstr_new(const char *const cstr, brrstrlen max_strlen);

/* Returns a pointer to the c-string represented by 'str'.
 * If 'str' is invalid (NULL or 'opaque' is NULL), NULL is returned.
 * */
BRRAPI const char *BRRCALL brrstr_cstr(const brrstrT *const str);

/* Returns the index of the first c-string that is equal to the c-string 'str', with
 * case-insensitive comparison if 'case_insensitive' is non-zero.
 * The variable set of c-strings passed must be terminated with NULL.
 * If there is no equality, -1 is returned.
 * Comparisons are done with 'strcmp'.
 * */
BRRAPI int BRRCALL brrstr_cstr_compare(const char *const str, int case_insensitive, ...);
/* Returns the index of the first c-string that is equal to the c-string represented
 * by 'str', with case-insensitive comparison if 'case_insensitive' is non-zero.
 * The variable set of c-strings passed must be terminated with NULL.
 * If there is no equality, -1 is returned.
 * Comparisons are done with 'strcmp'.
 * */
BRRAPI int BRRCALL brrstr_str_compare(const brrstrT *const str, int case_insensitive, ...);

/* Returns 1 when 'str' begins with the given 'prefix', with case-insensitive comparison
 * if 'case_insensitive' is non-zero.
 * */
BRRAPI int BRRCALL brrstr_starts_with(const brrstrT *const str, const char *const prefix, int case_insensitive);
/* Returns 1 when 'str' ends with the given 'prefix', with case-insensitive comparison
 * if 'case_insensitive' is non-zero.
 * */
BRRAPI int BRRCALL brrstr_ends_with(const brrstrT *const str, const char *const suffix, int case_insensitive);

#endif /* BRRSTR_H */
