# BRRTOOLS
A custom C99 library with the goal to have a single place for multiple miscellaneous
utilities that are generally useful. Also with the goal of having them be cross-platform.

Has absolutely no relation to [BRRtools](https://github.com/Optiroc/BRRtools)
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
|\*NIX|\*NIX    |GNU `make` and `gcc` toolchain|
|\*NIX|Windows  |GNU `make` and `mingw`|
|Windows|\*NIX  |N/A|
|Windows|Windows|`mingw` toolchain, or Cygwin with GNU `make` and toolchain|

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

There are other environment variables that can be set to control the type of binary that is produced.
  * Set `BITS` to `32` or `64` to customize bit-width of compiled binary; defaults to `64`.
  * Set `MODE` to `SHARED` or `STATIC` to compile either a dynamic library or static library, respectively; defaults to `SHARED`.
All these settings are case-sensitive.

## Installation
[Contents](#contents)  
Installation through make only happens on `UNIX` hosts with `UNIX` targets.  
Simply run:  
```shell
make prefix=/custom/install/root
```
to install the library to the custom prefix. `prefix` defaults to `/usr/local`.

## Customization
[Contents](#contents)  
There are many settings/variables that can be edited/set to customize build settings.
They are all listed with their default values in `config.mk`.

The environment variables `DEBUG` and `MEMCHECK` can be set before compilation
to change the compilation commands respectively.  
  * `DEBUG` enables compiling with debugging symbols, and does little-to-no optimization.  
  * `MEMCHECK` changes the options changed by `DEBUG` to make them `valgrind`-compatible.  

## Usage
[Contents](#contents)  
When linking dynamically in Windows, define `BRRTOOLS_IMPORTS` before including any of the library headers.
It isn't necessary, but better to be safe.

Otherwise, linking however you usually do it on your platform should work.
