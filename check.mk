include config.mk

CHECK_TARGET_NAME := check

CHECK_SRC :=\
	main.c\
	tests/pack.c\

CHECK_HDR :=\
	tests/pack.h\

$(call _verify_setting,CHECK_OUTPUT_DIRECTORY,,$(BUILD_TREE_ROOT)/$(BUILD_TREE)/check,)

# Target name suffix based on TARGET_MODE
$(call _verify_setting,_check_target_name_mode_dyn,,,)
$(call _verify_setting,_check_target_name_mode_sta,,_s,)
$(call _ternary_join_right,CHECK_TARGET_NAME,$(TARGET_MODE),SHARED,$(_check_target_name_mode_dyn),$(_check_target_name_mode_sta))
# Target name suffix based on TARGET_BIT
$(call _verify_setting,_check_target_name_bit_64,,-x64,)
$(call _verify_setting,_check_target_name_bit_32,,-x86,)
$(call _ternary_join_right,CHECK_TARGET_NAME,$(TARGET_BIT),$(HOST_BIT),,$(_check_target_name_bit_$(_not_hostbit)))
# Target name extension based on TARGET
$(call _verify_setting,_check_target_name_extension_uni,,,)
$(call _verify_setting,_check_target_name_extension_win,,.exe,)
$(call _ternary_join_right,CHECK_TARGET_NAME,$(TARGET),UNIX,$(_check_target_name_extension_uni),$(_check_target_name_extension_win))

$(call _verify_setting,CHECK_OUTPUT_FILE,,$(CHECK_OUTPUT_DIRECTORY)/$(CHECK_TARGET_NAME),)

_check_includes = $(_cc_includes)
_check_warnings = $(_cc_warnings)
_check_defines =

$(call _ternary_prepend,_check_defines,$(TARGET),UNIX,-D_XOPEN_SOURCE=500 -D_POSIX_C_SOURCE=200112L,)

ifndef DEBUG
 _check_optimize := -O3 -Ofast
 ifeq ($(TARGET_MODE),SHARED)
  _check_performance := -s
 endif
else
 _check_optimize := -O0 -g
 ifndef MEMCHECK
  _check_performance := -pg -no-pie
 endif
endif
$(call _ternary_append,_check_performance,$(TARGET_BIT),64,-m64,-m32)

$(call _ternary_prepend,_check_linkage,$(TARGET),WINDOWS,-Wl$(_util_comma)--subsystem$(_util_comma)windows,)
ifeq ($(TARGET),WINDOWS)
 $(call _ternary_prepend,_check_defines,$(TARGET_MODE),SHARED,-D$(UPROJECT)_IMPORTS,)
endif

ifndef DEBUG
 ifndef MEMCHECK
  $(call _binary_prepend,_check_linkage,NO_STRIP,,-Wl$(_util_comma)--strip-all)
 endif
endif

$(call _ternary_append,_check_linkage,$(TARGET_MODE),SHARED,-L$(OUTPUT_DIRECTORY) -lbrrtools,$(OUTPUT_FILE))

CHECK_CFLAGS = -std=$(STD) $(_check_warnings) $(_check_includes) $(_check_optimize) $(_check_performance) $(CFLAGS)
CHECK_CPPFLAGS = $(_check_defines) $(STCPPFLAGS) $(CPPFLAGS)
CHECK_LDFLAGS = $(_check_performance) $(_check_linkage) $(LDFLAGS)

$(call _aio_settings,CHECK_,$(CHECK_OUTPUT_DIRECTORY),$(CHECK_SRC))
$(call _aio_rules,CHECK_,$(CHECK_OUTPUT_DIRECTORY),$(SRC_DIR),$(CHECK_CPPFLAGS) $(CHECK_CFLAGS))
$(call _aio_requirements,CHECK_,$(addprefix $(SRC_DIR)/,$(CHECK_HDR)) $(MAKE_DEPS))
$(call _aio_directory_rules,CHECK_)
$(call _aio_phony_rules,CHECK_)
$(call _aio_clean_rules,CHECK_)

check: all $(CHECK_OUTPUT_FILE)

$(CHECK_OUTPUT_FILE): $(MAKE_DEPS) | CHECK_obj
	$(CC) $(CHECK_OBJ) -o '$@' $(CHECK_LDFLAGS)

clean-check: clean-CHECK_aio
	$(RM_FILE) $(CHECK_OUTPUT_FILE) 2>$(NULL) || :
