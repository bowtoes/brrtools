# Each setting being set by '_verify_setting' or '_ternary_setting' are
# user-settable when calling make; just do 'make ... VARIABLE_NAME=VALUE'
# (note settings whose names start with an underscore are not intended to be overwritten manually)

include util.mk

# BASIC -- Basic project settings and their defaults, all overridable
PROJECT = $(notdir $(CURDIR))
# Uppercase project, used for c-macro prefixes
UPROJECT = BRRTOOLS

SRC_DIR = src

MAKE_DEPS := Makefile config.mk util.mk aio_gen.mk

PROJECT_MAJOR := 0
PROJECT_MINOR := 0
PROJECT_REVIS := 0

SRC :=\
	brrtools/brrcon.c\
	brrtools/brrhash.c\
	brrtools/brrheap.c\
	brrtools/brrlib.c\
	brrtools/brrlog.c\
	brrtools/brrnum.c\
	brrtools/brrpath.c\
	brrtools/brrrand.c\
	brrtools/brrstringr.c\
	brrtools/brrtest.c\
	brrtools/brrtime.c\

HDR :=\
	brrtools/brrapi.h\
	brrtools/brrcon.h\
	brrtools/brrdebug.h\
	brrtools/brrendian.h\
	brrtools/brrhash.h\
	brrtools/brrheap.h\
	brrtools/brrlib.h\
	brrtools/brrlog.h\
	brrtools/brrmacro.h\
	brrtools/brrmacro_map.h\
	brrtools/brrnum.h\
	brrtools/brrpath.h\
	brrtools/brrplatform.h\
	brrtools/brrrand.h\
	brrtools/brrstringr.h\
	brrtools/brrtest.h\
	brrtools/brrtime.h\
	brrtools/brrtypes.h\

# Set the default installation prefix to whatever, or just override 'prefix' in the make command to set it outright.
DEFAULT_UNIX_PREFIX=/usr/local
DEFAULT_WINDOWS_PREFIX=$(CURDIR)/install
# Set the default host system type to one of 'UNIX' or 'WINDOWS'. Determines the default valid value of 'HOST'.
DEFAULT_HOST=UNIX
# Set the default host bit to one of '64' or '32. Determines the default valid value of 'HOST_BIT'.
DEFAULT_HOST_BIT=64
# Set the default target system type to one of 'UNIX' or 'WINDOWS', or to 'DEFAULT_HOST'. Determines the default valid value of 'TARGET'.
DEFAULT_TARGET=$(DEFAULT_HOST)
# Set the default target bit to one of '64' or '32', or to DEFAULT_HOST_BIT.  Determines the default valid value of 'TARGET_BIT'.
DEFAULT_TARGET_BIT=$(DEFAULT_HOST_BIT)
# Set the default target mode to one of 'STATIC' or 'SHARED'. Determines the default valid value of 'TARGET_MODE'.
DEFAULT_TARGET_MODE=SHARED

# Subdirectory where intermediates (.e) go in the build directory
INTDIR=int
# Subdirectory where objects (.o) go in the build directory
OBJDIR=obj
# Subdirectory where assemblies (.s) go in the build directory
ASSDIR=ass

# Verify default host/target settings are valid
$(call _verify_setting,DEFAULT_HOST,_not_default_host,UNIX,WINDOWS)
$(call _verify_setting,DEFAULT_HOST_BIT,_not_default_host_bit,64,32)
$(call _verify_setting,DEFAULT_TARGET,_not_default_target,UNIX,WINDOWS)
$(call _verify_setting,DEFAULT_TARGET_MODE,_not_default_target_mode,SHARED,STATIC)
$(call _verify_setting,DEFAULT_TARGET_BIT,_not_default_target_bit,64,32)

# Verify derived/manual host/target settings are valid
$(call _verify_setting,HOST,_not_host,$(DEFAULT_HOST),$(_not_default_host))
$(call _verify_setting,HOST_BIT,_not_host_bit,$(DEFAULT_HOST_BIT),$(_not_default_host_bit))
$(call _verify_setting,TARGET,_not_target,$(DEFAULT_TARGET),$(_not_default_target))
$(call _verify_setting,TARGET_MODE,,$(DEFAULT_TARGET_MODE),$(_not_default_target_mode))
$(call _verify_setting,TARGET_BIT,,$(DEFAULT_TARGET_BIT),$(_not_default_target_bit))

# Installation prefix based on host system
prefix=$(DEFAULT_$(HOST)_PREFIX)

DEFAULT_STD=c11
STD=$(DEFAULT_STD)
# Whether to enable pedantic compilation; set to specifically '0' to disable
DEFAULT_PEDANTIC=1
PEDANTIC=$(DEFAULT_PEDANTIC)
# Whether to enabled stripping of output binaries; set specifically to '0' to disable
# Stripping is not enabled when DEBUG or MEMCHECK are set, regardless.
DEFAULT_DOSTRIP=1
DOSTRIP=$(DEFAULT_DOSTRIP)
DEFAULT_DOLDCONFIG=1
DOLDCONFIG=$(DEFAULT_DOLDCONFIG)

# SYSTEM -- Meta compilation/system settings, overridable
ifeq ($(HOST),WINDOWS)
 ifeq ($(TARGET),UNIX)
  $(error Windows cross-compilation to unix unsupported)
 endif
endif

# BUILD -- Build directory structure, all overridable
# Default name of the root build directory
DEFAULT_BUILD_ROOT_NAME=build
BUILD_ROOT_NAME=$(DEFAULT_BUILD_ROOT_NAME)
# Default names for the TARGET build subdirectory
DEFAULT_BUILD_SUBDIR_UNIX=uni
DEFAULT_BUILD_SUBDIR_WINDOWS=win
BUILD_SUBDIR_TARGET=$(DEFAULT_BUILD_SUBDIR_$(TARGET))
# Default names for the TARGET_MODE build subdirectory
DEFAULT_BUILD_SUBDIR_SHARED=dyn
DEFAULT_BUILD_SUBDIR_STATIC=sta
BUILD_SUBDIR_MODE=$(DEFAULT_BUILD_SUBDIR_$(TARGET_MODE))
# Default names for the TARGET_BIT build subdirectory
DEFAULT_BUILD_SUBDIR_64BIT=64
DEFAULT_BUILD_SUBDIR_32BIT=32
BUILD_SUBDIR_BIT=$(DEFAULT_BUILD_SUBDIR_$(TARGET_BIT)BIT)

# If you don't want to deal with all that nonsense above, you can just override
# the final directories on the commandline with these variables.
BUILD_TREE=$(BUILD_ROOT_NAME)/$(BUILD_SUBDIR_TARGET)/$(BUILD_SUBDIR_MODE)/$(BUILD_SUBDIR_BIT)
BUILD_TREE_ROOT=$(CURDIR)
OUTPUT_DIRECTORY=$(BUILD_TREE_ROOT)/$(BUILD_TREE)

TARGET_LIB := $(PROJECT)
TARGET_NAME_BASE := lib$(TARGET_LIB)

DEFAULT_TARGET_PART_UNIX_SHARED=.so
DEFAULT_TARGET_PART_UNIX_STATIC=.a
DEFAULT_TARGET_PART_WINDOWS_SHARED=.dll
DEFAULT_TARGET_PART_WINDOWS_STATIC=.lib

DEFAULT_TARGET_PART=$(DEFAULT_TARGET_PART_$(TARGET)_$(TARGET_MODE))

TARGET_NAME=$(TARGET_NAME_BASE)$(DEFAULT_TARGET_PART)
OUTPUT_FILE=$(OUTPUT_DIRECTORY)/$(TARGET_NAME)

TARGET_DEF=$(TARGET_NAME_BASE).def
OUTPUT_DEF=$(OUTPUT_DIRECTORY)/$(TARGET_DEF)
TARGET_IMPORT=$(TARGET_NAME_BASE).dll.lib
OUTPUT_IMPORT=$(OUTPUT_DIRECTORY)/$(TARGET_IMPORT)

# CC -- Compilation executables (CC, AR, etc...), overridable
#
# Appropriate system tuple, only overwrite the default values

# Currently, these defaults are set for compilation on a unix HOST, not sure what
# to use for windows HOST.
DEFAULT_ARCH_64BIT=x86_64
DEFAULT_ARCH_32BIT=i686
_sys_arch=$(DEFAULT_ARCH_$(TARGET_BIT)BIT)
DEFAULT_VENDOR_UNIX=pc
DEFAULT_VENDOR_WINDOWS=w64
_sys_vendor=$(DEFAULT_VENDOR_$(TARGET))
DEFAULT_OS_UNIX=linux-gnu
DEFAULT_OS_WINDOWS=mingw32
_sys_os=$(DEFAULT_OS_$(TARGET))
_sys=$(_sys_arch)-$(_sys_vendor)-$(_sys_os)


# $1 = variable name
# $2 = default exe name (for linux)
# $3 = system tuple (_sys)
_gen_compile_exe = $(eval \
DEFAULT_$1=$2$(_util_newline)\
ifeq ($(HOST),UNIX)$(_util_newline)\
 $$(call _ternary_setting,$1_CUSTOM,$(TARGET),UNIX,$$(DEFAULT_$1),$3-$$(DEFAULT_$1))$(_util_newline)\
else$(_util_newline)\
 $$(call _verify_setting,$1_CUSTOM,,$$(DEFAULT_$1),)$(_util_newline)\
endif$(_util_newline)\
$1=$$($1_CUSTOM)$(_util_newline)\
)

$(call _gen_compile_exe,CC,gcc,$(_sys))
$(call _gen_compile_exe,AR,gcc-ar,$(_sys))
$(call _gen_compile_exe,DLLTOOL,dlltool,$(_sys))

DEFAULT_LDCONFIG_UNIX=ldconfig
DEFAULT_LDCONFIG_WINDOWS=
LDCONFIG_CUSTOM=$(DEFAULT_LDCONFIG_$(TARGET))
LDCONFIG=$(LDCONFIG_CUSTOM)

# COMMAND -- System commands used by make recipes, overridable (though not recommended)
$(call _ternary_setting,NULL,$(HOST),UNIX,/dev/null,null)
$(call _ternary_setting,RM_FILE,$(HOST),UNIX,rm -v,del)
$(call _ternary_setting,RM_EMPTY_DIR,$(HOST),UNIX,rmdir -v --ignore-fail-on-non-empty,rmdir)
$(call _ternary_setting,RM_FULL_DIR,$(HOST),UNIX,rm -rv,rmdir /s /q)
$(call _ternary_setting,MK_DIR,$(HOST),UNIX,mkdir -pv,helpers/win/mkdir_tree.bat)
$(call _ternary_setting,COPY_FILE,$(HOST),UNIX,cp -fv,copy /y)

_cc_includes=-I$(SRC_DIR)
_cc_warnings=-Wall -Wextra\
	-Werror=implicit-function-declaration -Werror=missing-declarations\
	-Wno-unused-function -Wno-sign-compare -Wno-unused-parameter\

_cc_defines:=\
	-D$(UPROJECT)_MAJOR=$(PROJECT_MAJOR)\
	-D$(UPROJECT)_MINOR=$(PROJECT_MINOR)\
	-D$(UPROJECT)_REVIS=$(PROJECT_REVIS)\
	-D$(UPROJECT)_VERSION='$(PROJECT_MAJOR).$(PROJECT_MINOR).$(PROJECT_REVIS)'\
	-D$(UPROJECT)_HOST_$(HOST)\
	-D$(UPROJECT)_HOST_BIT=$(HOST_BIT)\
	-D$(UPROJECT)_TARGET_$(TARGET)\
	-D$(UPROJECT)_TARGET_BIT=$(TARGET_BIT)\
	-D$(UPROJECT)_TARGET_MODE_$(TARGET_MODE)\
	-D$(UPROJECT)_DEBUG=$(if $(DEBUG),1,0)\
	-D$(UPROJECT)_MEMCHECK=$(if $(MEMCHECK),1,0)\

ifneq ($(PEDANTIC),0)
 _cc_warnings := -pedantic -pedantic-errors -Wpedantic $(_cc_warnings)
endif

ifeq ($(TARGET),UNIX)
 _cc_defines:=-D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L $(_cc_defines)
else
 _cc_defines:=-DWIN32_LEAN_AND_MEAN $(_cc_defines)
endif

# _cc_optimize: Compilation optimization or other settings
# _cc_performance: Compilation & linkage shared parameters (I can't think of a better name)
ifndef DEBUG
 _cc_optimize:=-O3 -Ofast
 ifeq ($(TARGET_MODE),SHARED)
  _cc_performance:=-s
 else
  _cc_performance:=-static-pie
 endif
else
 _cc_optimize:=-O0 -g
 ifndef MEMCHECK
  _cc_performance:=-pg -no-pie
 endif
endif
_cc_performance += -m$(TARGET_BIT)

ifeq ($(TARGET),UNIX)
 ifeq ($(TARGET_MODE),SHARED)
  $(call _binary_append,_cc_optimize,PIC2,-fPIC,-fpic)
 endif
endif

ifeq ($(TARGET_MODE),SHARED)
 _cc_linkage += -shared
endif
ifeq ($(TARGET),WINDOWS)
 _cc_linkage += -Wl,--subsystem,windows
 ifeq ($(TARGET_MODE),SHARED)
  _cc_defines += -D$(UPROJECT)_EXPORTS
 endif
endif

ifndef DEBUG
 ifndef MEMCHECK
  ifneq ($(DOSTRIP),0)
   _cc_linkage += -Wl,--strip-all
  endif
 endif
endif

PROJECT_CFLAGS = -std=$(STD) $(_cc_warnings) $(_cc_includes) $(_cc_optimize) $(_cc_performance) $(CFLAGS)
PROJECT_CPPFLAGS = $(_cc_defines) $(STCPPFLAGS) $(CPPFLAGS)
PROJECT_LDFLAGS = $(_cc_performance) $(_cc_linkage) $(LDFLAGS)
