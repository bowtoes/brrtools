/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef BRRCON_H
#define BRRCON_H

#include <brrtools/brrapi.h>

#if defined(__cplusplus)
extern "C" {
#endif

/* Pauses console until a character is pushed to stdin.
 * Returns 0 if implemented on the host system, 1 otherwise.
 * */
BRRAPI int BRRCALL brrcon_pause(void);
/* Clears console and puts cursor at start position.
 * Returns 0 if implemented on the host system, 1 otherwise.
 * */
BRRAPI int BRRCALL brrcon_clear(void);

#if defined(__cplusplus)
}
#endif

#endif /* BRRCON_H */
