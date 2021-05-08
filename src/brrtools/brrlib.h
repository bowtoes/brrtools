#ifndef BRRTOOLS_BRRLIB_H
#define BRRTOOLS_BRRLIB_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* Pause console until a character is pushed to stdin.
 * Returns true. */
BRRAPI brrb1 BRRCALL brrlib_pause(void);
/* Clears console and puts cursor at start position.
 * Returns true. */
BRRAPI brrb1 BRRCALL brrlib_clear(void);

/* Returns the current time in microseconds. */
BRRAPI brru8 BRRCALL brrlib_utime(void);
/* Pause program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrlib_usleep(brru8 usec);

/* Allocates/reallocates memory location pointed to by 'current' and returns
 * 'true' on success, 'false' on failure. */
BRRAPI brrb1 BRRCALL brrlib_alloc(void **current, brrsz size, brrb1 zero);
/* Counts the number of digits in 'number'. If 'is_signed' is true, 'number'
 * is treated as if it is signed.
 * Sign is not counted in resulting digit count. */
BRRAPI brrsz BRRCALL brrlib_ndigits(brrb1 is_signed, brru8 number);
BRRAPI brrs8 BRRCALL brrlib_wrap(brrs8 number, brru8 wrap, brrs8 offset);

/* Generates a random unsigned 64-bit integer and returns the result. */
BRRAPI brru8 BRRCALL brrlib_rand(void);
/* Seeds the random number generator and returns first result. */
BRRAPI brru8 BRRCALL brrlib_srand(brru8 seed);
/* Generates a random number with microsecond time as the generator's seed.  */
BRRAPI brru8 BRRCALL brrlib_trand(void);

BRRCPPEND

#endif /* BRRTOOLS_BRRLIB_H */
