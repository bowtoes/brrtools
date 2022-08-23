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

#define _brrapi_e(_X_)\
	_X_(SUCCESS, 0x00, "Success")\
	_X_(MEMERR,  0x10, "Memory Allocation Failure")\
	_X_(ARGERR,  0x20, "Invalid Argument(s)")\
	_X_(ARRFULL, 0x21, "Array is Full")\
	_X_(LIBC,    0x30, "C Library Errored")\

#define _brrapi_e_op(_n_, _i_, _D_) BRRAPI_E_##_n_ = _i_,
typedef enum brrapi_err { _brrapi_e(_brrapi_e_op) } brrapi_err_t;
#undef _brrapi_e_op

BRRAPI void BRRCALL
brrapi_sete(int e);

BRRAPI int BRRCALL
brrapi_gete(void);

BRRAPI const char *BRRCALL
brrapi_err(void);

BRR_cppend

#ifndef _brrapi_keep_gens
#undef _brrapi_e
#endif

#endif /* BRRTOOLS_BRRAPI_H */
