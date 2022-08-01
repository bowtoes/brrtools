#ifndef BRRTOOLS_BRRAPI_H
#define BRRTOOLS_BRRAPI_H

#if defined(_WIN32)
# define BRRCALL __cdecl
# if !(defined(BRR_exports) || defined(BRR_imports)) // Static linkage
#  define BRRAPI
# elif defined(BRR_exports) && !defined(BRR_imports) // Compiling as dll
#  define BRRAPI __declspec(dllexport)
# else // Linking to dll
#  define BRRAPI __declspec(dllimport)
# endif
#else // Unix target
# define BRRAPI __attribute__((visibility("default")))
# define BRRCALL
#endif // BRRPLATFORMTYPE_WINDOWS

#endif /* BRRTOOLS_BRRAPI_H */
