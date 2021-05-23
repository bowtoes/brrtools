#ifndef BRRTOOLS_BRRLIB_H
#define BRRTOOLS_BRRLIB_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

extern BRRAPI brrb1 brrlib_use_extended_bases;
extern BRRAPI const char brrlib_bases[];

BRRAPI brrsz BRRCALL brrlib_max_base(void);

BRRAPI brrsz BRRCALL brrlib_print_base(char *const dst, brru8 max_length,
    brru8 number, brrb1 is_signed, brru1 base);

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

BRRAPI brru8 BRRCALL brrlib_umax(brru8 a, brru8 b);
BRRAPI brru8 BRRCALL brrlib_umin(brru8 a, brru8 b);
BRRAPI brru8 BRRCALL brrlib_uclamp(brru8 x, brru8 min, brru8 max);
BRRAPI brrs8 BRRCALL brrlib_smax(brrs8 a, brrs8 b);
BRRAPI brrs8 BRRCALL brrlib_smin(brrs8 a, brrs8 b);
BRRAPI brrs8 BRRCALL brrlib_sclamp(brrs8 x, brrs8 min, brrs8 max);

#define BRRLIB_DUM_MAX(a, b) ((a)>(b)?(a):(b)) /* Only use expressions without side effects if using these. */
#define BRRLIB_DUM_MIN(a, b) ((a)<(b)?(a):(b))
#define BRRLIB_DUM_CLAMP(x, min, max) ((x)<(min)?(min):(x)>(max)?(max):(x))

BRRCPPEND

#endif /* BRRTOOLS_BRRLIB_H */
