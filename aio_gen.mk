
# These functions are used to generate the rules used for
# assembling/preprocessing/compiling a set of input files.
#
# After calling these functions in 'Makefile' (and having the $(CC) and
# $(MK_DIR) make variables set), a user can call 'make [ass/int/obj/aio]' to
# assemble/preprocess/compile/all-in-one the input files.

# $1 = setting prefix; PROJECT_..., VENDOR_..., etc...
# $2 = output root
# $3 = source files
# Generates all-in-one variables for compilation
# So they would look like:
#  $1ASS := $(3:.c=.a)
#  $1INT := $(3:.c=.e)
#  $1OBJ := $(3:.c=.o)
_aio_settings = $(eval\
$1ASS := $(addprefix $2/$(ASSDIR)/,$(3:.c=.s))$(_util_newline)\
$1INT := $(addprefix $2/$(INTDIR)/,$(3:.c=.e))$(_util_newline)\
$1OBJ := $(addprefix $2/$(OBJDIR)/,$(3:.c=.o))$(_util_newline)\
$1AIO := $$($1ASS) $$($1INT) $$($1OBJ)$(_util_newline)\
$(_util_newline)\
$1ASS_DIRECTORIES := $$(call _get_directories,$$($1ASS))$(_util_newline)\
$1INT_DIRECTORIES := $$(call _get_directories,$$($1INT))$(_util_newline)\
$1OBJ_DIRECTORIES := $$(call _get_directories,$$($1OBJ))$(_util_newline)\
$1AIO_DIRECTORIES := $$($1ASS_DIRECTORIES) $$($1INT_DIRECTORIES) $$($1OBJ_DIRECTORIES)$(_util_newline)\
)

# $1 = setting prefix
# $2 = output root
# $3 = source file root
# $4 = compilation flags
# Generates the generic rules for producing AIO output files
_aio_rules = $(eval\
$2/$(ASSDIR)/%.s: $3/%.c ; $$(CC) $4 -S $$< -o $$@$(_util_newline)\
$2/$(INTDIR)/%.e: $3/%.c ; $$(CC) $4 -E $$< -o $$@$(_util_newline)\
$2/$(OBJDIR)/%.o: $3/%.c ; $$(CC) $4 -c $$< -o $$@$(_util_newline)\
)

# $1 = setting prefix
# $2 = output directory
# $3 = additional requirements
# Assigns the appropriate requirements for the $1AIO rules
_aio_requirements = $(eval\
$($1ASS): | $($1ASS_DIRECTORIES)$(_util_newline)\
$($1INT): | $($1INT_DIRECTORIES)$(_util_newline)\
$($1OBJ): | $($1OBJ_DIRECTORIES)$(_util_newline)\
$($1AIO): $2$(_util_newline)\
)
# $1 = setting prefix
# Generates the rules to create the output directories needed for $1AIO output
_aio_directory_rules = $(eval\
$($1AIO_DIRECTORIES): ; @$$(MK_DIR) '$$@'$(_util_newline)\
)

# $1 = setting prefix
# Generates the phony rules that are callbacks to $1AIO generators
_aio_phony_rules = $(eval\
$1ass: $($1ASS)$(_util_newline)\
$1int: $($1INT)$(_util_newline)\
$1obj: $($1OBJ)$(_util_newline)\
$1aio: $($1AIO)$(_util_newline)\
.PHONY: $1ass $1int $1obj $1aio$(_util_newline)\
)

# $1 = setting prefix
_aio_clean_rules = $(eval\
clean-$1ass: ; @$$(RM_FILE) $($1ASS) 2>$(NULL) || :; $$(RM_FULL_DIR) $($1ASS_DIRECTORIES) 2>$(NULL) || :;$(_util_newline)\
clean-$1int: ; @$$(RM_FILE) $($1INT) 2>$(NULL) || :; $$(RM_FULL_DIR) $($1INT_DIRECTORIES) 2>$(NULL) || :;$(_util_newline)\
clean-$1obj: ; @$$(RM_FILE) $($1OBJ) 2>$(NULL) || :; $$(RM_FULL_DIR) $($1OBJ_DIRECTORIES) 2>$(NULL) || :;$(_util_newline)\
clean-$1aio: clean-$1ass clean-$1int clean-$1obj$(_util_newline)\
.PHONY: clean-$1ass clean-$1int clean-$1obj clean-$1aio$(_util_newline)\
)

# $1 = library targets
# Generates all of
# 'again-$1: clean-$1 $1'
# 'libs: $1'
# 'clean-libs: clean-$1'
# 'again-libs: again-$1'
# and sets them all .PHONY, for each word in $1
_libs_phony_rules = $(foreach i,$1,$(eval\
again-$i: clean-$i $i$(_util_newline)\
libs: $i$(_util_newline)\
clean-libs: clean-$i$(_util_newline)\
again-libs: again-$i$(_util_newline)\
.PHONY: $i again-$i clean-$i libs clean-libs again-libs$(_util_newline)\
))
