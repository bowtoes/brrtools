# BRRTOOLS
A personal ~C99~ C11 library with the goal to have a single place for multiple
cross-platform, miscellaneous utilities that are generally useful (to me
anyways).

**TODO**: this readme needs an update.

Has absolutely no relation to [BRRtools][otherbrr]
## Contents
[Contents](#contents)

1. [Usage](#usage)  
2. [Installation](#installation)  
3. [Build](#build)  

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
`ldconfig` will likely need to be run on the `.so`'s installation directory so
that `ld` can find it, which is automatically done when installing if
`ld_config` is set:
```shell
sudo make install target_mode=shared [prefix=/some/stupid/prefix] do_ldconfig=1
```
### Windows:
Windows IDEs like Visual Studio have the ability to link against specific
binaries, though I don't personally know how to do that, but it should be no
different than when linking against any other library; if something doesn't
work, lemmino.

Please note that for dynamic linkage, `BRR_imports` must be `#define`d
before compilation.

## Installation
[Contents](#contents)  
### Unix:
```shell
make install [prefix=/custom/install/root]
```

The default prefix on unix is `/usr/local`; on Windows it's in a local
directory `./install`.

### Windows:
If calling `make install` fails with some error approximating `unrecognized
command` or `no such file or directory` or the like, please let me know.

## Build
*Note:* For a more in-depth list and explanation, run `make help` or check
`help.mk`.

In order to build, you'll need GNU Make (which, as far as I know, can be
installed on Windows, though I wouldn't know how to such).

If you look through the various `.mk` files, you'll see a plethora of various
settings that can be customized (they're assigned with `?=`); here are only the
primary ones for build customization:

| Host    | Target  | Mode   | Command                                                   |
| :---    | :---:   | ---:   | :---                                                      |
| Unix    | Unix    | Shared | `make target_mode=shared [target=unix] [host=unix] ...`   |
| Windows | Unix    | Shared | Not implemented                        |
| Unix    | Windows | Shared | `make target_mode=shared target=windows [host=unix] ...`  |
| Windows | Windows | Shared | `make target_mode=shared target=windows host=windows ...` |
| Unix    | Unix    | Static | `make [target_mode=static] [target=unix] [host=unix] ...`   |
| Windows | Unix    | Static | Not implemented                        |
| Unix    | Windows | Static | `make [target_mode=static] target=windows [host=unix] ...`  |
| Windows | Windows | Static | `make [target_mode=static] target=windows host=windows ...` |

Host and target architectures can be specified with `host_bit` and `target_bit`
respectively, either `32` or `64`; choosing target systems/architectures
different from your host will require the appropriate toolchain to be installed
(32-bit toolchain for 32-bit targets built on 64-bit machines and vice-versa,
Windows toolchain for Windows targets built on unix machines, etc.).

**Disclaimer:** I only have a Linux distribution, so I can't/am too lazy to
test building on Windows or other Unixes (don't be surprised if it doesn't
work, but do let me know).  

## TODO
* Speaking of improvement, `brrlog`. Holy hell it's a mess, especially trying
  to stylize any amount of text is a huge pain; currently working on that,
  might be v0.0.2b or something.
* Features section in this readme
* `brrstringr`/`brrpath` I don't think are very memory efficient, and could be
  vastly improved to say the least.

[otherbrr]:https://github.com/Optiroc/BRRtools
[gawk]:https://www.gnu.org/software/gawk/manual/html_node/index.html
[gmake]:https://www.gnu.org/software/make/manual/html_node/index.html
[gcc]:https://gcc.gnu.org/onlinedocs/gcc-11.1.0/gcc/
[ezwin]:https://sourceforge.net/projects/ezwinports/
[mingww64]:https://en.wikipedia.org/wiki/Mingw-w64
[mingwwin]:https://sourceforge.net/projects/mingw-w64/
[gmakewin]:https://sourceforge.net/projects/ezwinports/
