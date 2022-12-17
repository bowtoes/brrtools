#ifndef brrtools_brrdbg_h
#define brrtools_brrdbg_h

#ifdef __cplusplus
extern "C" {
#endif

#if defined(brrtools_debug) /* Enable all debugging if library debugging is enabled */
# ifdef BRRDBG_NO_BREAK
#  undef BRRDBG_NO_BREAK
# endif
# ifdef BRRDBG_NO_TRACE
#  undef BRRDBG_NO_TRACE
# endif
# ifdef BRRDBG_NO_ASSERT
#  undef BRRDBG_NO_ASSERT
# endif
#elif !defined(BRRDBG) /* Disable all debugging unless BRRDBG is explicitly defined */
# ifndef BRRDBG_NO_BREAK
#  define BRRBDG_NO_BREAK
# endif
# ifndef BRRDBG_NO_TRACE
#  define BRRDBG_NO_TRACE
# endif
# ifndef BRRDBG_NO_ASSERT
#  define BRRDBG_NO_ASSERT
# endif
#endif

#define _brrdbg_int_str0(_x_) #_x_
#define _brrdbg_int_str(_x_) _brrdbg_int_str0(_x_)

/* https://github.com/nemequ/portable-snippets debug-trap.h */
#ifdef BRRDBG_NO_BREAK
# define BRRDBG_HAS_BREAK 0
# define brrdbg_break()
#else
# define BRRDBG_HAS_BREAK 1
# if defined(__has_builtin) && !defined(__ibmxl__)
#  if __has_builtin(__builtin_debugtrap)
#   define brrdbg_break() __builtin_debugtrap()
#  elif __has_builtin(__debugbreak)
#   define brrdbg_break() __debugbreak()
#  endif
# endif
# if !defined(brrdbg_break)
#  if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#   define brrdbg_break() __debugbreak()
#  elif defined(__ARMCC_VERSION)
#   define brrdbg_break() __breakpoint(42)
#  elif defined(__ibmxl__) || defined(__xlC__)
#   include <builtins.h>
#   define brrdbg_break() __trap(42)
#  elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#   define brrdbg_break() __builtin_trap()
#  else
#   include <signal.h>
#   if defined(SIGTRAP)
#    define brrdbg_break() raise(SIGTRAP)
#   else
#    define brrdbg_break() raise(SIGABRT)
#   endif
#  endif
# endif
#endif

#ifdef BRRDBG_NO_TRACE
# define BRRDBG_HAS_TRACE 0
# define brrdbg_abort()
#else
# if BRRDBG_HAS_BREAK
# define BRRDBG_HAS_TRACE 1
#  define brrdbg_abort() brrdbg_break()
# else
#  include <stdlib.h>
#  define brrdbg_abort() abort()
# endif
# include <stdio.h>
# define brrdbg_tracem(...) do { fprintf(stderr, __VA_ARGS__); brrdbg_abort(); } while (0)
#endif
#define brrdbg_trace(...) brrdbg_tracem("TRACE in '"__FILE__"' @ "_brrdbg_int_str(__LINE__)": "__VA_ARGS__)

#ifdef BRRDBG_NO_ASSERT
# define BRRDBG_HAS_ASSERT 0
# define brrdbg_assertm(_x_, ...)
# define brrdbg_static_assertm(_x_, _m_)
#else
# define BRRDBG_HAS_ASSERT 1
# if BRRDBG_HAS_TRACE
#  define brrdbg_assertm(_x_, ...) do { if (!(_x_)) { brrdbg_trace(__VA_ARGS__); } } while (0)
# else
#  define brrdbg_assertm(_x_, ...)
# endif
# if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 201112L /* C11 brought builtin static_assert */
/* https://stackoverflow.com/a/64272144/13528679 */
#  define brrdbg_static_assertm(_x_, _m_) do { char test[sizeof(_m_)*((_x_)?1:-1)] = _m_; } while (0)
# else
#  if defined(_WIN32)
#   define brrdbg_static_assertm(_x_, _m_) static_assert(_x_, _m_)
#  else
#   define brrdbg_static_assertm(_x_, _m_) _Static_assert(_x_, _m_)
#  endif
# endif
#endif
#define brrdbg_assert(_x_) brrdbg_assertm(_x_, "Assertion '%s' failed", _brrdbg_int_str(_x_))
#define brrdbg_static_assert(_x_) brrdbg_static_assertm(_x_, "")

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrdbg_h */
