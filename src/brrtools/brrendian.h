#ifndef BRRENDIAN_H
#define BRRENDIAN_H

#if defined(__cplusplus)
extern "C" {
#endif

/* ---------- Bitdepth detection */
#if !defined(BRRTOOLS_NO_BITDEPTH)
# define BRRBITDEPTH_64BIT   64
# define BRRBITDEPTH_32BIT   32
# define BRRBITDEPTH_UNKNOWN 0

# if !defined(BRRBITDEPTH_SYSTEM) || \
     !(BRRBITDEPTH_SYSTEM == BRRBITDEPTH_32BIT || BRRBITDEPTH_SYSTEM == BRRBITDEPTH_64BIT)
#  if defined(BRRBITDEPTH_SYSTEM)
#   undef BRRBITDEPTH_SYSTEM
#  endif
#  if defined(__LP64) || defined(__LP64__) || defined(_WIN64) || \
     defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM64) || defined(_M_AMD64) || \
     defined(__ia64) || defined(__aarch64__) || \
     defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || \
     (defined(__x86_64__) && !defined(__ILP32__))
#   define BRRBITDEPTH_SYSTEM BRRBITDEPTH_64BIT
#  else
#   define BRRBITDEPTH_SYSTEM BRRBITDEPTH_32BIT
#  endif
# endif // !BRRBITDEPTH_SYSTEM

/* Enum representing different system bitdepths. */
typedef enum brrbitdepth {
	/* Value for a 64-bit system. */
	brrbitdepth_64bit = BRRBITDEPTH_64BIT,
	/* Value for a 32-bit system. */
	brrbitdepth_32bit = BRRBITDEPTH_32BIT,
	/* Value representing current system bitdepth.
	 * May be 'BRRBITDEPTH_64BIT', 'BRRBITDEPTH_32BIT', or 'BRRBITDEPTH_UNKNOWN'. */
	brrbitdepth_system = BRRBITDEPTH_SYSTEM,
} brrbitdepthT;
#endif /* !BRRTOOLS_NO_BITDEPTH */

/* ---------- Endianness detection */
#if !defined(BRRTOOLS_NO_ENDIAN)
# define BRRENDIAN_LITTLE  0x00010203UL
# define BRRENDIAN_BIG     0x03020100UL
# define BRRENDIAN_UNKNOWN 0x00000000UL

# if !defined(BRRENDIAN_SYSTEM) || \
     !(BRRENDIAN_SYSTEM == BRRENDIAN_LITTLE || BRRENDIAN_SYSTEM == BRRENDIAN_BIG)
#  if defined(BRRSYSTEM_BITDEPTH)
#   undef BRRSYSTEM_BITDEPTH
#  endif
#  if defined(__GNUC__)
#   if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#   elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#   else
#    error Unimplemented system endianness.
#   endif // __BYTE_ORDER__
#  elif defined(unix) || defined(__unix) || defined(__unix__) ||
        (defined(__APPLE__) && defined(__MACH__))
#   include <sys/param.h>
#   if __BYTE_ORDER == __BIG_ENDIAN
#    define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
#    define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#   else
#    error Unimplemented system endianness.
#   endif // __BYTE_ORDER
#  elif defined(WIN32) || defined(__WIN32) || defined(__WIN32__) // Windows
#   define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#  elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
     defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#   define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#  elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
     defined(_MIPSEL) || defined(__MIPSEL) || defined(__MISPEL__)
#   define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#  else
#   define BRRENDIAN_SYSTEM BRRENDIAN_UNKNOWN
#  endif // __GNUC__
# endif // !BRRENDIAN_SYSTEM

# if BRRENDIAN_SYSTEM == BRRENDIAN_BIG
#  define BRRSYSTEM_BIG_ENDIAN 1
# elif BRRENDIAN_SYSTEM == BRRENDIAN_LITTLE
#  define BRRSYSTEM_LITTLE_ENDIAN 1
# endif

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
#endif /* !BRRTOOLS_NO_ENDIAN */

#if defined(__cplusplus)
}
#endif

#endif /* BRRENDIAN_H */
