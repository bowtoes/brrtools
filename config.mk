# { Core configuration.
CURDIR:=$(PWD)
PROJECT:=brrtools
UPROJECT:=BRRTOOLS
ifndef prefix
 prefix=/usr/local
endif

$(PROJECT)_MAJOR:=0
$(PROJECT)_MINOR:=0
$(PROJECT)_REVIS:=0
# }

ifndef HOST # {
 HOST:=UNIX
else ifneq ($(HOST),UNIX)
 ifneq ($(HOST),WINDOWS)
  HOST:=UNIX
 else # Windows host always compiles windows target
  TARGET:=WINDOWS
 endif
endif ### }
ifndef TARGET # {
 TARGET:=UNIX
else ifneq ($(TARGET),UNIX)
 ifneq ($(TARGET),WINDOWS)
  TARGET:=UNIX
 endif
endif # }
ifndef MODE # {
 MODE:=SHARED
else ifneq ($(MODE),STATIC)
 ifneq ($(MODE),SHARED)
  MODE:=SHARED
 endif
endif # }
ifndef BITS # {
 BITS:=64
else ifneq ($(BITS),32)
 ifneq ($(BITS),64)
  BITS:=64
 endif
endif # }

ifeq ($(HOST),UNIX) # {
 NULL:=/dev/null
 RM:=rm -fv
else
 NULL:=nul
 RM:=del /F
 RMDIR:=rmdir
endif # }

ifndef SRCDIR # {
SRCDIR:=src
endif # }
ifndef HDRDIR # {
HDRDIR:=src/brrtools
endif # }
ifndef BLDDIR # {
BLDDIR:=build
endif # }
ifndef UNIDIR # {
UNIDIR:=uni
endif # }
ifndef WINDIR # {
WINDIR:=win
endif # }
ifndef SHRDIR # {
SHRDIR:=shared
endif # }
ifndef STADIR # {
STADIR:=static
endif # }
ifndef B32DIR # {
B32DIR:=x86
endif # }
ifndef B64DIR # {
B64DIR:=x64
endif # }
ifndef CHKDIR # {
CHKDIR:=check
endif # }
ifndef INTDIR # {
INTDIR:=int
endif # }
ifndef ASSDIR # {
ASSDIR:=ass
endif # }
ifndef OBJDIR # {
OBJDIR:=obj
endif # }

ifndef UNISHRNAME # {
UNISHRNAME:=$(PROJECT)
endif # }
ifndef UNISTANAME # {
UNISTANAME:=$(PROJECT)
endif # }
ifndef WINSHRNAME # {
WINSHRNAME:=$(PROJECT)
endif # }
ifndef WINDEFNAME # {
WINDEFNAME:=$(PROJECT)
endif # }
ifndef WINIMPNAME # {
WINIMPNAME:=$(PROJECT)
endif # }
ifndef WINSTANAME # {
WINSTANAME:=$(PROJECT)
endif # }

ifndef UNISHRFILE # {
UNISHRFILE:=lib$(PROJECT).so
endif # }
ifndef UNISTAFILE # {
UNISTAFILE:=lib$(PROJECT).a
endif # }
ifndef WINSHRFILE # {
WINSHRFILE:=$(PROJECT).dll
endif # }
ifndef WINDEFFILE # {
WINDEFFILE:=$(PROJECT).def
endif # }
ifndef WINIMPFILE # {
WINIMPFILE:=$(PROJECT).dll.lib
endif # }
ifndef WINSTAFILE # {
WINSTAFILE:=lib$(PROJECT).lib
endif # }

ifndef CC_CUSTOM # {
 ifeq ($(HOST),UNIX)
  ifeq ($(TARGET),UNIX)
   CC:=gcc
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    CC:=i686-w64-mingw32-gcc
   else
    CC:=x86_64-w64-mingw32-gcc
   endif
  endif
 else # Windows host, must have mingw
  CC:=gcc
 endif
else
 ifeq ($(HOST),UNIX)
  ifeq ($(TARGET),UNIX)
   # Custom cc only on unix, to unix for now
   CC:=$(CC_CUSTOM)
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    CC:=i686-w64-mingw32-gcc
   else
    CC:=x86_64-w64-mingw32-gcc
   endif
  endif
 else # Windows host, must have mingw
  CC:=gcc
 endif
endif # }
ifndef AR_CUSTOM # {
 ifeq ($(HOST),UNIX)
  ifeq ($(TARGET),UNIX)
   AR:=gcc-ar
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    AR:=i686-w64-mingw32-gcc-ar
   else
    AR:=x86_64-w64-mingw32-gcc-ar
   endif
  endif
 else # Windows host, must have mingw
  AR:=gcc-ar
 endif
else
 AR:=$(AR_CUSTOM)
endif # }
ifndef DLLTOOL # {
 ifeq ($(HOST),UNIX)
  ifeq ($(TARGET),UNIX)
   DLLTOOL:=echo There is no dlltool for unix
  else # Windows target, must have mingw
   ifeq ($(BITS),32)
    DLLTOOL:=i686-w64-mingw32-dlltool
   else
    DLLTOOL:=x86_64-w64-mingw32-dlltool
   endif
  endif
 else # Windows host
  DLLTOOL:=dlltool
 endif
endif # }

ifeq ($(TARGET),WINDOWS) # {
 OUTDIR:=$(BLDDIR)/$(WINDIR)
else
 OUTDIR:=$(BLDDIR)/$(UNIDIR)
endif # }
ifeq ($(BITS),32) # {
 OUTDIR:=$(OUTDIR)/$(B32DIR)
else
 OUTDIR:=$(OUTDIR)/$(B64DIR)
endif # }
ifeq ($(MODE),SHARED) # {
 OUTDIR:=$(OUTDIR)/$(SHRDIR)
else
 OUTDIR:=$(OUTDIR)/$(STADIR)
endif # }

# { INCS & WRNS
INCS:=-I$(SRCDIR)
WRNS:=-Wall -Wextra -Wpedantic -Werror=pedantic\
      -Werror=implicit-function-declaration -Werror=missing-declarations\
      -Wno-unused-function -Wno-sign-compare -Wno-unused-parameter
ifneq ($(CC),tcc)
 WRNS:=-pedantic -pedantic-errors $(WRNS)
endif # }
# { DEFS
DEFS:=-D$(UPROJECT)_MAJOR=$($(PROJECT)_MAJOR)\
      -D$(UPROJECT)_MINOR=$($(PROJECT)_MINOR)\
      -D$(UPROJECT)_REVIS=$($(PROJECT)_REVIS)\
      -D$(UPROJECT)_VERSION='$($(PROJECT)_MAJOR).$($(PROJECT)_MINOR).$($(PROJECT)_REVIS)'

ifdef DEBUG
 DEFS:=$(DEFS) -D$(UPROJECT)_DEBUG
 ifdef MEMCHECK
  DEFS:=$(DEFS) -D$(UPROJECT)_MEMCHECK
 endif
endif
ifeq ($(TARGET),UNIX)
 DEFS:=$(DEFS) -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L
else
 DEFS:=-DWIN32_LEAN_AND_MEAN $(DEFS)
endif # }
# { CC[LD]FLG
ifdef DEBUG
# CCLDFLG: Performance options (applied at compile- & link-time)
# CCFLG: Optimization options (applied at compile-time)
 CCFLG:=-O0 -g
 ifdef MEMCHECK
  CCLDFLG:=
 else
  CCLDFLG:=-pg -no-pie
 endif ## MEMCHECK
else
 CCFLG:=-O3 -Ofast
 ifeq ($(TARGET),SHARED)
  CCLDFLG:=-s
 else
  CCLDFLG:=
 endif
endif
ifeq ($(BITS),32)
 CCLDFLG:=$(CCLDFLG) -m32
else
 CCLDFLG:=$(CCLDFLG) -m64
endif # }

# { CHECK
$(PROJECT)_CHECK_CFLAGS:=-std=c99 $(WRNS) $(INCS) $(CCFLG) $(CCLDFLG) $(CFLAGS)
$(PROJECT)_CHECK_CPPFLAGS:=$(DEFS) $(STCPPFLAGS) $(CPPFLAGS)
$(PROJECT)_CHECK_LDFLAGS:=$(CCLDFLG) $(LDFLAGS)
ifeq ($(TARGET),WINDOWS)
 ifeq ($(MODE),SHARED)
  $(PROJECT)_CHECK_CPPFLAGS:=-D$(PROJECT)_IMPORTS $($(PROJECT)_CHECK_CPPFLAGS)
 endif
endif

CHECK_SRC:=\
	src/main.c\

CHECK_HDR:=\

# }

ifeq ($(BITS),64) # {
 DEFS:=-D$(UPROJECT)_TARGET_BIT=64 $(DEFS)
else
 DEFS:=-D$(UPROJECT)_TARGET_BIT=32 $(DEFS)
endif # }
LNK:=
ifeq ($(TARGET),UNIX) # {
 ifeq ($(MODE),SHARED)
  ifdef PIC2
   CCFLG:=$(CCFLG) -fPIC
  else
   CCFLG:=$(CCFLG) -fpic
  endif
  LNK:=-shared
 endif
else # Windows target
 LNK:=-Wl,--subsystem,windows
 ifeq ($(MODE),SHARED)
  DEFS:=-D$(PROJECT)_EXPORTS $(DEFS)
  LNK:=$(LNK) -shared
 endif
endif # }
ifeq ($(TARGET),UNIX) # {
 ifeq ($(MODE),SHARED)
  LIBFILE:=$(UNISHRFILE)
  LIBNAME:=$(UNISHRNAME)
 else
  LIBFILE:=$(UNISTAFILE)
  LIBNAME:=$(UNISTANAME)
 endif
else # Windows target
 ifeq ($(MODE),SHARED)
  LIBFILE:=$(WINSHRFILE)
  LIBNAME:=$(WINSHRNAME)
 else
  LIBFILE:=$(WINSTAFILE)
  LIBNAME:=$(WINSTANAME)
 endif
endif # }

$(PROJECT)_CFLAGS=-std=c99 $(WRNS) $(INCS) $(CCFLG) $(CCLDFLG) $(CFLAGS)
$(PROJECT)_CPPFLAGS=$(DEFS) $(STCPPFLAGS) $(CPPFLAGS)
$(PROJECT)_LDFLAGS=$(CCLDFLG) $(LNK) $(LDFLAGS)

SRC:=\
	src/brrtools/brrtest.c\
	src/brrtools/brrlib.c\
	src/brrtools/brrlog.c\
	src/brrtools/brrmem.c\
	src/brrtools/brrstr.c\
	#src/brrtools/brrbuffer.c\

HDR:=\
	src/brrtools/brrapi.h\
	src/brrtools/brrplatform.h\
	src/brrtools/brrtypes.h\
	src/brrtools/brrtil.h\
	src/brrtools/brrmap.h\
	src/brrtools/brrtest.h\
	src/brrtools/brrlib.h\
	src/brrtools/brrlog.h\
	src/brrtools/brrmem.h\
	src/brrtools/brrstr.h\
	#src/brrtools/brrbuffer.h\

