# brrtools v0.0.2
A personal C11 library with the goal to have a single place for multiple cross-platform, miscellaneous
utilities that I find generally useful.

Has absolutely no relation to [BRRtools][otherbrr] (SNES audio format tooling).
## Table of Contents
1. [Goal](#goal)
  1. [Modules](#modules)
1. [Build](#build)
1. [Todo](#todo)

## Goal
[ToC](#table-of-contents)  

This library is a collection of modules with different, specific purposes, implemented with attention paid to
different platform OS/architecture/... specifics.  
Each time I start a new project, I don't want to have to worry about reimplementing the same basic
functionality for some vast number of different architectures just to have a modicum of portability; this
library is my attempt at that.

### Modules
[ToC](#table-of-contents)  

There are various modules in this library, some small, others comparatively large; this is a list of them,
their purpose/functionality:
#### Main modules
 * `brrlog.h`: A logging utility, with built-in stylability configuration. TODO a bit of good documentation.
 * `brrstr.h`: A 'light' abstraction on `char *`s, that provides basic length-checking functionality and
   manipulations for now; will have more as I deem it convenient.

#### Smaller/Miscellaneous Modules
Ordered slightly by dependencies:
 * `brrplat.h`: A collection of preprocessor conditionals that define various platform booleans, such as:
   * `brrplat_dos`: 1 when compiling for Windows, 0 otherwise.
   * `brrplat_unix`: 1 when compiling for unix, 0 otherwise.
   * `brrplat_posix`: 1 when compiling for a system with POSIX conformity, 0 otherwise.
   * `brrplat_apple`: 1 when compiling on an Apple product, 0 otherwise.
   * `brrplat_msvc`: 1 when using a Microsoft Visual-C compatible compiler, 0 otherwise.
   * `brrplat_gnuc`: 1 when using a GCC compatible compiler, 0 otherwise.
   * `brrplat_c99`: 1 when the current C-standard is at least C99, 0 otherwise.
   * `brrplat_c11`: 1 when the current C-standard is at least C11, 0 otherwise.

   No dependencies.
 * `brrapi.h`: Contains some `#define`s used throughout the library to declare all its functions. Declares the
   following functions:
   * `void brrapi_sete(int)`: Sets the current library error code; used by library functions (eventually, only `brrlog`
     for now).
   * `int brrapi_gete()`: Returns the current library error code.
   * `char *brrapi_err()`: Returns a string representing the current library error code.

   See `brrapi.h` for a list of defined error codes (within the `_brrapi_e` macro).
   Uses `brrplat.h`.
 * `brrdbg.h`: A collection of various macro `#define`s intended to be used to help when debugging a program:
   * `brrdbg_break()`: If `BRRDBG_NO_BREAK` is not `#define`d, `brrdbg_break()` resolves to a debug break at
     the invocation, if breaks are supported, otherwise an appropriate signal is raised at runtime.  
   * `brrdbg_abort()` & `brrdbg_trace(...)`: If `BRRDBG_NO_TRACE` is not `#define`d,`brrdbg_abort()` either
     causes a `brrdbg_break()`, if it's available, or calls `abort()` at its location. `brrdbg_trace(...)`
     uses this to halt the program at a specific point, typically in response to an unexpected or fatal
     error. `brrdbg_trace(...)` will pass its arguments to `printf` before halting.  
   * `brrdbg_assert(x, ...)` & `brrdbg_static_assert(x, m)`: If `BRRDBG_NO_ASSERT` is not `#define`d,
     `brrdbg_assert(x, ...)` will check the condition `x` and if it is false (0), raise a runtime exception
     via `brrdbg_trace(...)`, passing along its `__VA_ARGS__`; `brrdbg_trace(...)` must be available, else
     `brrdbg_assert(x, ...)` will do nothing.  
     `brrdbg_static_assert(x, m)` will error with the string message `m` if the condition `x` is false (0) at
     compile-time; when using C11 or greater, `brrdbg_static_assert(x, m)` will resolve to the standard
     static assert, otherwise `x`, if false, will insert a syntax-error at the location.

   No dependencies.
 * `brrtypes.h`: `typedef`s for the various integer types used throughout the library.

   No dependencies.
 * `brrmacro.h`: `#define`s a few helper macros to ease the creation of other, more complicated macros:
   * `brr_expand(x)`: Does recursive preprocessor expansion on the token `x`.
   * `brr_stringify(x)`: Stringifies the token `x`.
   * `brr_estringify(x)`: Does recursive preprocessor expansion on the token `x`, then stringifies the
     result.
   * `brr_narg(...)`: Returns the count of arguments passed to it, up to a maximum of 255.
   * `brr_join(a, b)`: Performs preprocessor token adjoining on `a` and `b` to produce `a##b`
   * `brr_ejoin(a, b)`: Does recursive preprocessor expansion on the tokens `a` and `b`, then joins the
     result.
   * `brr_any(x, f)`: Checks that any of the bitflags specified by `f` are set in `x`.
   * `brr_all(x, f)`: Checks that all of the bitflags specified by `f` are set in `x`.
   * `brr_none(x, f)`: Checks that no bitflags specified by `f` are set in `x`.
   * `brr_map(X, ...)`: Applies the macro `X` to all of the following arguments, implemented via
     [`map-macro`][map-macro].
   * `brr_map_list(X, ...)`: Applies the macro `X` to all of the following arguments, with commas separating
     the output, implemented via [`map-macro`][map-macro].

   No dependencies.
 * `brrarray.h`: Functions for manipulating data as an array of elements, including splitting (&
   partitioning), joining (append/prepend), insertion, deletion, sorting, and (soon) filtering; see the
   header file for more in-depth documentation.

   Requires on `brrapi.h` and `brrtypes.h`.
 * `brrcon.h`: Console/terminal specific functionality:
   * `brrcon_pause()`: Pause execution until a key is read on `stdin`.
   * `brrcon_clear()`: Clear the text of the terminal.

   Requires `brrapi.h`. * `brrhash.h`: Contains functions (eventually) for computing various hashes on
   different types of data; currently, only the [`fnv1a`][fnv1a] hash is implemented.
 * `brrnum.h`: A few numerical functions on integers:
   * `brrnum_ndigits(brru8 x, int base)`: Count the number of digits required to represent `x`
     in base `base`.
   * `brrnum_wrap(brrs8 x, brru8 modulus, brrs8 offset)`: Performs 'correct' (citation needed) modulus,
     casting when appropriate to make sure outlier conditions of the `%` operator are avoided.
   * `brrnum_[u|s]gcf(brr_8 a, brr_8 b)`: Performs the Euclidean GCF algorithm on the integers `a` and `b`.
   * `brrnum_[u|s][min|max](brr_8 a, brr_8 b)`: Returns the minimum/maximum of `a` and `b`.
   * `brrnum_[u|s]clamp(brr_8 x, brr_8 min, brr_8 max)`: Returns `x` clamped between `min` and `max`.

   Requires `brrtypes.h`
 * `brrrand.h`: Functions that return sequences of pseudo-random integers, implemented using
    [`splitmix64`][splitmix64] and [`xoshiro256`][xoshiro256]:
   * `brrrand_rand()`: Returns a single random integer, based off of a static seed value.
   * `brrrand_seed(brru8 seed)`: Sets the seed for `brrrand_rand()`.
   * `brrrand_srand(brru8 seed, brru8 iter)`: Performs `iter` iterations of randomization on `seed` to
     generate a random number.
   * `brrrand_trand(brru8 iter)`: Performs `iter` iterations of randomization on a seed value that depends
     on the current time, as gotten from `brrtime.h`.

  Requires `brrapi.h`, `brrtypes.h`, and `brrtime.h`.
 * `brrtime.h`: Functions for getting/manipulating the running program's time; not to be used for actually
   timing things however:
   * `brrtime_utime()`: Returns the current time as read from the wall, in microseconds.
   * `brrtime_sleep(brru8 usec)`: Pauses program execution for `usec` microseconds.

#### Do not use these
They're in need of major rewrite, but I'm presently too lazy to fix them.
 * `brrpath`: Functions for querying the file system.


## Build
[ToC](#table-of-contents)

*Note:* For a more in-depth list and explanation, run `make help` or check `help.mk`.

In order to build, you'll need GNU Make.

If you look through the various `.mk` files, you'll see a plethora of various settings that can be customized
(they're assigned with `?=`); here are only the primary ones for build customization:

| Host    | Target  | Command                                |
| :---    |  ---:   | :---                                   |
| Unix    | Unix    | `make [target=unix] [host=unix] ...`   |
| Unix    | Windows | `make target=windows [host=unix] ...`  |
| Windows | Unix    | Not implemented                        |
| Windows | Windows | `make target=windows host=windows ...` |

Host and target architectures can be specified with `host_bit` and `target_bit` respectively, either `32` or
`64`; choosing target systems/architectures different from your host will require the appropriate toolchain
to be installed (32-bit toolchain for 32-bit targets built on 64-bit machines and vice-versa, Windows
toolchain for Windows targets built on unix machines, etc.).

Each command will build the library appropriate to the target, so a `.so`/`.dll` for a shared linux/Windows
build, and `.a`/`.lib` for a static linux/Windows build. 32-bit builds get a `_32` suffix by default.

**Disclaimer:** I only have a linux distribution, so I am unable/too lazy to test building on Windows or
other unixes, so don't be surprised if it doesn't work; in such a case that it doesn't a github issue would
be appreciated.

## Todo
[ToC](#table-of-contents)
* `brrstringr`/`brrpath` could be improved (quite drastically, I suspect).
* `brrarray`: Was `brrdata`, and manipulated bytes directly, without the need for the `brrarray` struct; I
  think something like that would be convenient in its own right, as well as the array-based functionality.

[otherbrr]:https://github.com/Optiroc/BRRtools
[map-macro]:https://github.com/swansontec/map-macro
[fnv1a]:https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
[splitmix64]:https://xorshift.di.unimi.it/splitmix64.c
[xoshiro256]:https://prng.di.unimi.it/xoshiro256plus.c
[gawk]:https://www.gnu.org/software/gawk/manual/html_node/index.html
[gmake]:https://www.gnu.org/software/make/manual/html_node/index.html
[gcc]:https://gcc.gnu.org/onlinedocs/gcc-11.1.0/gcc/
[ezwin]:https://sourceforge.net/projects/ezwinports/
[mingww64]:https://en.wikipedia.org/wiki/Mingw-w64
[mingwwin]:https://sourceforge.net/projects/mingw-w64/
[gmakewin]:https://sourceforge.net/projects/ezwinports/
