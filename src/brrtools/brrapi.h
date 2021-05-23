#ifndef BRRTOOLS_BRRAPI_H
#define BRRTOOLS_BRRAPI_H

#if defined(_WIN32)
# define BRRCALL __cdecl
# if defined(BRRTOOLS_EXPORTS) || defined(BRRTOOLS_IMPORTS)
#  if defined(BRRTOOLS_EXPORTS) && !defined(BRRTOOLS_IMPORTS)
#   define BRRAPI __declspec(dllexport)
#  else
#   define BRRAPI __declspec(dllimport)
#  endif // BRRTOOLS_EXPORTS && !BRRTOOLS_IMPORTS
# else // Static library
#  define BRRAPI
# endif // BRRTOOLS_EXPORTS || BRRTOOLS_IMPORTS
#else // UNIX target
# define BRRAPI
# define BRRCALL
#endif // BRRPLATFORMTYPE_WINDOWS

#if defined(__cplusplus)
# define BRRCPPSTART extern "C" {
# define BRRCPPEND }
#else
# define BRRCPPSTART
# define BRRCPPEND
#endif // __cplusplus

#endif /* BRRTOOLS_BRRAPI_H */
