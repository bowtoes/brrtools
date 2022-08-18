.POSIX:
.SUFFIXES:
all:

include config.mk
include help.mk

info: _delim_1 _info _delim_2
build-info: _delim_1 _build_info _delim_2
output-info: _delim_1 _output_info _delim_2
all-info: _delim_1 _info _build_info _output_info _delim_2
.PHONY: info build_info output_info all_info

all: info $(project)
setup:
	@$(mk_dir_tree) $(build_directories) 2>$(null) ||:
.PHONY: all setup

$(ass_out_dir)/%.s: $(src_dir)/%.c ; $(cc_custom) $(project_cppflags) $(project_cflags) -S $< -o $@
$(int_out_dir)/%.e: $(src_dir)/%.c ; $(cc_custom) $(project_cppflags) $(project_cflags) -E $< -o $@
$(obj_out_dir)/%.o: $(src_dir)/%.c ; $(cc_custom) $(project_cppflags) $(project_cflags) -c $< -o $@
$(ass_out) $(int_out) $(obj_out): $(vnd_bins) $(addprefix $(src_dir)/,$(hdrs)) $(makefiles)

ass: info setup $(ass_out)
int: info setup $(int_out)
obj: info setup $(obj_out)
aio: ass int obj
.PHONY: ass int obj aio

$(output_file): vnd $(obj_out) $(makefiles) $(addprefix $(src_dir)/,$(hdrs))
ifeq ($(target_mode),static)
	$(ar_custom) -crs '$@' $(obj_out) $(vnd_bins)
else
 ifeq ($(target),unix)
	$(cc_custom) -o '$@' $(obj_out) $(vnd_bins) $(project_ldflags)
 else
	$(cc_custom) -o '$@' $(obj_out) $(vnd_bins) $(project_ldflags) -Wl,--output-def,'$(output_defines)'
	$(cc_custom) -o '$@' $(obj_out) $(vnd_bins) $(project_ldflags) -Wl,--kill-at
	$(dlltool_custom) --kill-at -d '$(output_defines)' -D '$@' -l '$(output_imports)'
 endif
endif
$(project): setup $(output_file)
.PHONY: $(project)

clean:
	@$(rm_file) '$(output_file)' $(ass_out) $(int_out) $(obj_out) 2>$(null) ||:
	@$(rm_recurse) $(build_directories) 2>$(null) ||:

again: clean vnd-clean-light $(project)

CLEAN: vnd-clean clean
AGAIN: CLEAN all
.PHONY: clean again CLEAN AGAIN

install-lib: all
	@$(mk_dir_tree) '$(prefix)/lib'
	@$(copy_file) '$(output_file)' '$(prefix)/lib'
ifeq ($(target)-$(target_mode),windows-shared)
	$(copy_file) '$(output_defines)' '$(output_imports)' '$(prefix)/lib'
endif
	@$(strip_exe) '$(prefix)/lib/$(output_name)'
uninstall-lib:
	@$(rm_file) '$(prefix)/lib/$(output_name)'
ifeq ($(target)-$(target_mode),windows-shared)
	@$(rm_file) '$(prefix)/lib/$(build_defines)' '$(prefix)/lib/$(build_imports)'
endif

install-hdr:
	@$(mk_dir_tree) $(hdrs_inst_directories)
	@$(foreach hdr,$(hdrs),$(copy_file) '$(src_dir)/$(hdr)' '$(prefix)/include/$(dir $(hdr))';)
uninstall-hdr:
	@$(rm_file) $(hdrs_inst) 2>$(null) ||:
	@$(rm_dir) $(hdrs_inst_directories) 2>$(null) ||:

install: all install-lib install-hdr
ifeq ($(host)-$(target),unix-unix)
 ifneq ($(do_ldconfig),0)
	$(ldconfig) '$(realpath $(prefix))/lib' ||:
 endif
endif
uninstall: uninstall-lib uninstall-hdr
ifeq ($(host)-$(target),unix-unix)
 ifneq ($(do_ldconfig),0)
	$(ldconfig) '$(realpath $(prefix))/lib' ||:
 endif
endif
.PHONY: install-lib uninstall-lib install-hdr uninstall-hdr install uninstall

include tests.mk
