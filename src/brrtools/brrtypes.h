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

#ifndef BRRTOOLS_TYPES_H
#define BRRTOOLS_TYPES_H

#include <stdint.h>

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

// Byte count or absolute byte offset.
typedef brru8   brrsz;
#define BRRTOOLS_BRRSZ_MAX BRRTOOLS_BRRU8_MAX
// Relative byte offset.
typedef brrs8   brrof;
#define BRRTOOLS_BRROF_MAX BRRTOOLS_BRRS8_MAX
#define BRRTOOLS_BRROF_MIN BRRTOOLS_BRRS8_MIN
// Byte.
typedef brru1   brrby;
#define BRRTOOLS_BRRBY_MAX 255

// Array element count.
typedef brru2   brrct;
#define BRRTOOLS_BRRCT_MAX BRRTOOLS_BRRU2_MAX

/* Elements:
 *  * char fourcc[4];
 *  * brru4 code; */
typedef union brrfcc {
	char fourcc[4]; // Four-character char code.
	brru4 code; // Numeric code.
} brrfccT;

#endif /* BRRTOOLS_TYPES_H */
