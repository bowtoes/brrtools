#ifndef BRRTOOLS_PLATFORM_H
#define BRRTOOLS_PLATFORM_H

#define BRRPLATFORM_AIX         0
#define BRRPLATFORM_HPUX        1
#define BRRPLATFORM_Sun         2
#define BRRPLATFORM_MinGW64     3
#define BRRPLATFORM_Win64       4
#define BRRPLATFORM_Cygwin      5
#define BRRPLATFORM_MinGW       6
#define BRRPLATFORM_Win86       7
#define BRRPLATFORM_POSIXCygwin 8
#define BRRPLATFORM_DragonFly   9
#define BRRPLATFORM_FreeBSD     10
#define BRRPLATFORM_NetBSD      11
#define BRRPLATFORM_OpenBSD     12
#define BRRPLATFORM_Xcode       13
#define BRRPLATFORM_iOS         14
#define BRRPLATFORM_OSX         15
#define BRRPLATFORM_Linux       16

/* ---------- Specific OS detection */
#if defined(_AIX)
# define BRRPLATFORMNAME "IBM AIX"
# define BRRPLATFORM BRRPLATFORM_AIX
#elif defined(hpux) || defined(__hpux)
# define BRRPLATFORMNAME "HP-UX"
# define BRRPLATFORM BRRPLATFORM_HPUX
#elif defined(__sun) && defined(__SVR4) /* does not detect old BSD-based sol */
# define BRRPLATFORMNAME "Solaris"
# define BRRPLATFORM BRRPLATFORM_Sun
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
# if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
#  if defined(__MINGw64__)
#   define BRRPLATFORMNAME "MinGW 64"
#   define BRRPLATFORM BRRPLATFORM_MinGW64
#  else
#   define BRRPLATFORMNAME "Windows x64"
#   define BRRPLATFORM BRRPLATFORM_Win64
#  endif
# elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#  define BRRPLATFORMNAME "Cygwin"
#  define BRRPLATFORM BRRPLATFORM_Cygwin
# elif defined(__MINGW32__)
#  define BRRPLATFORMNAME "MinGW"
#  define BRRPLATFORM BRRPLATFORM_MinGW
# else
#  define BRRPLATFORMNAME "Windows x86"
#  define BRRPLATFORM BRRPLATFORM_Win86
# endif
/* posix cygwin */
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
# define BRRPLATFORMNAME "POSIX Cygwin"
# define BRRPLATFORM BRRPLATFORM_POSIXCygwin
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
// 'sys/param.h' for 'BSD'
# include <sys/param.h>
# if defined(BSD) || defined(__bsdi__)
#  if defined(__DragonFly__)
#   define BRRPLATFORMNAME "DragonFly"
#   define BRRPLATFORM BRRPLATFORM_DragonFly
#  elif defined(__FreeBSD__)
#   define BRRPLATFORMNAME "FreeBSD"
#   define BRRPLATFORM BRRPLATFORM_FreeBSD
#  elif defined(__NetBSD__)
#   define BRRPLATFORMNAME "NetBSD"
#   define BRRPLATFORM BRRPLATFORM_NetBSD
#  elif defined(__OpenBSD__)
#   define BRRPLATFORMNAME "OpenBSD"
#   define BRRPLATFORM BRRPLATFORM_OpenBSD
#  elif defined(__APPLE__) && defined(__MACH__)
#   include <TargetConditionals.h>
#   define BRRPLATFORMTYPE_APPLE
#   define BRRPLATFORMTYPE_UNIX /* Apple does not define the unix's */
#   if TARGET_IPHONE_SIMULATOR == 1
#    define BRRPLATFORMNAME "Xcode"
#    define BRRPLATFORM BRRPLATFORM_Xcode
#   elif TARGET_OS_IPHONE == 1
#    define BRRPLATFORMNAME "iOS"
#    define BRRPLATFORM BRRPLATFORM_iOS
#   elif TARGET_OS_MAC == 1
#    define BRRPLATFORMNAME "OSX"
#    define BRRPLATFORM BRRPLATFORM_OSX
#   else
#    error Huh?
#   endif
#  else
#   error Unknown BSD distribution
#  endif
// end (BSD || __bsdi__)
# elif defined(linux) || defined(__linux) || defined(__linux__)
#  define BRRPLATFORMNAME "Linux"
#  define BRRPLATFORM BRRPLATFORM_Linux
# else
#  error Unknown UNIX system
# endif
#endif

/* ---------- Broad environment detection */
#if defined(BSD) || defined(__bsdi__)
# define BRRPLATFORMTYPE_BSD
#endif
#if defined(unix) || defined(__unix) || defined(__unix__)
# define BRRPLATFORMTYPE_UNIX
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)) && !(defined(__CYGWIN__) || defined(__CYGWIN32__))
# define BRRPLATFORMTYPE_WINDOWS
# if defined(__MINGW32__)
#  define BRRPLATFORMTYPE_MINGW
# endif
#endif
#if defined(__CYGWIN__) || defined(__CYGWIN32__)
# define BRRPLATFORMTYPE_CYGWIN
#endif

/* Get POSIX information */
#if defined(BRRPLATFORMTYPE_UNIX) && !defined(BRRPLATFORMTYPE_WINDOWS)
# include <unistd.h>
# if defined(_POSIX_VERSION)
#  define BRRPLATFORMTYPE_POSIX
# endif
#endif

#if defined(__GNUC__)
# define BRRPLATFORMTYPE_GNU
#endif

/* ---------- Detect system endianness */
#define BRRLITTLE_ENDIAN  0x00010203UL
#define BRRBIG_ENDIAN     0x03020100UL
#define BRRUNKNOWN_ENDIAN 0x00000000UL

#if !defined(BRRSYSTEM_ENDIAN)
# if defined(__GNUC__)
#  if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define BRRSYSTEM_ENDIAN BRRBIG_ENDIAN
#  elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define BRRSYSTEM_ENDIAN BRRLITTLE_ENDIAN
#  else
#   error Unimplemented system endianness.
#  endif // __BYTE_ORDER__
# elif defined(BRRPLATFORMTYPE_UNIX)
#  include <sys/param.h>
#  if __BYTE_ORDER == __BIG_ENDIAN
#   define BRRSYSTEM_ENDIAN BRRBIG_ENDIAN
#  elif __BYTE_ORDER == __LITTLE_ENDIAN
#   define BRRSYSTEM_ENDIAN BRRLITTLE_ENDIAN
#  else
#   error Unimplemented system endianness.
#  endif // __BYTE_ORDER
# elif defined(BRRPLATFORMTYPE_WINDOWS)
// can windows be big endian?
#  define BRRSYSTEM_ENDIAN BRRLITTLE_ENDIAN
# elif defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)
#  define BRRSYSTEM_ENDIAN BRRBIG_ENDIAN
# elif defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MISPEL__)
#  define BRRSYSTEM_ENDIAN BRRLITTLE_ENDIAN
# else
#  define BRRSYSTEM_ENDIAN BRRUNKNOWN_ENDIAN
# endif // __GNUC__
#endif // !BRRSYSTEM_ENDIAN

#if BRRSYSTEM_ENDIAN == BRRBIG_ENDIAN
# define BRRSYSTEM_BIG_ENDIAN 1
#elif BRRSYSTEM_ENDIAN == BRRLITTLE_ENDIAN
# define BRRSYSTEM_LITTLE_ENDIAN 1
#endif

/* Enum representing different types of endianness. */
typedef enum brrendian {
	/* Value representing little-endian. */
	brrendian_little = BRRLITTLE_ENDIAN,
	/* Value representing big-endian. */
	brrendian_big = BRRBIG_ENDIAN,
	/* Value representing endianness of system.
	 * May be 'BRRLITTLE_ENDIAN', 'BRRBIG_ENDIAN', or 'BRRUNKNOWN_ENDIAN'. */
	brrendian_system = BRRSYSTEM_ENDIAN,
} brrendianT;

/* ---------- Detect system bitdepth */
#define BRRBITDEPTH_64BIT   64
#define BRRBITDEPTH_32BIT   32
#define BRRBITDEPTH_UNKNOWN 0

#if !defined(BRRSYSTEM_BITDEPTH)
# if defined(__LP64) || defined(__LP64__) || defined(_WIN64) || \
    defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM64) || defined(_M_AMD64) || \
    defined(__ia64) || defined(__aarch64__) || \
    defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || \
    (defined(__x86_64__) && !defined(__ILP32__))
#  define BRRSYSTEM_BITDEPTH BRRBITDEPTH_64BIT
# else
#  define BRRSYSTEM_BITDEPTH BRRBITDEPTH_32BIT
# endif
#else
# define BRRSYSTEM_BITDEPTH BRRSYSTEM_BITDEPTH_UNKNOWN
#endif // !BRRSYSTEM_BITDEPTH

/* Enum representing different system bitdepths. */
typedef enum brrbitdepth {
	/* Value for a 64-bit system. */
	brrbitdepth_64bit = BRRBITDEPTH_64BIT,
	/* Value for a 32-bit system. */
	brrbitdepth_32bit = BRRBITDEPTH_32BIT,
	/* Value representing current system bitdepth.
	 * May be 'BRRBITDEPTH_64BIT', 'BRRBITDEPTH_32BIT', or 'BRRBITDEPTH_UNKNOWN'. */
	brrbitdepth_system = BRRSYSTEM_BITDEPTH,
} brrbitdepthT;

#endif /* BRRTOOLS_PLATFORM_H */
