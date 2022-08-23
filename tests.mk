test_srcs :=\
	brrlogtest.c
test_output := do_tests$(if $(target:unix=),.exe)

test_obj_dir ?= $(obj_out_dir)/test
test_obj_out := $(addprefix $(obj_out_dir)/,$(test_srcs:.c=.o))

build_directories := $(sort $(build_directories) $(test_obj_dir))

$(test_output): all $(test_obj_out)
	$(cc_custom) -o '$@' $(test_obj_out) $(output_file) $(project_ldflags)

test: $(test_output)
.PHONY: test

### Vendor shit
vnd:
vnd-clean:
vnd-again: vnd-clean vnd
vnd-clean-light:
.PHONY: vnd vnd-clean vnd-clean-light vnd-again
