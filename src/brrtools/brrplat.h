#ifndef brrtools_brrplat_h
#define brrtools_brrplat_h

#ifdef __cplusplus
extern "C" {
#endif

/* Basic platform detection using macro checks
 *
 * Eventually, will have more specific stuff, but I've not got any specific ideas of what should be added for
 * now.
 * There are far too many systems/architectures/compilers to consider, and their combinations to test, for
 * one guy. */

#ifndef BRRPLAT_DISABLE
#if defined(__APPLE_) && defined(__MACH__)
# define brrplat_apple 1
#endif
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
# define brrplat_unix 1
# if defined(_POSIX_SOURCE) || defined(_POSIX_C_SOURCE) || defined(__CYGWIN__) /* TODO what other cygwin checks? */
#  define brrplat_posix 1
# else
#  include <unistd.h>
#  if defined(_POSIX_VERSION)
#   define brrplat_posix 1
#  endif
# endif
#elif defined(_WIN32)
# define brrplat_dos  1
#endif
#endif

#ifndef brrplat_unix
#define brrplat_unix  0
#endif
#ifndef brrplat_apple
#define brrplat_apple 0
#endif
#ifndef brrplat_dos
#define brrplat_dos   0
#endif
#ifndef brrplat_posix
#define brrplat_posix 0
#endif

#ifndef BRRPLAT_DISABLE
#if defined(_MSC_VER)
#define brrplat_msvc 1
#elif defined(__GNUC__)
#define brrplat_gnuc 1
#endif
#endif

#ifndef brrplat_msvc
#define brrplat_msvc 0
#endif
#ifndef brrplat_gnuc
#define brrplat_gnuc 0
#endif

#if defined(_ISOC11_SOURCE) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L)
# define brrplat_c11 1
#endif
#ifndef brrplat_c11
#define brrplat_c11 0
#endif

#if defined(_ISOC99_SOURCE) || (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L)
# define brrplat_c99 1
#endif
#ifndef brrplat_c99
#define brrplat_c99 brrplat_c11
#endif

#define brrplat_endian_unknown -1
#define brrplat_endian_little 0
#define brrplat_endian_big 1
#ifndef brrplat_endian
# if brrplat_gnuc
#  if (__BYTE_ORDER__) == (__ORDER_BIG_ENDIAN__)
#   define brrplat_endian brrplat_endian_big
#  elif (__BYTE_ORDER__) == (__ORDER_LITTLE_ENDIAN__)
#   define brrplat_endian brrplat_endian_little
#  else
#   define brrplat_endian brrplat_endian_unknown
#  endif
# else
    /* I don't really know about this one */
#   define brrplat_endian (*(short *)("\x12\x34") == 0x12)
# endif
#endif

extern const char *const brrplat_endian_str[3];

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrplat_h */
