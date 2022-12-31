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
	_X_(SUCCESS,      "Success")\
	_X_(MEMERR,       "Memory Allocation Failure")\
	_X_(ARGERR,       "Invalid Argument(s)")\
	_X_(ARRFULL,      "Array is Full")\
	_X_(LIBC,         "libc Error")\
	_X_(OSERR,        "OS Error")\
	_X_(INVALID_CODE, "brrapi Invalid Code")\

#define _brrapi_e_op(_n_, _D_) BRRAPI_E_##_n_,
typedef enum brrapi_err { _brrapi_e(_brrapi_e_op) } brrapi_err_t;
#undef _brrapi_e_op

BRRAPI void BRRCALL
brrapi_error(const char *const file, const char *const function, int line, int code, const char *const message, ...);

#define brrapi_error(_code_, ...) brrapi_error(__FILE__, __func__, __LINE__, _code_, __VA_ARGS__)
#define brrapi_error_update() brrapi_error_set_position(__FILE__, __func__, __LINE__)

BRRAPI int BRRCALL
brrapi_error_code(void);

BRRAPI const char *BRRCALL
brrapi_error_name(int code);

BRRAPI char *BRRCALL
brrapi_error_message(char *const dst, int max_size);

BRRAPI void BRRCALL
brrapi_error_set_position(const char *const file, const char *const function, int line);

#ifndef _brrapi_keep_gens
#undef _brrapi_e
#endif

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrapi_h */
