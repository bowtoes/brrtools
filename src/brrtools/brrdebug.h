/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef BRRTOOLS_DEBUG_H
#define BRRTOOLS_DEBUG_H

/* https://github.com/nemequ/portable-snippets debug-trap.h */
#if defined(__has_builtin) && !defined(__ibmxl__)
# if __has_builtin(__builtin_debugtrap)
#  define BRRDEBUG_BREAK() __builtin_debugtrap()
# elif __has_builtin(__debugbreak)
#  define BRRDEBUG_BREAK() __debugbreak()
# endif
#endif
#if !defined(BRRDEBUG_BREAK)
# if defined(_MSC_VER) || defined(__INTEL_COMPILER)
#  define BRRDEBUG_BREAK() __debugbreak()
# elif defined(__ARMCC_VERSION)
#  define BRRDEBUG_BREAK() __breakpoint(42)
# elif defined(__ibmxl__) || defined(__xlC__)
#  include <builtins.h>
#  define BRRDEBUG_BREAK() __trap(42)
# elif defined(__STDC_HOSTED__) && (__STDC_HOSTED__ == 0) && defined(__GNUC__)
#  define BRRDEBUG_BREAK() __builtin_trap()
# else
#  include <signal.h>
#  if defined(SIGTRAP)
#   define BRRDEBUG_BREAK() raise(SIGTRAP)
#  else
#   define BRRDEBUG_BREAK() raise(SIGABRT)
#  endif
# endif
#endif /* !BRRDEBUG_BREAK */
#define BRRDEBUG_HASBREAK
#if !defined(BRRDEBUG_ASSERTS)
# define BRRDEBUG_ASSERTS
#endif
#if !defined(BRRDEBUG_TRACING)
# define BRRDEBUG_TRACING
#endif

#if defined(BRRDEBUG_TRACING) && !defined(BRRDEBUG_NOTRACING)
# if defined(BRRDEBUG_HASBREAK)
#  define BRRDEBUG_TRACE(...) { BRRLOG_CRI(__VA_ARGS__); BRRDEBUG_BREAK(); }
# else
#  include <stdlib.h>
#  define BRRDEBUG_TRACE(...) { BRRLOG_CRI(__VA_ARGS__); abort(); }
# endif
#else
# define BRRDEBUG_TRACE(...)
#endif

#if defined(BRRDEBUG_ASSERTS) && !defined(BRRDEBUG_NOASSERTS)
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  if defined(__WIN32) || defined(__WIN32__)
#   define BRRDEBUG_STATIC_ASSERT(n) static_assert(n, "")
#   define BRRDEBUG_STATIC_ASSERTM(n, m) static_assert(n, m)
#  else
#   define BRRDEBUG_STATIC_ASSERT(n) _Static_assert(n, "")
#   define BRRDEBUG_STATIC_ASSERTM(n, m) _Static_assert(n, m)
#  endif // __WIN32
# else
/* https://stackoverflow.com/a/64272144/13528679 */
#  define BRRDEBUG_STATIC_ASSERT(n) { typedef char test[(n)?1:-1]; (void)(test*) "Assertion failed"; }
#  define BRRDEBUG_STATIC_ASSERTM(n, m) { typedef char test[(n)?1:-1]; (void)(test*) m; }
# endif // __STDC_VERSION__ >= 201112L
# define BRRDEBUG_ASSERT(n) { if (!(n)) { BRRDEBUG_TRACE("Assertion '%s' failed", #n); } }
# define BRRDEBUG_ASSERTM(n, ...) { if (!(n)) { BRRDEBUG_TRACE(__VA_ARGS__); } }
# define BRRDEBUG_ASSERTI(n) BRRDEBUG_ASSERT(n)
# define BRRDEBUG_ASSERTIM(n, ...) BRRDEBUG_ASSERTM(n, __VA_ARGS__)
#else
# define BRRDEBUG_STATIC_ASSERT(n)
# define BRRDEBUG_STATIC_ASSERTM(n, m)
# define BRRDEBUG_ASSERT(n)
# define BRRDEBUG_ASSERTM(n, ...)
# define BRRDEBUG_ASSERTI(n) (n)
# define BRRDEBUG_ASSERTIM(n, ...) (n)
#endif /* BRRDEBUG_Asserts && !BRRDEBUG_NoAsserts */

#endif /* BRRTOOLS_DEBUG_H */
