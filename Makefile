.POSIX:
.SUFFIXES:

all:
include config.mk
include aio_gen.mk

all: options $(OUTPUT_FILE)
options:
	@echo "<------------------------>"
	@echo "$(PROJECT) Build Settings:"
	@echo "Project name    : $(PROJECT)"
	@echo "Project binary  : $(TARGET_NAME)"
	@echo "Ultimate output : $(OUTPUT_FILE)"
	@echo ""
	@echo "Build Host      : $(HOST), $(HOST_BIT)-bit"
	@echo "Build Target    : $(TARGET), $(TARGET_BIT)-bit"
	@echo "Build Mode      : $(TARGET_MODE)"
	@echo "Build Tree      : $(BUILD_TREE)"
	@echo "Debug Build     : $(if $(DEBUG),On,Off)"
	@echo "Memcheck Build  : $(if $(MEMCHECK),On,Off)"
	@echo ""
	@echo "Includes        : $(_cc_includes)"
	@echo "Warnings        : $(_cc_warnings)"
	@echo "Defines         : $(_cc_defines)"
	@echo "CC              : $(CC)"
	@echo "AR              : $(AR)"
	@echo "DllTool         : $(DLLTOOL)"
	@echo "Compilation Flags   : $(PROJECT_CFLAGS)"
	@echo "Preprocessing Flags : $(PROJECT_CPPFLAGS)"
	@echo "Linking Flags       : $(PROJECT_LDFLAGS)"
	@echo "<========================>"

$(call _aio_settings,,$(OUTPUT_DIRECTORY),$(SRC))
$(call _aio_rules,,$(OUTPUT_DIRECTORY),$(SRC_DIR),$(PROJECT_CPPFLAGS) $(PROJECT_CFLAGS))
$(call _aio_requirements,,$(addprefix $(SRC_DIR)/,$(HDR) $(HDR_NOINSTALL)) $(MAKE_DEPS))
$(call _aio_directory_rules,)
$(call _aio_phony_rules,)
$(call _aio_clean_rules,)

$(OUTPUT_FILE): options | obj
ifeq ($(TARGET_MODE),SHARED)
 ifeq ($(TARGET),UNIX)
	$(CC) $(OBJ) -o '$@' $(PROJECT_LDFLAGS)
 else
	$(CC) $(OBJ) -o '$@' $(PROJECT_LDFLAGS) -Wl,--output-def,$(OUTPUT_DEF)
	$(CC) $(OBJ) -o '$@' $(PROJECT_LDFLAGS) -Wl,--kill-at
	$(DLLTOOL) --kill-at -d $(OUTPUT_DEF) -D $(OUTPUT_FILE) -l $(OUTPUT_IMPORT)
 endif
else
	$(AR) -crs $(OUTPUT_FILE) $(OBJ)
endif

clean: clean-aio
	@$(RM_FILE) $(OUTPUT_FILE) 2>$(NULL) || :
ifeq ($(TARGET),WINDOWS)
	@$(RM_FILE) $(OUTPUT_DEF) 2>$(NULL) || :
	@$(RM_FILE) $(OUTPUT_IMPORT) 2>$(NULL) || :
endif
	@$(RM_EMPTY_DIR) $(OUTDIR) 2>$(NULL) || :
again: clean all

.PHONY: all clean again options

install-libs:
	@$(MK_DIR) $(prefix)/lib
	@$(COPY_FILE) $(OUTPUT_FILE) $(prefix)/lib/
ifeq ($(TARGET),WINDOWS)
 ifeq ($(TARGET_MODE),SHARED)
	@$(COPY_FILE) $(OUTPUT_DEF) $(prefix)/lib/
	@$(COPY_FILE) $(OUTPUT_IMPORT) $(prefix)/lib/
 endif
endif

_source_headers := $(addprefix $(SRC_DIR)/,$(HDR))
_target_headers := $(addprefix $(prefix)/include/,$(HDR))
_target_headers_directories := $(call _get_directories,$(_target_headers))
_make_rule = $(eval $1: $2)
$(call _map_two,_make_rule,$(_target_headers),$(_source_headers))
$(_target_headers_directories): ; @$(MK_DIR) '$@'
$(_target_headers): | $(_target_headers_directories)
	@$(COPY_FILE) $^ $@
install-headers: $(_target_headers)
install: all install-libs install-headers
ifeq ($(HOST),UNIX)
 ifeq ($(TARGET),UNIX)
  ifneq ($(DO_LDCONFIG),0)
	$(LDCONFIG) $(realpath $(prefix)) || :
  endif
 endif
endif
.PHONY: install install-libs install-headers

uninstall-lib:
	@$(RM_FILE) $(prefix)/lib/$(TARGET_NAME) 2>$(NULL) || :
ifeq ($(TARGET),WINDOWS)
 ifeq ($(TARGET_MODE),SHARED)
	@$(RM_FILE) $(prefix)/lib/$(TARGET_DEF) 2>$(NULL) || :
	@$(RM_FILE) $(prefix)/lib/$(TARGET_IMPORT) 2>$(NULL) || :
 endif
endif
uninstall-headers:
	@$(RM_FILE) $(_target_headers) 2>$(NULL) || :
	@$(RM_EMPTY_DIR) $(call _get_directories,$(_target_headers)) 2>$(NULL) || :
uninstall: uninstall-lib uninstall-headers
ifeq ($(HOST),UNIX)
 ifeq ($(TARGET),UNIX)
  ifneq ($(DO_LDCONFIG),0)
	$(LDCONFIG) $(realpath $(prefix)) || :
  endif
 endif
endif
.PHONY: uninstall uninstall-lib uninstall-headers
nop: ; @echo Nop!
