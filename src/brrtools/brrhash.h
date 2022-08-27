/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrhash_h
#define brrtools_brrhash_h

#ifdef __cplusplus
extern "C" {
#endif

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Computes a FNV-1a hash on 'data' of length 'data_size'.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and a default value
 * is returned.
 * */
BRRAPI brru8 BRRCALL brrhash_fnv1a(const void *const data, brrsz data_size);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrhash_h */
