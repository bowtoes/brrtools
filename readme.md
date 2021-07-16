# BRRTOOLS
A custom C99 library with the goal to have a single place for multiple cross-platform,
miscellaneous utilities that are generally useful.

Has absolutely no relation to [BRRtools][otherbrr]
## Contents
[Contents](#contents)

1. [Compilation](#compilation)  
2. [Installation](#installation)  
3. [Customization](#customization)  
3. [Usage](#usage)  

## Compilation
[Contents](#contents)  

|Host |Target   |Requirements|
|:---:|:---:    |:---       |
|\*NIX|\*NIX    |[GNU `make`][gmake] and [`gcc` toolchain][gcc]|
|\*NIX|Windows  |[GNU `make`][gmake] and [`mingw` toolchain][mingww64]|
|Windows|\*NIX  |N/A|
|Windows|Windows|[GNU `make`][gmakewin] and [`mingw` toolchain][mingwwin]|

To compile, just run `make` with `HOST` set to either `UNIX` or `WINDOWS`, depending on your host system
and set `TARGET` likewise. `HOST` and `TARGET` default to `UNIX`.  

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
NOTE: Windows-based compilation is untested; don't be surprised if it fails for
whatever reason.

To customize the type of binary to generate, there are different environment
varivariables that can be set before compilation:
  * `BITS`: Set to `32` to generate a 32-bit binary, or `64` for a 64-bit
  binary.  
  Defaults to `64`
  * `MODE`: Set to `SHARED` to generate a shared library or dll depending on
  what the target system is.  
  Defaults to `SHARED`  
  * 'PEDANTIC': Set to anything in order to enable GCC pedantic compilation
  checks.  

These settings are very much case-sensitive.

## Installation
[Contents](#contents)  
### Unix:
```shell
make prefix=/custom/install/root
```
to install the library to the custom prefix.
The default prefix is `/usr/local`.

### Windows:
Put the binary of your choosing, together with the headers, into whatever search
paths you have set for your project.

## Build Customization
[Contents](#contents)  
It's possible to set various environment variables to customize the build
process:
  * `DEBUG`: Enables compiling with debugging
  symbols, and does little-to-no optimization.  
  * `MEMCHECK`: Changes the behavior of `DEBUG` to so that the compiled binaries
  are `valgrind`-compatible.  

Custom compiler/linker/archiver executables can be set:
  * `CC_CUSTOM`: Set a custom c-compiler. Only useable on Unix.  
  Defaults to `gcc`, or appropriate mingw version if targeting Windows.
  * `AR_CUSTOM`: Set a custom archiver for static library generation.  
  Defaults to `gcc-ar`, or appropriate mingw version if targeting Windows.
  * `DLLTOOL`: Executable to use for `dlltool` to generate dll files.  
  Defaults to `dlltool` on Windows, or appropriate mingw version if on Unix and
  targeting Windows.

Output build directories can be changed:

|Variable         |Default          |Description                              |
|:---             |:---             |:---                                     |
|`BLDDIR`         |`build`          |Root                                     |
|`UNIDIR`/`WINDIR`|`uni`/`win`      |Unix/Windows output                      |
|`SHRDIR`/`STADIR`|`shared`/`static`|Shared/static output                     |
|`B32DIR`/`B64DIR`|`x86`/`x64`      |32-/64-bit output                        |
|`CHKDIR`         |`check`          |Name of check executable output directory|
|`INTDIR`         |`int`            |Name of intermediates output directory   |
|`ASSDIR`         |`ass`            |Name of assembly output directory        |
|`OBJDIR`         |`obj`            |Name of objects output directory         |

## Usage
[Contents](#contents)  
### Unix:
#### Static linkage:
```shell
{compiler} /path/to/libbrrtools.a {compilerargs}
```
or if you can pass arguments to the linker through the compiler (like with `gcc`):
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

[otherbrr]:https://github.com/Optiroc/BRRtools
[gawk]:https://www.gnu.org/software/gawk/manual/html_node/index.html
[gmake]:https://www.gnu.org/software/make/manual/html_node/index.html
[gcc]:https://gcc.gnu.org/onlinedocs/gcc-11.1.0/gcc/
[ezwin]:https://sourceforge.net/projects/ezwinports/
[mingww64]:https://en.wikipedia.org/wiki/Mingw-w64
[mingwwin]:https://sourceforge.net/projects/mingw-w64/
[gmakewin]:https://sourceforge.net/projects/ezwinports/
