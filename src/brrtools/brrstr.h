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
