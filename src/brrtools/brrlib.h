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

#ifndef BRRTOOLS_BRRLIB_H
#define BRRTOOLS_BRRLIB_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/*
 * TODO
 *
 * It would be best to split this up into a few different modules, like
 * 'brrtim' for time,
 * 'brrrnd' for random,
 * 'brrcon' for console stuff,
 * 'brrnum' for some number algorithms,
 * */

BRRCPPSTART

/* Pause console until a character is pushed to stdin.
 * Returns 1.
 * */
BRRAPI int BRRCALL brrlib_pause(void);
/* Clears console and puts cursor at start position.
 * Returns 1.
 * */
BRRAPI int BRRCALL brrlib_clear(void);

/* Returns the current time in microseconds.
 * Useful for timing things.
 * Should not be used as an accurate indication of real-world time.
 * */
BRRAPI brru8 BRRCALL brrlib_utime(void);
/* Pause program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrlib_usleep(brru8 usec);

/* Allocates/reallocates the memory location pointed to by 'current'.
 * If 'size' is 0 and 'current' points to a non-NULL pointer, the pointer pointed
 * to by 'current' will be freed and set to NULL.
 * If allocation succeeds, 0 is returned.
 * If an error occurs, -1 is returned.
 * */
BRRAPI int BRRCALL brrlib_alloc(void **current, brrsz size, int zero);
/* Counts the number of digits in 'number'. If 'is_signed' is non-zero, 'number'
 * is treated as if it is a signed integer.
 * The sign is not counted in resulting digit count.
 * */
BRRAPI brrsz BRRCALL brrlib_ndigits(brru8 number, int is_signed, brru1 base);
BRRAPI brrs8 BRRCALL brrlib_wrap(brrs8 number, brru8 wrap, brrs8 offset);

/* Generates a random unsigned 64-bit integer and returns the result. */
BRRAPI brru8 BRRCALL brrlib_rand(void);
/* Seeds the random number generator and returns first result. */
BRRAPI brru8 BRRCALL brrlib_srand(brru8 seed);
/* Generates a random number with microsecond time as the generator's seed. */
BRRAPI brru8 BRRCALL brrlib_trand(void);

/* Computes a FNV-1a hash on 'data' of length 'data_size'.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and a default value is returned.
 * */
BRRAPI brru8 BRRCALL brrlib_hash(const void *const data, brrsz data_size);

/* Returns the maximum of the unsigned integers 'a' and 'b'. */
BRRAPI brru8 BRRCALL brrlib_umax(brru8 a, brru8 b);
/* Returns the minimum of the unsigned integers 'a' and 'b'. */
BRRAPI brru8 BRRCALL brrlib_umin(brru8 a, brru8 b);
/* Clamps the unsigned input value 'x' within the range [min, max]. */
BRRAPI brru8 BRRCALL brrlib_uclamp(brru8 x, brru8 min, brru8 max);
/* Returns the maximum of the signed integers 'a' and 'b'. */
BRRAPI brrs8 BRRCALL brrlib_smax(brrs8 a, brrs8 b);
/* Returns the minimum of the signed integers 'a' and 'b'. */
BRRAPI brrs8 BRRCALL brrlib_smin(brrs8 a, brrs8 b);
/* Clamps the signed input value 'x' within the range [min, max]. */
BRRAPI brrs8 BRRCALL brrlib_sclamp(brrs8 x, brrs8 min, brrs8 max);

/* Returns the unsigned greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned.
 * */
BRRAPI brru8 BRRCALL brrlib_ugcf(brru8 a, brru8 b);
/* Returns the signed greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned.
 * */
BRRAPI brrs8 BRRCALL brrlib_sgcf(brrs8 a, brrs8 b);

BRRCPPEND

#endif /* BRRTOOLS_BRRLIB_H */
