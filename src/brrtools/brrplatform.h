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

#ifndef BRRTOOLS_PLATFORM_H
#define BRRTOOLS_PLATFORM_H

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
#  elif defined(BRRPLATFORMTYPE_UNIX)
#   include <sys/param.h>
#   if __BYTE_ORDER == __BIG_ENDIAN
#    define BRRENDIAN_SYSTEM BRRENDIAN_BIG
#   elif __BYTE_ORDER == __LITTLE_ENDIAN
#    define BRRENDIAN_SYSTEM BRRENDIAN_LITTLE
#   else
#    error Unimplemented system endianness.
#   endif // __BYTE_ORDER
#  elif defined(BRRPLATFORMTYPE_WINDOWS)
/ / can windows be big endian?
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
# include <sys/param.h> /* For BSD */
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

#if !defined(BRRTOOLS_NO_DEBUG)
/* https://github.com/nemequ/portable-snippets debug-trap.h */
# if defined(__has_builtin) && !defined(__ibmxl__)
#  if __has_builtin(__builtin_debugtrap)
#   define BRRDEBUG_BREAK() __builtin_debugtrap()
#  elif __has_builtin(__debugbreak)
#   define BRRDEBUG_BREAK() __debugbreak()
#  endif
# endif
# if !defined(BRRDEBUG_BREAK)
#  if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#   define BRRDEBUG_BREAK() __debugbreak()
#  elif defined(__ARMCC_VERSION)
#   define BRRDEBUG_BREAK() __breakpoint(42)
#  elif defined(__ibmxl__) || defined(__xlC__)
#   include <builtins.h>
#   define BRRDEBUG_BREAK() __trap(42)
#  elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#   define BRRDEBUG_BREAK() __builtin_trap()
#  else
#   include <signal.h>
#   if defined(SIGTRAP)
#    define BRRDEBUG_BREAK() raise(SIGTRAP)
#   else
#    define BRRDEBUG_BREAK() raise(SIGABRT)
#   endif
#  endif
# endif /* !BRRDEBUG_BREAK */
# define BRRDEBUG_HASBREAK
# if !defined(BRRDEBUG_ASSERTS)
#  define BRRDEBUG_ASSERTS
# endif
# if !defined(BRRDEBUG_TRACING)
#  define BRRDEBUG_TRACING
# endif

# if defined(BRRDEBUG_TRACING) && !defined(BRRDEBUG_NOTRACING)
#  if defined(BRRDEBUG_HASBREAK)
#   define BRRDEBUG_TRACE(...) { BRRLOG_CRI(__VA_ARGS__); BRRDEBUG_BREAK(); }
#  else
#   include <stdlib.h>
#   define BRRDEBUG_TRACE(...) { BRRLOG_CRI(__VA_ARGS__); abort(); }
#  endif
# else
#  define BRRDEBUG_TRACE(...)
# endif

# if defined(BRRDEBUG_ASSERTS) && !defined(BRRDEBUG_NOASSERTS)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#   if defined(__WIN32) || defined(__WIN32__)
#    define BRRDEBUG_STATIC_ASSERT(n) static_assert(n, "")
#    define BRRDEBUG_STATIC_ASSERTM(n, m) static_assert(n, m)
#   else
#    define BRRDEBUG_STATIC_ASSERT(n) _Static_assert(n, "")
#    define BRRDEBUG_STATIC_ASSERTM(n, m) _Static_assert(n, m)
#   endif // __WIN32
#  else
/* https://stackoverflow.com/a/64272144/13528679 */
#   define BRRDEBUG_STATIC_ASSERT(n) { typedef char test[(n)?1:-1]; (void)(test*) "Assertion failed"; }
#   define BRRDEBUG_STATIC_ASSERTM(n, m) { typedef char test[(n)?1:-1]; (void)(test*) m; }
#  endif // __STDC_VERSION__ >= 201112L
#  define BRRDEBUG_ASSERT(n) { if (!(n)) { BRRDEBUG_TRACE("Assertion '%s' failed", #n); } }
#  define BRRDEBUG_ASSERTM(n, ...) { if (!(n)) { BRRDEBUG_TRACE(__VA_ARGS__); } }
#  define BRRDEBUG_ASSERTI(n) BRRDEBUG_ASSERT(n)
#  define BRRDEBUG_ASSERTIM(n, ...) BRRDEBUG_ASSERTM(n, __VA_ARGS__)
# else
#  define BRRDEBUG_STATIC_ASSERT(n)
#  define BRRDEBUG_STATIC_ASSERTM(n, m)
#  define BRRDEBUG_ASSERT(n)
#  define BRRDEBUG_ASSERTM(n, ...)
#  define BRRDEBUG_ASSERTI(n) (n)
#  define BRRDEBUG_ASSERTIM(n, ...) (n)
# endif /* BRRDEBUG_Asserts && !BRRDEBUG_NoAsserts */
#endif /* !BRRTOOLS_NO_DEBUG */

#endif /* BRRTOOLS_PLATFORM_H */
