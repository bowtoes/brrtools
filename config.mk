CURDIR:=$(PWD)
# Name of project (or overwrite it if you prefer)
ifndef PROJECT
PROJECT:=$(notdir $(CURDIR))
endif

$(PROJECT)_MAJOR:=0
$(PROJECT)_MINOR:=0
$(PROJECT)_REVIS:=0

# Assume linux host
ifndef HOST
 HOST:=LINUX
else ifneq ($(HOST),LINUX)
 ifneq ($(HOST),WINDOWS)
  HOST:=LINUX
 else # Windows host always compiles windows target
  TARGET:=WINDOWS
 endif
endif ## HOST
# Assume linux target
ifndef TARGET
 TARGET:=LINUX
else ifneq ($(TARGET),LINUX)
 ifneq ($(TARGET),WINDOWS)
  TARGET:=LINUX
 endif
endif ## TARGET
# Assume dynamic linkage
ifndef MODE
 MODE:=SHARED
else ifneq ($(MODE),STATIC)
 ifneq ($(MODE),SHARED)
  MODE:=SHARED
 endif
endif ## MODE
# Assume 64-bit target
ifndef BITS
 BITS:=64
else ifneq ($(BITS),32)
 ifneq ($(BITS),64)
  BITS:=64
 endif
endif ## BITS

### Directory locations relative to $(CURDIR)
ifndef SRCDIR
SRCDIR:=src
endif
ifndef HDRDIR
HDRDIR:=src/brrtools
endif
ifndef BLDDIR
BLDDIR:=build
endif
ifndef LINDIR
LINDIR:=lin
endif
ifndef WINDIR
WINDIR:=win
endif
ifndef SHRDIR
SHRDIR:=shared
endif
ifndef STADIR
STADIR:=static
endif
ifndef B32DIR
B32DIR:=x86
endif
ifndef B64DIR
B64DIR:=x64
endif

ifndef INTDIR
INTDIR:=int
endif
ifndef ASSDIR
ASSDIR:=ass
endif
ifndef OBJDIR
OBJDIR:=obj
endif

ifndef LINSHRNAME
LINSHRNAME:=lib$(PROJECT).so
endif
ifndef LINSTANAME
LINSTANAME:=lib$(PROJECT).a
endif
ifndef WINSHRNAME
WINSHRNAME:=$(PROJECT).dll
endif
ifndef WINDEFNAME
WINDEFNAME:=$(PROJECT).def
endif
ifndef WINIMPNAME
WINIMPNAME:=$(PROJECT).dll.lib
endif
ifndef WINSTANAME
WINSTANAME:=lib$(PROJECT).lib
endif
# Where generated/compiled output files will live, with directory structures
# matching that of $(SRCDIR)
ifeq ($(TARGET),WINDOWS)
 OUTDIR:=$(BLDDIR)/$(WINDIR)
else
 OUTDIR:=$(BLDDIR)/$(LINDIR)
endif ## TARGET
ifeq ($(MODE),SHARED)
 OUTDIR:=$(OUTDIR)/$(SHRDIR)
else
 OUTDIR:=$(OUTDIR)/$(STADIR)
endif ## MODE
ifeq ($(BITS),32)
 OUTDIR:=$(OUTDIR)/$(B32DIR)
else
 OUTDIR:=$(OUTDIR)/$(B64DIR)
endif ## BITS

### Toolchain
## Compiler
ifndef CC_CUSTOM
 ifeq ($(HOST),LINUX)
  ifeq ($(TARGET),LINUX)
   CC:=gcc
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    CC:=i686-w64-mingw32-gcc
   else # Assume 64 bit
    CC:=x86_64-w64-mingw32-gcc
   endif ## BITS
  endif ## TARGET
 else # Windows host, must have mingw
  CC:=gcc
 endif ## HOST
else
 CC:=$(CC_CUSTOM)
endif ## CC
## Archiver
ifndef AR_CUSTOM
 ifeq ($(HOST),LINUX)
  ifeq ($(TARGET),LINUX)
   AR:=gcc-ar
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    AR:=i686-w64-mingw32-gcc-ar
   else # Assume 64 bit
    AR:=x86_64-w64-mingw32-gcc-ar
   endif ## BITS
  endif ## TARGET
 else # Windows host, must have mingw
  AR:=gcc-ar
 endif ## HOST
else
 AR:=$(AR_CUSTOM)
endif ## AR
ifndef DLLTOOL
 ifeq ($(HOST),LINUX)
  ifeq ($(TARGET),LINUX)
   DLLTOOL:=echo There is no dlltool for linux
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    DLLTOOL:=i686-w64-mingw32-dlltool
   else
    DLLTOOL:=x86_64-w64-mingw32-dlltool
   endif ## BITS
  endif ## TARGET
 else # Windows host
  DLLTOOL:=dlltool
 endif ## HOST
endif ## DLLTOOL

### Compiler arguments
## Includes
INCS:=-I$(SRCDIR)
## Warnings/errors
WRNS:=-Wall -Wextra -Wpedantic -pedantic -Werror=pedantic -pedantic-errors\
      -Werror=implicit-function-declaration -Werror=missing-declarations\
      -Wno-unused-function -Wno-sign-compare -Wno-unused-parameter
## Defines
DEFS:=-D$(PROJECT)_MAJOR=$($(PROJECT)_MAJOR)\
      -D$(PROJECT)_MINOR=$($(PROJECT)_MINOR)\
      -D$(PROJECT)_REVIS=$($(PROJECT)_REVIS)\
      -D$(PROJECT)_VERSION='$($(PROJECT)_MAJOR).$($(PROJECT)_MINOR).$($(PROJECT)_REVIS)'
## Binary bitness
ifeq ($(BITS),32)
 BARG:=-m32
else
 BARG:=-m64
endif ## BITS
# PRF: Performance options (applied at compile- & link-time)
# OPT: Optimization options (applied at compile-time)
ifdef DEBUG
 ifndef MEMCHECK
  PRF:=-pg -no-pie
 else
  PRF:=
 endif ## MEMCHECK
 OPT:=-O0 -g
else
 ifeq ($(TARGET),SHARED)
  PRF:=-s
 else
  PRF:=
 endif
 OPT:=-O3 -Ofast
endif ## DEBUG
PRF:=$(PRF) $(BARG)

$(PROJECT)_PCFLAGS:=-std=c99 $(WRNS) $(INCS) $(OPT) $(PRF) $(CFLAGS)
$(PROJECT)_PCPPFLAGS:=$(DEFS) $(STCPPFLAGS) $(CPPFLAGS)
$(PROJECT)_PLDFLAGS:=$(PRF) $(LDFLAGS)

### Linker arguments
LNK:=
ifeq ($(TARGET),LINUX)
 ifeq ($(MODE),SHARED)
  ifdef PIC2
   OPT:=$(OPT) -fPIC
  else
   OPT:=$(OPT) -fpic
  endif ## PIC2
  LNK:=-shared
  TARGETNAME:=$(LINSHRNAME)
 else # Static
  TARGETNAME:=$(LINSTANAME)
 endif ## MODE
else # Windows target
 DEFS:=-DWIN32_LEAN_AND_MEAN $(DEFS)
 LNK:=-Wl,--subsystem,windows
 ifeq ($(MODE),SHARED)
  DEFS:=-DBRRTOOLS_SHARED -DBRRTOOLS_EXPORTS $(DEFS)
  LNK:=$(LNK) -shared
  TARGETNAME:=$(WINSHRNAME)
 else # Static
  TARGETNAME:=$(WINSTANAME)
 endif ## MODE
endif ## TARGET

# Basic, platform-indenpendent flags
$(PROJECT)_CFLAGS=-std=c99 $(WRNS) $(INCS) $(OPT) $(PRF) $(CFLAGS)
$(PROJECT)_CPPFLAGS=$(DEFS) $(STCPPFLAGS) $(CPPFLAGS)
$(PROJECT)_LDFLAGS=$(PRF) $(LNK) $(LDFLAGS)

SRC:=\
	src/brrtools/brrlib.c\
	src/brrtools/brrlog.c\

HDR:=\
	src/brrtools/brrplatform.h\
	src/brrtools/brrtypes.h\
	src/brrtools/brrlib.h\
	src/brrtools/brrlog.h\

PSRC:=\
	src/main.c\

PHDR:=\


ifeq ($(HOST),LINUX)
 NULL:=/dev/null
 RM:=rm -fv
else
 NULL:=nul
 RM:=del /F
 RMDIR:=rmdir
endif ## HOST

# Default installation prefix
ifndef prefix
prefix=/usr/local
endif
