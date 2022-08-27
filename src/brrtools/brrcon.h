/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrcon_h
#define brrtools_brrcon_h

#ifdef __cplusplus
extern "C" {
#endif

#include <brrtools/brrapi.h>

/* Pauses console until a character is pushed to stdin.
 * Returns 0 if implemented on the host system, 1 otherwise.
 * */
BRRAPI int BRRCALL brrcon_pause(void);
/* Clears console and puts cursor at start position.
 * Returns 0 if implemented on the host system, 1 otherwise.
 * */
BRRAPI int BRRCALL brrcon_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrcon_h */
