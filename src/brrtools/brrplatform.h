#ifndef BRRTOOLS_PLATFORM_H
#define BRRTOOLS_PLATFORM_H

#define BRRPLATFORM_AIX         0
#define BRRPLATFORM_HPUX        1
#define BRRPLATFORM_Sun         2
#define BRRPLATFORM_MinGW64     3
#define BRRPLATFORM_Win64       4
#define BRRPLATFORM_Cygwin      5
#define BRRPLATFORM_MinGW       6
#define BRRPLATFORM_Win32       7
#define BRRPLATFORM_POSIXCygwin 8
#define BRRPLATFORM_DragonFly   9
#define BRRPLATFORM_FreeBSD     10
#define BRRPLATFORM_NetBSD      11
#define BRRPLATFORM_OpenBSD     12
#define BRRPLATFORM_Xcode       13
#define BRRPLATFORM_iOS         14
#define BRRPLATFORM_OSX         15
#define BRRPLATFORM_Linux       16

#define BRRPLATFORMNAME_AIX         "IBM AIX"
#define BRRPLATFORMNAME_HPUX        "HP-UX"
#define BRRPLATFORMNAME_Sun         "Solaris"
#define BRRPLATFORMNAME_MinGW64     "MinGW 64"
#define BRRPLATFORMNAME_Win64       "Windows x64"
#define BRRPLATFORMNAME_Cygwin      "Cygwin"
#define BRRPLATFORMNAME_MinGW       "MinGW"
#define BRRPLATFORMNAME_Win32       "Windows x86"
#define BRRPLATFORMNAME_POSIXCygwin "POSIX Cygwin"
#define BRRPLATFORMNAME_DragonFly   "DragonFly"
#define BRRPLATFORMNAME_FreeBSD     "FreeBSD"
#define BRRPLATFORMNAME_NetBSD      "NetBSD"
#define BRRPLATFORMNAME_OpenBSD     "OpenBSD"
#define BRRPLATFORMNAME_Xcode       "Xcode"
#define BRRPLATFORMNAME_iOS         "iOS"
#define BRRPLATFORMNAME_OSX         "OSX"
#define BRRPLATFORMNAME_Linux       "Linux"

/* ---------- Specific OS detection */
#if defined(_AIX)
# define BRRPLATFORMNAME BRRPLATFORMNAME_AIX
# define BRRPLATFORM BRRPLATFORM_AIX
#elif defined(hpux) || defined(__hpux)
# define BRRPLATFORMNAME BRRPLATFORMNAME_HPUX
# define BRRPLATFORM BRRPLATFORM_HPUX
#elif defined(__sun) && defined(__SVR4) /* does not detect old BSD-based sol */
# define BRRPLATFORMNAME BRRPLATFORMNAME_Sun
# define BRRPLATFORM BRRPLATFORM_Sun
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
# if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
#  if defined(__MINGW64__)
#   define BRRPLATFORMNAME BRRPLATFORMNAME_MinGW64
#   define BRRPLATFORM BRRPLATFORM_MinGW64
#  else
#   define BRRPLATFORMNAME BRRPLATFORMNAME_Win64
#   define BRRPLATFORM BRRPLATFORM_Win64
#  endif
# elif defined(__CYGWIN__) || defined(__CYGWIN32__)
#  define BRRPLATFORMNAME BRRPLATFORMNAME_Cygwin
#  define BRRPLATFORM BRRPLATFORM_Cygwin
# elif defined(__MINGW32__)
#  define BRRPLATFORMNAME BRRPLATFORMNAME_MinGW
#  define BRRPLATFORM BRRPLATFORM_MinGW
# else
#  define BRRPLATFORMNAME BRRPLATFORMNAME_Win32
#  define BRRPLATFORM BRRPLATFORM_Win32
# endif
/* posix cygwin */
#elif defined(__CYGWIN__) || defined(__CYGWIN32__)
# define BRRPLATFORMNAME BRRPLATFORMNAME_POSIXCygwin
# define BRRPLATFORM BRRPLATFORM_POSIXCygwin
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
# include <sys/param.h> /* For BSD */
# if defined(BSD) || defined(__bsdi__)
#  if defined(__DragonFly__)
#   define BRRPLATFORMNAME BRRPLATFORMNAME_DragonFly
#   define BRRPLATFORM BRRPLATFORM_DragonFly
#  elif defined(__FreeBSD__)
#   define BRRPLATFORMNAME BRRPLATFORMNAME_FreeBSD
#   define BRRPLATFORM BRRPLATFORM_FreeBSD
#  elif defined(__NetBSD__)
#   define BRRPLATFORMNAME BRRPLATFORMNAME_NetBSD
#   define BRRPLATFORM BRRPLATFORM_NetBSD
#  elif defined(__OpenBSD__)
#   define BRRPLATFORMNAME BRRPLATFORMNAME_OpenBSD
#   define BRRPLATFORM BRRPLATFORM_OpenBSD
#  elif defined(__APPLE__) && defined(__MACH__)
#   include <TargetConditionals.h>
#   define BRRPLATFORMTYPE_Apple
#   if TARGET_IPHONE_SIMULATOR == 1
#    define BRRPLATFORMNAME BRRPLATFORMNAME_Xcode
#    define BRRPLATFORM BRRPLATFORM_Xcode
#   elif TARGET_OS_IPHONE == 1
#    define BRRPLATFORMNAME BRRPLATFORMNAME_iOS
#    define BRRPLATFORM BRRPLATFORM_iOS
#   elif TARGET_OS_MAC == 1
#    define BRRPLATFORMNAME BRRPLATFORMNAME_OSX
#    define BRRPLATFORM BRRPLATFORM_OSX
#   else
#    error Huh?
#   endif
#  else
#   error Unknown BSD distribution
#  endif
// end (BSD || __bsdi__)
# elif defined(linux) || defined(__linux) || defined(__linux__)
#  define BRRPLATFORMNAME BRRPLATFORMNAME_Linux
#  define BRRPLATFORM BRRPLATFORM_Linux
# else
#  error Unknown UNIX system
# endif
#endif

/* ---------- Broad environment detection */
#if defined(BSD) || defined(__bsdi__)
# define BRRPLATFORMTYPE_BSD 1
#endif
#if defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
# define BRRPLATFORMTYPE_unix 1
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)) && !(defined(__CYGWIN__) || defined(__CYGWIN32__))
# define BRRPLATFORMTYPE_Windows 1
# if defined(__MINGW32__)
#  define BRRPLATFORMTYPE_MinGW 1
# endif
#endif
#if defined(__CYGWIN__) || defined(__CYGWIN32__)
# define BRRPLATFORMTYPE_Cygwin 1
#endif

#endif /* BRRTOOLS_PLATFORM_H */
