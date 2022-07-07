/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef BRRTIME_H
#define BRRTIME_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Returns the current time-of-day in microseconds.
 * */
BRRAPI brru8 BRRCALL brrtime_utime(void);
/* Pauses program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrtime_sleep(brru8 usec);

#if defined(__cplusplus)
}
#endif

#endif /* BRRTIME_H */
