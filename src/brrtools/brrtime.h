/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrtime_h
#define brrtools_brrtime_h

#ifdef __cplusplus
extern "C" {
#endif

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Returns the current time-of-day in microseconds.
 * */
BRRAPI brru8 BRRCALL brrtime_utime(void);

/* Pauses program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrtime_sleep(brru8 usec);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrtime_h */
