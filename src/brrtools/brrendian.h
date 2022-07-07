/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef BRRENDIAN_H
#define BRRENDIAN_H

#if defined(__cplusplus)
extern "C" {
#endif

/* ---------- Bitdepth detection */
#if !defined(BRRTOOLS_NO_BITDEPTH)
# define BRRBITDEPTH_64      64
# define BRRBITDEPTH_32      32
# define BRRBITDEPTH_UNKNOWN 0

# if defined(BRRBITDEPTH) && BRRBITDEPTH != BRRBITDEPTH_32 && BRRBITDEPTH != BRRBITDEPTH_64
#  undef BRRBITDEPTH
# endif
# if !defined(BRRBITDEPTH)
#  if defined(__LP64) || defined(__LP64__) || defined(_WIN64) || \
     defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM64) || defined(_M_AMD64) || \
     defined(__ia64) || defined(__aarch64__) || \
     defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || \
     (defined(__x86_64__) && !defined(__ILP32__))
#   define BRRBITDEPTH BRRBITDEPTH_64
#  else
#   define BRRBITDEPTH BRRBITDEPTH_32
#  endif
# endif

/* Enum representing different system bitdepths. */
typedef enum brrbitdepth {
	brrbitdepth_unknown = BRRBITDEPTH_UNKNOWN,
	brrbitdepth_32 = BRRBITDEPTH_32,  /* Value for a 32-bit system. */
	brrbitdepth_64 = BRRBITDEPTH_64,  /* Value for a 64-bit system. */
	brrbitdepth_system = BRRBITDEPTH, /* Value representing bitdepth of the current system. */
} brrbitdepth_t;
#endif

/* ---------- Endianness detection */
#if !defined(BRRTOOLS_NO_ENDIAN)
# define BRRENDIAN_LITTLE  0x00010203UL
# define BRRENDIAN_BIG     0x03020100UL
# define BRRENDIAN_UNKNOWN 0x00000000UL

# if defined(BRRENDIAN) && BRRENDIAN != BRRENDIAN_LITTLE && BRRENDIAN != BRRENDIAN_BIT
#  undef BRRENDIAN
#endif
# if !defined(BRRENDIAN)
#  if defined(__GNUC__)
#   if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define BRRENDIAN BRRENDIAN_BIG
#   elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define BRRENDIAN BRRENDIAN_LITTLE
#   else
#    error Unimplemented system endianness.
#   endif // __BYTE_ORDER__
#  elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
#   include <sys/param.h>
#   if __BYTE_ORDER == __BIG_ENDIAN
#    define BRRENDIAN BRRENDIAN_BIG
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
#    define BRRENDIAN BRRENDIAN_LITTLE
#   else
#    error Unimplemented system endianness.
#   endif // __BYTE_ORDER
#  elif defined(WIN32) /|| defined(__WIN32) || defined(__WIN32__) // Windows
#   define BRRENDIAN BRRENDIAN_LITTLE
#  elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
     defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#   define BRRENDIAN BRRENDIAN_BIG
#  elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
     defined(_MIPSEL) || defined(__MIPSEL) || defined(__MISPEL__)
#   define BRRENDIAN BRRENDIAN_LITTLE
#  else
#   define BRRENDIAN BRRENDIAN_UNKNOWN
#  endif // __GNUC__
# endif // !BRRENDIAN

# if BRRENDIAN == BRRENDIAN_BIG
#  define BRRSYSTEM_BIG_ENDIAN 1
# elif BRRENDIAN == BRRENDIAN_LITTLE
#  define BRRSYSTEM_LITTLE_ENDIAN 1
# endif

/* Enum representing different types of endianness. */
typedef enum brrendian {
	brrendian_unkown = BRRENDIAN_UNKNOWN,
	brrendian_little = BRRENDIAN_LITTLE, /* Value representing little-endian systems. */
	brrendian_big    = BRRENDIAN_BIG,    /* Value representing big-endian systems. */
	brrendian_system = BRRENDIAN, /* Value representing the endianness of the system. */
} brrendian_t;
#endif /* !BRRTOOLS_NO_ENDIAN */

#if defined(__cplusplus)
}
#endif

#endif /* BRRENDIAN_H */
