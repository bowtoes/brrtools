# Each setting being set by '_verify_setting' or '_ternary_setting' are
# user-settable when calling make; just do 'make ... VARIABLE_NAME=VALUE'
# (note settings whose names start with an underscore are not intended to be overwritten manually)

include util.mk

# BASIC -- Basic project settings and their defaults, all overridable
PROJECT = brrtools
# Uppercase project, used for c-macro prefixes
UPROJECT = BRRTOOLS
TARGET_LIB := $(PROJECT)
TARGET_NAME := lib$(TARGET_LIB)

SRC_DIR = src

MAKE_DEPS := Makefile config.mk util.mk aio_gen.mk

PROJECT_MAJOR := 0
PROJECT_MINOR := 0
PROJECT_REVIS := 0

$(call _verify_setting,INTDIR,,int,) # Subdirectory where intermediates go in the build directory
$(call _verify_setting,OBJDIR,,obj,) # Subdirectory where objects go in the build directory
$(call _verify_setting,ASSDIR,,ass,) # Subdirectory where individual assemblies go in the build directory

SRC:=\
	brrtools/brrlib.c\
	brrtools/brrlog.c\
	brrtools/brrmem.c\
	brrtools/brrmeta.c\
	brrtools/brrpath.c\
	brrtools/brrstg.c\
	brrtools/brrtest.c\
	brrtools/noinstall/statics.c\

HDR:=\
	brrtools/brrapi.h\
	brrtools/brrmap.h\
	brrtools/brrplatform.h\
	brrtools/brrendian.h\
	brrtools/brrdebug.h\
	brrtools/brrtil.h\
	brrtools/brrtypes.h\
	brrtools/brrlib.h\
	brrtools/brrlog.h\
	brrtools/brrmem.h\
	brrtools/brrmeta.h\
	brrtools/brrpath.h\
	brrtools/brrstg.h\
	brrtools/brrtest.h\

HDR_NOINSTALL:=\
	brrtools/noinstall/statics.h\
	brrtools/noinstall/utils.h\

# SYSTEM -- Meta compilation/system settings, overridable
$(call _verify_setting,HOST,_not_host,UNIX,WINDOWS)
$(call _verify_setting,HOST_BIT,_not_host_bit,64,32)

$(call _verify_setting,TARGET,,$(HOST),$(_not_host))
$(call _verify_setting,TARGET_BIT,,$(HOST_BIT),$(_not_host_bit))
$(call _verify_setting,TARGET_MODE,,SHARED,STATIC)

ifeq ($(HOST),WINDOWS)
 ifeq ($(TARGET),UNIX)
  $(error Windows cross-compilation to unix unsupported)
 endif
endif

# Installation prefix
$(call _ternary_setting,prefix,$(HOST),UNIX,/usr/local,$(CURDIR)/install)

# Parallel threads to run for compilation
$(call _ternary_setting,PARALLEL,$(HOST),UNIX,$(shell nproc),1)

$(call _verify_setting,STD,,c99,)
$(call _verify_setting,PEDANTIC,,1,)

# COMMAND -- System commands used by make recipes, overridable (though not recommended)
$(call _ternary_setting,NULL,$(HOST),UNIX,/dev/null,null)
$(call _ternary_setting,RM_FILE,$(HOST),UNIX,rm -v,del)
$(call _ternary_setting,RM_EMPTY_DIR,$(HOST),UNIX,rmdir -v --ignore-fail-on-non-empty,rmdir)
$(call _ternary_setting,RM_FULL_DIR,$(HOST),UNIX,rm -rv,rmdir /s /q)
$(call _ternary_setting,MK_DIR,$(HOST),UNIX,mkdir -pv,helpers/win/mkdir_tree.bat)
$(call _ternary_setting,COPY_FILE,$(HOST),UNIX,cp -fv,copy /y)

# BUILD -- Build directory structure, all overridable
$(call _verify_setting,BUILD_DIR_NAME,,build,)
# Build tree element based on TARGET
$(call _verify_setting,_build_dir_target_uni,,uni,)
$(call _verify_setting,_build_dir_target_win,,win,)
$(call _ternary_setting,BUILD_DIR_TARGET,$(TARGET),UNIX,$(_build_dir_target_uni),$(_build_dir_target_win))
# Build tree element based on TARGET_MODE
$(call _verify_setting,_build_dir_mode_dyn,,dyn,)
$(call _verify_setting,_build_dir_mode_sta,,sta,)
$(call _ternary_setting,BUILD_DIR_MODE,$(TARGET_MODE),SHARED,$(_build_dir_mode_dyn),$(_build_dir_mode_sta))
# Build tree element based on TARGET_BIT
$(call _verify_setting,_build_dir_bit_64,,64,)
$(call _verify_setting,_build_dir_bit_32,,32,)
$(call _ternary_setting,BUILD_DIR_BIT,$(TARGET_BIT),64,$(_build_dir_bit_64),$(_build_dir_bit_32))

$(call _verify_setting,BUILD_TREE,,$(BUILD_DIR_NAME)/$(BUILD_DIR_TARGET)/$(BUILD_DIR_MODE)/$(BUILD_DIR_BIT),)
$(call _verify_setting,BUILD_TREE_ROOT,,$(CURDIR),)
$(call _verify_setting,OUTPUT_DIRECTORY,,$(BUILD_TREE_ROOT)/$(BUILD_TREE),)

# Target name extension based on TARGET and TARGET_MODE
$(call _ternary_setting,_target_name_extension_dyn,$(TARGET),UNIX,.so,.dll)
$(call _ternary_setting,_target_name_extension_sta,$(TARGET),UNIX,.a,.lib)
$(call _ternary_join_right,TARGET_NAME,$(TARGET_MODE),SHARED,$(_target_name_extension_dyn),$(_target_name_extension_sta))
$(call _verify_setting,OUTPUT_FILE,,$(OUTPUT_DIRECTORY)/$(TARGET_NAME),)
# Target definitions/imports files for windows TARGET
$(call _ternary_setting,TARGET_DEF,$(TARGET),UNIX,,$(TARGET_NAME).def)
$(call _ternary_setting,TARGET_IMPORT,$(TARGET),UNIX,,$(TARGET_NAME).dll.lib)
$(call _verify_setting,OUTPUT_DEF,,$(OUTPUT_DIRECTORY)/$(TARGET_DEF),)
$(call _verify_setting,OUTPUT_IMPORT,,$(OUTPUT_DIRECTORY)/$(TARGET_IMPORT),)


# CC -- Compilation executables (CC, AR, etc...), overridable
# Appropriate system tuple
$(call _ternary_setting,_sys_arch,$(TARGET_BIT),64,x86_64,i686)
$(call _ternary_setting,_sys_vendor,$(TARGET),UNIX,pc,w64)
$(call _ternary_setting,_sys_os,$(TARGET),UNIX,linux-gnu,mingw32)
_sys = $(_sys_arch)-$(_sys_vendor)-$(_sys_os)

# Specify 'DEFAULT_CC' to decide the name of your compiler (and build the appropriate
# executable name) or specify 'CC_CUSTOM' to directly set the compiler executable to use.
DEFAULT_CC:=gcc
ifeq ($(HOST),UNIX)
 $(call _ternary_setting,CC_CUSTOM,$(TARGET),UNIX,$(DEFAULT_CC),$(_sys)-$(DEFAULT_CC))
else
 $(call _verify_setting,CC_CUSTOM,,$(DEFAULT_CC),)
endif
CC = $(CC_CUSTOM)

# Same as above
DEFAULT_AR:=gcc-ar
ifeq ($(HOST),UNIX)
 $(call _ternary_setting,AR_CUSTOM,$(TARGET),UNIX,$(DEFAULT_AR),$(_sys)-$(DEFAULT_AR))
else
 $(call _verify_setting,AR_CUSTOM,,$(DEFAULT_AR),)
endif
AR = $(AR_CUSTOM)

# Same as above, except UNIX has no DLLTOOL, so that is left blank by default
# if UNIX is the target
$(call _ternary_setting,DEFAULT_DLLTOOL,$(TARGET),UNIX,,dlltool)
ifeq ($(HOST),UNIX)
 $(call _ternary_setting,DLLTOOL_CUSTOM,$(TARGET),UNIX,$(DEFAULT_DLLTOOL),$(_sys)-$(DEFAULT_DLLTOOL))
else
 $(call _verify_setting,DLLTOOL_CUSTOM,,$(DEFAULT_DLLTOOL),)
endif
DLLTOOL = $(DLLTOOL_CUSTOM)

$(call _ternary_setting,DEFAULT_LDCONFIG,$(TARGET),UNIX,ldconfig,)
$(call _verify_setting,LDCONFIG_CUSTOM,,$(DEFAULT_LDCONFIG),)
LDCONFIG = $(LDCONFIG_CUSTOM)

_cc_includes = -I$(SRC_DIR)
_cc_warnings = -Wall -Wextra\
	-Werror=implicit-function-declaration -Werror=missing-declarations\
	-Wno-unused-function -Wno-sign-compare -Wno-unused-parameter\

_cc_defines = \
	-D$(UPROJECT)_MAJOR=$(PROJECT_MAJOR)\
	-D$(UPROJECT)_MINOR=$(PROJECT_MINOR)\
	-D$(UPROJECT)_REVIS=$(PROJECT_REVIS)\
	-D$(UPROJECT)_VERSION='$(PROJECT_MAJOR).$(PROJECT_MINOR).$(PROJECT_REVIS)'

ifneq ($(PEDANTIC),0)
 _cc_warnings := -pedantic -pedantic-errors -Wpedantic $(_cc_warnings)
endif

$(call _binary_append,_cc_defines,DEBUG,-D$(UPROJECT)_DEBUG,)
$(call _binary_append,_cc_defines,MEMCHECK,-D$(UPROJECT)_MEMCHECK,)
$(call _ternary_append,_cc_defines,$(TARGET_BIT),64,-D$(UPROJECT)_TARGET_BIT=64,-D$(UPROJECT)_TARGET_BIT=32)
$(call _ternary_prepend,_cc_defines,$(TARGET),UNIX,-D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L,-DWIN32_LEAN_AND_MEAN)
ifeq ($(TARGET),WINDOWS)
 $(call _ternary_prepend,_cc_defines,$(TARGET_MODE),SHARED,-D$(UPROJECT)_EXPORTS,)
endif

# _cc_optimize: Compilation optimization or other settings
# _cc_performance: Compilation & linkage shared parameters (I can't think of a better name)
$(call _binary_setting,_cc_optimize,DEBUG,-O0 -g,-O3 -Ofast)
$(call _ternary_setting,_cc_performance,$(TARGET_MODE),SHARED,-s,-static-pie)
$(call _binary_append,_cc_performance,MEMCHECK,-pg -no-pie,)
$(call _ternary_append,_cc_performance,$(TARGET_BIT),64,-m64,-m32)

ifeq ($(TARGET),UNIX)
 ifeq ($(TARGET_MODE),SHARED)
  $(call _binary_append,_cc_optimize,PIC2,-fPIC,-fpic)
 endif
endif

# _cc_linkage: Linkage parameters
$(call _ternary_setting,_cc_linkage,$(TARGET_MODE),SHARED,-shared)
$(call _ternary_prepend,_cc_linkage,$(TARGET),WINDOWS,-Wl$(_util_comma)--subsystem$(_util_comma)windows,)
ifeq ($(TARGET),WINDOWS)
 $(call _ternary_prepend,_cc_defines,$(TARGET_MODE),SHARED,-D$(UPROJECT)_EXPORTS,)
endif

ifndef DEBUG
 ifndef MEMCHECK
  $(call _binary_prepend,_cc_linkage,NO_STRIP,,-Wl$(_util_comma)--strip-all)
 endif
endif

PROJECT_CFLAGS = -std=$(STD) $(_cc_warnings) $(_cc_includes) $(_cc_optimize) $(_cc_performance) $(CFLAGS)
PROJECT_CPPFLAGS = $(_cc_defines) $(STCPPFLAGS) $(CPPFLAGS)
PROJECT_LDFLAGS = $(_cc_performance) $(_cc_linkage) $(LDFLAGS)
