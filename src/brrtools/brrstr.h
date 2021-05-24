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

#ifndef BRRTOOLS_STRTOOLS_H
#define BRRTOOLS_STRTOOLS_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Maximum length that will be measured by 'brrstr_len'.
 * Default is 4096. */
extern BRRAPI brrsz brrstr_maxlen;

/* Returns the length of string 's', not counting null-terminator.
 * Maximum measurable length is 'brrstr_maxlen'. */
BRRAPI brrsz BRRCALL brrstr_len(const char *const s);
/* Duplicates 's' into a newly allocated space in memory that is 'size+1' bytes
 * and returns a pointer to the duplicate memory. */
BRRAPI char *BRRCALL brrstr_dup(const char *const s, brrsz *const size);
BRRAPI char *BRRCALL brrstr_merge(const char *const a, const char *const b, brrsz *const size);
/* Appends 'suffix' to the end of 's' and returns 's'. Length of modified string returned in 'newsize'.
 * 's' must have sufficient space allocated. */
BRRAPI char *BRRCALL brrstr_append(char *const s, const char *const suffix, brrsz *const newsize);

#endif /* BRRTOOLS_STRTOOLS_H */
