/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef BRRTOOLS_TYPES_H
#define BRRTOOLS_TYPES_H

#include <stdint.h>
#include <stddef.h>

#ifndef BRRBITDEPTH_SYSTEM
#include <brrtools/brrendian.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef int8_t      brrs1;
typedef uint8_t     brru1;
typedef int16_t     brrs2;
typedef uint16_t    brru2;
typedef int32_t     brrs4;
typedef uint32_t    brru4;
typedef int64_t     brrs8;
typedef uint64_t    brru8;

typedef _Bool       brrbl;
#define brrtrue 1
#define brrfalse 0

#define BRRS1_MAX  INT8_MAX
#define BRRS1_MIN  INT8_MIN
#define BRRU1_MAX  UINT8_MAX
#define BRRS2_MAX  INT16_MAX
#define BRRS2_MIN  INT16_MIN
#define BRRU2_MAX  UINT16_MAX
#define BRRS4_MAX  INT32_MAX
#define BRRS4_MIN  INT32_MIN
#define BRRU4_MAX  UINT32_MAX
#define BRRS8_MAX  INT64_MAX
#define BRRS8_MIN  INT64_MIN
#define BRRU8_MAX  UINT64_MAX

typedef size_t brrsz; // Byte count or absolute byte offset.
# define BRRSZ_MAX SIZE_MAX

#if BRRBITDEPTH_SYSTEM == BRRBITDEPTH_64
typedef brrs8 brrof; // Relative byte offset.
# define BRROF_MAX BRRS8_MAX
# define BRROF_MIN BRRS8_MIN
#else
typedef brrs4 brrof; // Relative byte offset.
# define BRROF_MAX BRRS4_MAX
# define BRROF_MIN BRRS4_MIN
#endif

#if defined(__cplusplus)
}
#endif

#endif /* BRRTOOLS_TYPES_H */
