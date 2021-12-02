#ifndef BRRDEBUG_H
#define BRRDEBUG_H

#if defined(__cplusplus)
extern "C" {
#endif

/* Debugging defines */
#if defined(BRRTOOLS_DEBUG) && !defined(BRRTOOLS_NO_DEBUG)
/* https://github.com/nemequ/portable-snippets debug-trap.h */
# if defined(__has_builtin) && !defined(__ibmxl__)
#  if __has_builtin(__builtin_debugtrap)
#   define BRRDEBUG_BREAK() __builtin_debugtrap()
#  elif __has_builtin(__debugbreak)
#   define BRRDEBUG_BREAK() __debugbreak()
#  endif
# endif
# if !defined(BRRDEBUG_BREAK)
#  if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#   define BRRDEBUG_BREAK() __debugbreak()
#  elif defined(__ARMCC_VERSION)
#   define BRRDEBUG_BREAK() __breakpoint(42)
#  elif defined(__ibmxl__) || defined(__xlC__)
#   include <builtins.h>
#   define BRRDEBUG_BREAK() __trap(42)
#  elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#   define BRRDEBUG_BREAK() __builtin_trap()
#  else
#   include <signal.h>
#   if defined(SIGTRAP)
#    define BRRDEBUG_BREAK() raise(SIGTRAP)
#   else
#    define BRRDEBUG_BREAK() raise(SIGABRT)
#   endif
#  endif
# endif /* !BRRDEBUG_BREAK */
# define BRRDEBUG_HASBREAK
# if !defined(BRRDEBUG_ASSERTS)
#  define BRRDEBUG_ASSERTS
# endif
# if !defined(BRRDEBUG_TRACING)
#  define BRRDEBUG_TRACING
# endif

# if defined(BRRDEBUG_TRACING) && !defined(BRRDEBUG_NOTRACING)
#  if defined(BRRDEBUG_HASBREAK)
#   define BRRDEBUG_TRACE(...) do { BRRLOG_CRI(__VA_ARGS__); BRRDEBUG_BREAK(); } while (0)
#  else
#   include <stdlib.h>
#   define BRRDEBUG_TRACE(...) do { BRRLOG_CRI(__VA_ARGS__); abort(); } while (0)
#  endif
# else
#  define BRRDEBUG_TRACE(...)
# endif

# if defined(BRRDEBUG_ASSERTS) && !defined(BRRDEBUG_NOASSERTS)
#  if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
//  C11 has static asserts, though windows calls them differently.
#   if defined(__WIN32) || defined(__WIN32__)
#    define BRRDEBUG_STATIC_ASSERT(n) static_assert(n, "")
#    define BRRDEBUG_STATIC_ASSERTM(n, m) static_assert(n, m)
#   else
#    define BRRDEBUG_STATIC_ASSERT(n) _Static_assert(n, "")
#    define BRRDEBUG_STATIC_ASSERTM(n, m) _Static_assert(n, m)
#   endif // __WIN32
#  else
/* https://stackoverflow.com/a/64272144/13528679 */
#   define BRRDEBUG_STATIC_ASSERT(n) do { typedef char test[(n)?1:-1]; (void)(test*) "Assertion failed"; } while (0)
#   define BRRDEBUG_STATIC_ASSERTM(n, m) do { typedef char test[(n)?1:-1]; (void)(test*) m; } while (0)
#  endif // __STDC_VERSION__ >= 201112L
#  define BRRDEBUG_ASSERT(n) do { if (!(n)) { BRRDEBUG_TRACE("Assertion '%s' failed", #n); } } while (0)
#  define BRRDEBUG_ASSERTM(n, ...) do { { if (!(n)) { BRRDEBUG_TRACE(__VA_ARGS__); } } } while (0)
#  define BRRDEBUG_ASSERTI(n) BRRDEBUG_ASSERT(n)
#  define BRRDEBUG_ASSERTIM(n, ...) BRRDEBUG_ASSERTM(n, __VA_ARGS__)
# else
#  define BRRDEBUG_STATIC_ASSERT(n)
#  define BRRDEBUG_STATIC_ASSERTM(n, m)
#  define BRRDEBUG_ASSERT(n)
#  define BRRDEBUG_ASSERTM(n, ...)
#  define BRRDEBUG_ASSERTI(n) (n)
#  define BRRDEBUG_ASSERTIM(n, ...) (n)
# endif /* BRRDEBUG_Asserts && !BRRDEBUG_NoAsserts */
#endif /* !BRRTOOLS_NO_DEBUG */

#if defined(__cplusplus)
}
#endif

#endif /* BRRDEBUG_H */
