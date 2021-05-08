#ifndef BRRTOOLS_TYPES_H
#define BRRTOOLS_TYPES_H

#include <stdint.h>
#include <stdbool.h>

typedef int8_t      brrs1;
typedef uint8_t     brru1;
typedef int16_t     brrs2;
typedef uint16_t    brru2;
typedef int32_t     brrs4;
typedef uint32_t    brru4;
typedef int64_t     brrs8;
typedef uint64_t    brru8;

#define BRRTOOLS_BRRS1_MAX  INT8_MAX
#define BRRTOOLS_BRRS1_MIN  INT8_MIN
#define BRRTOOLS_BRRU1_MAX  UINT8_MAX
#define BRRTOOLS_BRRS2_MAX  INT16_MAX
#define BRRTOOLS_BRRS2_MIN  INT16_MIN
#define BRRTOOLS_BRRU2_MAX  UINT16_MAX
#define BRRTOOLS_BRRS4_MAX  INT32_MAX
#define BRRTOOLS_BRRS4_MIN  INT32_MIN
#define BRRTOOLS_BRRU4_MAX  UINT32_MAX
#define BRRTOOLS_BRRS8_MAX  INT64_MAX
#define BRRTOOLS_BRRS8_MIN  INT64_MIN
#define BRRTOOLS_BRRU8_MAX  UINT64_MAX

typedef brru8   brrsz; // Byte count.
#define BRRTOOLS_BRRSZ_MAX BRRTOOLS_BRRU8_MAX
typedef brrs8   brrof; // Relative byte offset.
#define BRRTOOLS_BRROF_MAX BRRTOOLS_BRRS8_MAX
#define BRRTOOLS_BRROF_MIN BRRTOOLS_BRRS8_MAX
typedef brru1   brrby; // Byte.
#define BRRTOOLS_BRRBY_MAX 255
typedef bool    brrb1; // Bool.

typedef union brrfcc {
	char fourcc[4]; // Four-character char code.
	brru4 code; // Numeric code.
} brrfccT;

#endif /* BRRTOOLS_TYPES_H */
