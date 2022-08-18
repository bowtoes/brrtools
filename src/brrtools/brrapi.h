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

#if defined(_MSC_VER)
# define BRR_inline inline __forceinline
#elif defined(__GNUC__) || (defined(__clang__) && __has_attribute(__always_inline__))
# define BRR_inline inline __attribute__((__always_inline__))
#else
# define BRR_inline inline
#endif

#ifdef __cplusplus
# define BRR_cppstart extern "C" {
# define BRR_cppend }
#else
# define BRR_cppstart
# define BRR_cppend
#endif

BRR_cppstart

#define BRRAPI_E_SUCCESS (0x00)
#define BRRAPI_E_MEMERR  (0x10)
#define BRRAPI_E_ARGERR  (0x20)
#define BRRAPI_E_ARRFULL (0x21)

BRRAPI void BRRCALL
brrapi_sete(int e);

BRRAPI int BRRCALL
brrapi_gete(void);

BRRAPI const char *BRRCALL
brrapi_err(void);

BRR_cppend

#endif /* BRRTOOLS_BRRAPI_H */
