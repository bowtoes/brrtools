/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef BRRTOOLS_BRRLIB_H
#define BRRTOOLS_BRRLIB_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* (Re)Allocates memory to the pointer pointed to by 'current'.
 * Returns 0 on success.
 * If an error occurs, '*current' is left unaffected and -1 returned.
 * If 'current' is NULL, nothing is done and 0 is returned.
 * If 'size' is 0, '*current' is freed and set to NULL.
 * If 'blank != 0' and 'size > 0', the memory (re)allocated is set to 0.
 * */
BRRAPI int BRRCALL brrlib_alloc(void **current, brrsz size, brrbl zero);

#if defined(__cplusplus)
}
#endif

#endif /* BRRTOOLS_BRRLIB_H */
