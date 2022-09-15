#ifndef brrtools_brrapi_h
#define brrtools_brrapi_h

#ifdef __cplusplus
extern "C" {
#endif

#include <brrtools/brrplat.h>
/* NOTE calling convention will be different for different architectures, something to consider in future */

#if brrplat_dos || brrplat_msvc
# define BRRCALL __cdecl
# if !(defined(BRR_exports) || defined(BRR_imports))
// Static linkage
# elif defined(BRR_exports) && !defined(BRR_imports)
// Compiling as dll
#  define BRRAPI __declspec(dllexport)
# else
// Linking to dll
#  define BRRAPI __declspec(dllimport)
# endif
#elif brrplat_gnuc
# if defined(__has_attribute)
#  if __has_attribute(cdecl)
#   define BRRCALL
#  endif
#  if __has_attribute(visibility)
#   define BRRAPI __attribute__((visibility("default")))
#  endif
# endif
#endif
#ifndef BRRAPI
#define BRRAPI extern
#endif
#ifndef BRRCALL
#define BRRCALL
#endif

#if brrplat_msvc
# define BRR_inline inline __forceinline
#elif brrplat_gnuc
# if defined(__has_attribute)
#  if __has_attribute(__always_inline__)
#   define BRR_inline inline __attribute__((__always_inline__))
#  endif
# endif
#endif
#ifndef BRR_inline
#define BRR_inline inline
#endif

#define _brrapi_e(_X_)\
	_X_(SUCCESS, 0x00, "Success")\
	_X_(MEMERR,  0x10, "Memory Allocation Failure")\
	_X_(ARGERR,  0x20, "Invalid Argument(s)")\
	_X_(ARRFULL, 0x21, "Array is Full")\
	_X_(LIBC,    0x30, "C Library Errored")\
	_X_(OSERR,   0x31, "Operating System specific error")\

#define _brrapi_e_op(_n_, _i_, _D_) BRRAPI_E_##_n_ = _i_,
typedef enum brrapi_err { _brrapi_e(_brrapi_e_op) } brrapi_err_t;
#undef _brrapi_e_op

BRRAPI void BRRCALL
brrapi_sete(int e);

BRRAPI int BRRCALL
brrapi_gete(void);

BRRAPI const char *BRRCALL
brrapi_err(void);

#ifndef _brrapi_keep_gens
#undef _brrapi_e
#endif

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrapi_h */
