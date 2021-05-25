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

BRRCPPSTART

extern BRRAPI brrb1 brrlib_use_extended_bases;
extern BRRAPI const char brrlib_bases[];

/* Returns the current maximum printable base, depending on the state of 'brrlib_use_extended_bases'. */
BRRAPI brrsz BRRCALL brrlib_max_base(void);

/* Prints a the given 'number', whose signededness is determined by 'is_signed',
 * into 'destination' in the given 'base'.
 * Prints at most 'max_length' bytes, including the null-terminator.
 * Returns total bytes printed, excluding the null-terminator. */
BRRAPI brrsz BRRCALL brrlib_print_base(char *const destination, brru8 max_length,
    brru8 number, brrb1 is_signed, brru1 base);

/* Pause console until a character is pushed to stdin.
 * Returns true. */
BRRAPI brrb1 BRRCALL brrlib_pause(void);
/* Clears console and puts cursor at start position.
 * Returns true. */
BRRAPI brrb1 BRRCALL brrlib_clear(void);

/* Returns the current time in microseconds.
 * NOTE: Should not be used as an accurate indication of real-world time.
 * Useful for timing things. */
BRRAPI brru8 BRRCALL brrlib_utime(void);
/* Pause program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrlib_usleep(brru8 usec);

/* Allocates/reallocates memory location pointed to by 'current' and returns
 * 'true' on success, 'false' on failure.
 * If 'size' is 0 and 'current' points to a non-NULL pointer, the pointer pointed
 * to by 'current' will be freed and set to NULL. */
BRRAPI brrb1 BRRCALL brrlib_alloc(void **current, brrsz size, brrb1 zero);
/* Counts the number of digits in 'number'. If 'is_signed' is true, 'number'
 * is treated as if it is signed.
 * Sign is not counted in resulting digit count. */
BRRAPI brrsz BRRCALL brrlib_ndigits(brrb1 is_signed, brru8 number, brru1 base);
BRRAPI brrs8 BRRCALL brrlib_wrap(brrs8 number, brru8 wrap, brrs8 offset);

/* Generates a random unsigned 64-bit integer and returns the result. */
BRRAPI brru8 BRRCALL brrlib_rand(void);
/* Seeds the random number generator and returns first result. */
BRRAPI brru8 BRRCALL brrlib_srand(brru8 seed);
/* Generates a random number with microsecond time as the generator's seed.  */
BRRAPI brru8 BRRCALL brrlib_trand(void);

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

#define BRRLIB_DUM_MAX(a, b) ((a)>(b)?(a):(b)) /* Only use expressions without side effects if using these. */
#define BRRLIB_DUM_MIN(a, b) ((a)<(b)?(a):(b))
#define BRRLIB_DUM_CLAMP(x, min, max) ((x)<(min)?(min):(x)>(max)?(max):(x))

/* Returns the unsigned greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned. */
BRRAPI brru8 BRRCALL brrlib_ugcf(brru8 a, brru8 b);
/* Returns the signed greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned. */
BRRAPI brrs8 BRRCALL brrlib_sgcf(brrs8 a, brrs8 b);

BRRCPPEND

#endif /* BRRTOOLS_BRRLIB_H */
