# BRRTOOLS
A personal ~C99~ C11 library with the goal to have a single place for multiple
cross-platform, miscellaneous utilities that are generally useful (to me
anyways).

Has absolutely no relation to [BRRtools][otherbrr]
## Contents
[Contents](#contents)

1. [Compilation](#compilation)  
2. [Installation](#installation)  
3. [Customization](#build-customization)  
3. [Usage](#usage)  

## Compilation
[Contents](#contents)  

|Host   |Target |Requirements                                            |
|:---:  |:---:  |:---                                                    |
|\*NIX  |\*NIX  |[GNU `make`][gmake] and [`gcc` toolchain][gcc]          |
|\*NIX  |Windows|[GNU `make`][gmake] and [`mingw` toolchain][mingww64]   |
|Windows|\*NIX  |Windows-to-\*NIX compilation is not and likely won't be implemented|
|Windows|Windows|[GNU `make`][gmakewin] and [`mingw` toolchain][mingwwin]|

To compile, just run `make` with `HOST` set to either `UNIX` or `WINDOWS`,
depending on your host system and set `TARGET` likewise. `HOST` defaults to
`UNIX`, and `TARGET` defaults to `HOST`.

**Note** that to cross-compile from 64-bit to 32-bit or vice-versa (though
vice-versa isn't implemented yet) will also require the `mingw` toolchain.

Example:  
  * Unix host with Unix target:  
```shell
make HOST=UNIX TARGET=UNIX
```
  * Unix host with Windows target:  
```shell
make HOST=UNIX TARGET=WINDOWS
```
  * Windows host with windows target:  
```shell
make HOST=WINDOWS TARGET=WINDOWS
```  

**NOTE** Windows-based compilation is untested; don't be surprised if it fails
for whatever reason.

To customize the type of binary to generate, there are different environment
variables that can be set before compilation:
  * `HOSTBIT`: Set to specify what bitness your system is, either `32` or
    `64`.  
    Defaults to `64`
  * `TARGETBIT`: Set to specify what bitness compilation should target, either 
    `32` to generate a 32-bit binary, or `64` for a 64-bit binary.  
    Defaults to `HOSTBIT`
  * `TARGETMODE`: Set to `SHARED` to generate a shared library or dll depending
    on what the target system is.  
    Defaults to `SHARED`  
  * `PEDANTIC`: Enables GCC pedantic compilation checks; on by default, set to
    `0` to disable.  

All settings are case-sensitive.

## Installation
[Contents](#contents)  
### Unix:
```shell
make install [prefix=/custom/install/root]
```
to install the library to the custom prefix.  
The default prefix on \*NIX is `/usr/local` and `./install` on Windows.

### Windows:
If calling `make install` fails with some error approximating `unrecognized
command` or `no such file or directory` or the like,  please let me know.

## Build Customization
[Contents](#contents)  
There are various variables you can set after to customize the build process:
  * `DEBUG`: Enables compiling with debugging
  symbols, and does little-to-no optimization.  
  * `MEMCHECK`: Changes the behavior of `DEBUG` to so that the compiled
    binaries are `valgrind`-compatible.  

Custom compiler/linker/archiver executables can be set:
  * `CC_CUSTOM`: Set a custom c-compiler. Only useable on Unix.  
  Defaults to `gcc`, or appropriate mingw version if targeting Windows.
  * `AR_CUSTOM`: Set a custom archiver for static library generation.  
  Defaults to `gcc-ar`, or appropriate mingw version if targeting Windows.
  * `DLLTOOL`: Executable to use for `dlltool` to generate dll files.  
  Defaults to `dlltool` on Windows, or appropriate mingw version if on Unix and
  targeting Windows.

Build output is generated in a set of subdirectories, typically following the
form  
```shell
BLDROOT/BLDNAME/BLDTARGET/BLDMODE/BLDBIT
```
where each element can be manually set, or left to be a default value,
depending on what output configurations are set to.

|Variable   |Default              |Description                                                                                     |
|:---       |:---                 |:---                                                                                            |
|`BLDROOT`  |The project directory|The root of build output.                                                                       |
|`BLDNAME`  |`build`              |The name of the directory in `BLDROOT` where all output will be placed.                         |
|`BLDTARGET`|`uni`/`win`          |Defaults to `uni` if `TARGET=UNIX` or `win` if `TARGET=WINDOWS`.                            |
|`BLDMODE`  |`dyn`/`sta`          |Defaults to `dyn` (dynamic) if `TARGETMODE=SHARED` or `sta` (static) if `TARGETMODE=STATIC`.|
|`BLDBIT`   |`64`/`32`            |Defaults to `64` if `TARGETBIT=64` or `32` if `TARGETBIT`=`32`.                               |

Setting any target to be empty (`''`) effectively removes it from the output
path.

There are a few other such options that can be configured; see near the end of
`config.mk` to find what can and shouldn't be changed.

## Usage
[Contents](#contents)  
### Unix:
#### Static linkage:
```shell
{compiler} /path/to/libbrrtools.a {compilerargs}
```
or if you can pass arguments to the linker through the compiler (like with
`gcc`):
```shell
{compiler} {compilerargs} -Wl,-Bstatic -lbrrtools -Wl,-Bdynamic {moreargs}
```
#### Dynamic linkage:
```shell
{compiler} {compilerargs} [-L/custom/brrtools/prefix/lib] -lbrrtools
```
### Windows:
Windows IDEs like Visual Studio have the ability to link against specific
binaries. It is unknown to me how to do that, but it should be no different
than when linking against any other library.

However, for `Dynamic` linkage, `BRRTOOLS_IMPORTS` must be `#define`d before
compilation.

## TODO
This is NOT memory efficient (at least nothing to do with strings/path walking)

[otherbrr]:https://github.com/Optiroc/BRRtools
[gawk]:https://www.gnu.org/software/gawk/manual/html_node/index.html
[gmake]:https://www.gnu.org/software/make/manual/html_node/index.html
[gcc]:https://gcc.gnu.org/onlinedocs/gcc-11.1.0/gcc/
[ezwin]:https://sourceforge.net/projects/ezwinports/
[mingww64]:https://en.wikipedia.org/wiki/Mingw-w64
[mingwwin]:https://sourceforge.net/projects/mingw-w64/
[gmakewin]:https://sourceforge.net/projects/ezwinports/
