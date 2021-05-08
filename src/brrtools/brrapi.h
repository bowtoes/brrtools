#ifndef BRRTOOLS_BRRAPI_H
#define BRRTOOLS_BRRAPI_H

#if defined(_WIN32)
# if defined(BRRTOOLS_SHARED)
#  if defined(BRRTOOLS_EXPORTS)
#   define BRRAPI __declspec(dllexport)
#  else
#   define BRRAPI __declspec(dllimport)
#  endif // BRRTOOLS_EXPORT
# else // Static library
#  define BRRAPI
# endif // !BRRTOOLS_STATIC
# define BRRCALL __cdecl
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
