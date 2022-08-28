# This file is the primary configuration point for the build process, including
# various compilation flags, output binary/ies name/s, project version, etc.

include platform.mk

project ?= brrtools
# prefix for project-specific macro defines
uproject ?= BRR

# used as a prerequisite in some recipes, to make sure that updating the makefiles
# triggers a rebuild
override makefiles := platform.mk config.mk Makefile

override project_major := 0
override project_minor := 0
override project_revis := 2
override project_letter :=
override project_version := $(project_major).$(project_minor).$(project_revis)$(if $(project_letter),/$(project_letter),)
override project_date := $(shell git show -s --date=format:'%Y/%m/%d %l:%M%p' --format=%ad || echo "")

src_dir := src
#vnd_dir := vnd

srcs :=\
	brrtools/_brrlib.c\
	brrtools/brrarray.c\
	brrtools/brrlog.c\
	brrtools/brrstringr.c\

hdrs :=\
	brrtools/brrapi.h\
	brrtools/brrcon.h\
	brrtools/brrarray.h\
	brrtools/brrdbg.h\
	brrtools/brrhash.h\
	brrtools/brrlog.h\
	brrtools/brrmacro.h\
	brrtools/brrnum.h\
	brrtools/brrplat.h\
	brrtools/brrrand.h\
	brrtools/brrstringr.h\
	brrtools/brrtime.h\
	brrtools/brrtypes.h\

## These variables must be set to exclusively 0 to disable them
# Be pedantic about the source files when compiling
pedantic ?= 1
# Strip executable(s) when installing
do_strip ?= 1
do_ldconfig ?= 0
# Enable debug compiler flags
debug ?= 0
extra_debug ?= 0
# Enable valgrind memcheck-compitable debug flags (only takes effect when debug != 0)
memcheck ?= 0
tcmalloc ?= 0

## Toolchain flags
# c standard to use
std ?= c11

c_links =
c_includes = '$(src_dir)'
c_warnings =\
	-Wall\
	-Wextra\
	-Wno-unused-function\
	-Wno-sign-compare\
	-Wno-unused-parameter\
	-Wno-unused-variable\
	-Werror=implicit-function-declaration\
	-Werror=missing-declarations\

c_defines := \
	-D$(uproject)_major=$(project_major)\
	-D$(uproject)_minor=$(project_minor)\
	-D$(uproject)_revis=$(project_revis)\
	-D$(uproject)_version='"$(project_version)"'\
	-D$(uproject)_host_$(host)\
	-D$(uproject)_host_bit=$(host_bit)\
	-D$(uproject)_target_$(target)\
	-D$(uproject)_target_bit=$(target_bit)\
	-D$(uproject)_target_mode_$(target_mode)\

# These values are filled in in the Makefile, where vendor build stuffs is done.
#vnd_bins :=
#vnd_links :=
#vnd_includes :=
#vnd_defines :=
#vnd_cflags :=

ifeq ($(target),unix)
 # yeah yeah, gnu isn't portable I get it
 # todo have to reimplement dirtree walking manually, with opendir or something like that
 c_defines := -D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L -D_GNU_SOURCE $(c_defines)
else
 c_defines := -DWIN32_LEAN_AND_MEAN $(c_defines)
endif

ifneq ($(pedantic),0)
 c_warnings := -pedantic -pedantic-errors -Wpedantic $(c_warnings)
 c_defines += -D$(uproject)_pedantic
endif

c_optimization :=
c_performance :=
ifeq ($(debug),0)
 c_optimization += -O3 -Ofast
 c_links += -Wl,--strip-all,--discard-all
 ifeq ($(target_mode),shared)
  c_performance += -s
 else
  c_performance += -static-pie
 endif
else
 c_optimization += -O0 -g
 c_defines += -D$(uproject)_debug
 ifeq ($(memcheck),0)
  c_performance += -pg -no-pie
 else
  c_defines += -D$(uproject)_memcheck
 endif
 ifneq ($(extra_debug),0)
  c_defines += -D$(uproject)_extra_debug
 endif
endif
c_performance += -m$(target_bit)

ifeq ($(target_mode),shared)
 c_links += -shared
 ifeq ($(target),windows)
  c_links += -Wl,--subsystem,windows
  c_defines += -D$(uproject)_exports
 endif
#else
# c_links += -static-pie
endif

# TODO I desperately need to find out this stuff, because it can cause many issues that I don't understand
# for the life of me
ifeq ($(target_mode),shared)
 PIC ?= PIC
endif
ifdef PIC
 ifneq ($(PIC),pic)
  ifneq ($(PIC),PIC)
   ifneq ($(PIC),pie)
    ifneq ($(PIC),PIE)
     override PIC:=PIC
    endif
   endif
  endif
 endif
endif
ifneq ($(PIC),)
 c_performance += -f$(PIC)
endif

ifeq ($(target_mode),shared)
 c_links += -shared
endif
ifeq ($(target),windows)
 c_links += -Wl,--subsystem,windows
 ifeq ($(target_mode),shared)
  c_defines += -D$(UPROJECT)_exports
 endif
endif

extra_links :=
ifneq ($(tcmalloc),0)
 extra_links += -ltcmalloc
 c_defines += -D$(uproject)_tcmalloc
endif

project_cflags = \
	-std=$(std)\
	$(c_warnings)\
	$(c_optimization)\
	$(c_performance)\
	$(vnd_cflags)\
	$(CFLAGS)
project_cppflags = \
	$(c_defines)\
	$(vnd_defines)\
	$(addprefix -I,$(c_includes) $(vnd_includes))\
	$(STCPPFLAGS)\
	$(CPPFLAGS)
project_ldflags = \
	$(c_performance)\
	$(c_links)\
	$(vnd_links)\
	$(extra_links)\
	$(LDFLAGS)

### Build-output variables
$(eval $(call dUnixWindowsVar,prefix,/usr/local,$(CURDIR)/install,target))

build_subdir_target_unix ?= uni
build_subdir_target_windows ?= win
build_subdir_target ?= $(build_subdir_target_$(target))

build_subdir_mode_shared ?= shared
build_subdir_mode_static ?= static
build_subdir_mode ?= $(build_subdir_mode_$(target_mode))

build_subdir_bit_64 ?= 64
build_subdir_bit_32 ?= 32
build_subdir_bit ?= $(build_subdir_bit_$(target_bit))

build_root ?= $(CURDIR)/build
build_tree ?= $(build_subdir_target)/$(build_subdir_bit)/$(build_subdir_mode)
output_directory ?= $(build_root)/$(build_tree)

output_suf_unix_32 ?= _32
output_suf_unix_64 ?=
output_suf_windows_32 ?= _32
output_suf_windows_64 ?=
output_suf ?= $(output_suf_$(target)_$(target_bit))

output_ext_unix_shared ?= .so
output_ext_unix_static ?= .a
output_ext_windows_shared ?= .dll
output_ext_windows_static ?= .lib
output_ext ?= $(output_ext_$(target)_$(target_mode))

output_base_name ?= lib$(project)$(output_suf)
output_name ?= $(output_base_name)$(output_ext)
output_file ?= $(output_directory)/$(output_name)

# windows libraries only
build_defines ?= $(output_base_name).def
output_defines ?= $(output_directory)/$(build_defines)
build_imports ?= $(output_base_name).dll.lib
output_imports ?= $(output_directory)/$(build_imports)

ass_dir ?= ass
ass_out_dir ?= $(output_directory)/$(ass_dir)
ass_out := $(addprefix $(ass_out_dir)/,$(srcs:.c=.s))

int_dir ?= int
int_out_dir ?= $(output_directory)/$(int_dir)
int_out := $(addprefix $(int_out_dir)/,$(srcs:.c=.e))

obj_dir ?= obj
obj_out_dir ?= $(output_directory)/$(obj_dir)
obj_out := $(addprefix $(obj_out_dir)/,$(srcs:.c=.o))

hdrs_inst := $(addprefix $(prefix)/include/,$(hdrs))
hdrs_inst_directories := $(sort $(dir $(hdrs_inst)))

build_directories := $(sort $(dir $(ass_out) $(int_out) $(obj_out)))

# undef config/platform-only defs, they shouldn't be used in the Makefile
override dVerifySetting :=
override dGenExe :=
override dUnixWindowsVar :=
