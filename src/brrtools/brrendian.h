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

#ifndef BRRENDIAN_H
#define BRRENDIAN_H

#define BRRENDIAN_LITTLE  0x00010203UL
#define BRRENDIAN_BIG     0x03020100UL
#define BRRENDIAN_UNKNOWN 0x00000000UL

#if !defined(BRRENDIAN_SYSTEM) || \
	!(BRRENDIAN_SYSTEM == BRRENDIAN_LITTLE || BRRENDIAN_SYSTEM == BRRENDIAN_BIG)
# if defined(BRRSYSTEM_BITDEPTH)
#  undef BRRSYSTEM_BITDEPTH
# endif
# if defined(__GNUC__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#  else
#   error Unimplemented system endianness.
#  endif // __BYTE_ORDER__
# elif defined(BRRPLATFORMTYPE_UNIX)
#  include <sys/param.h>
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#  elif __BYTE_ORDER == __LITTLE_ENDIAN
#   define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#  else
#   error Unimplemented system endianness.
#  endif // __BYTE_ORDER
# elif defined(BRRPLATFORMTYPE_WINDOWS)
// can windows be big endian?
#  define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
# elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#  define BRRENDIAN_SYSTEM BRRENDIAN_BIG
# elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MISPEL__)
#  define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
# else
#  define BRRENDIAN_SYSTEM BRRENDIAN_UNKNOWN
# endif // __GNUC__
#endif // !BRRENDIAN_SYSTEM

#if BRRENDIAN_SYSTEM == BRRENDIAN_BIG
# define BRRSYSTEM_BIG_ENDIAN 1
#elif BRRENDIAN_SYSTEM == BRRENDIAN_LITTLE
# define BRRSYSTEM_LITTLE_ENDIAN 1
#endif

/* Enum representing different types of endianness. */
typedef enum brrendian {
	/* Value representing little-endian. */
	brrendian_little = BRRENDIAN_LITTLE,
	/* Value representing big-endian. */
	brrendian_big = BRRENDIAN_BIG,
	/* Value representing endianness of system.
	 * May be 'BRRENDIAN_LITTLE', 'BRRENDIAN_BIG', or 'BRRENDIAN_UNKNOWN'. */
	brrendian_system = BRRENDIAN_SYSTEM,
} brrendianT;

#endif /* BRRENDIAN_H */
