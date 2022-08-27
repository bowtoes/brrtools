/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrrand_h
#define brrtools_brrrand_h

#ifdef __cplusplus
extern "C" {
#endif

/* None of this is cryptographic, please don't think so */

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Generates the next random unsigned 64-bit integer and returns the result.
 * */
BRRAPI brru8 BRRCALL brrrand_rand(void);
BRRAPI void BRRCALL brrrand_seed(brru8 seed);
/* Generates 'iter' random numbers seeded with 'seed' and returns the last.
 * */
BRRAPI brru8 BRRCALL brrrand_srand(brru8 seed, brru8 iter);
/* Generates 'iter' random numbers with seeded with the current time in microseconds and returns the last.
 * */
BRRAPI brru8 BRRCALL brrrand_trand(brru8 iter);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrrand_h */
