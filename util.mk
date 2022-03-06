# TODO I probably don't need most of this

define _util_newline


endef

_util_comma:=,

_uniq = $(if $1,$(firstword $1) $(call _uniq,$(filter-out $(firstword $1),$1)))

_print_variable = $(eval\
$(info $1$(if $(filter-out undefined,$(flavor $1)),\
	$(if $(filter-out recursive,$(flavor $1)),:= $($1),= $($1)),\
	undefined))\
)

# $1 = The first wordlist
# $2 = The second wordlist
# Returns $(join $1,$2) for as many elements are in either $1 or $2, without
# going over.
_join_two_min = $(filter-out $2,$(filter-out $1,$(join $1,$2)))
# Returns all words after but not including $1 in $2
_before = $(wordlist 1,$1,$2)
# $1 = Index of word to start at
# $2 = Words
# Returns all words at and after index $1 in $2
_after = $(wordlist $1,$(words $2),$2)
# $1 = Function to call for each consecutive pairing of $2 and $3
# $2 = The first wordlist
# $3 = The second wordlist
# $4 = Extra parameter to always pass to $1
# Returns $(call $1,$i,$j) for each consecutive i,j pair in $2 and $3
_map_two = $(if $(call _join_two_min,$2,$3),$(call $1,$(firstword $2),$(firstword $3),$4)\
		   $(call _map_two,$1,$(call _after,2,$2),$(call _after,2,$3),$4))

# $1 = input files
# Get unique directories from all inputs
_get_directories = $(call _uniq,$(dir $1))

# $1 = setting name
# $2 = optional 'not' setting name (whatever '$1' ends up not being defined to), set if not empty
# $3 = setting default if not defined or valid
# $4 = one other valid setting, may be empty to disable
# Sets the variable '$1' to '$3' if it is undefined, or if '$4' is non-empty and
# '$1' does not equal '$4'
# (take out the '$(_util_newline)'s and line-continuations and it should be a little easier to read
_verify_setting = $(eval\
ifndef $1$(_util_newline)\
 $1 := $3$(_util_newline)\
 $(if $4,\
 $(if $2,$2 := $4$(_util_newline),)\
else ifneq ($$($1),$3)$(_util_newline)\
 ifneq ($$($1),$4)$(_util_newline)\
  override $1 := $3$(_util_newline)\
  $(if $2,\
  override $2 := $4$(_util_newline)\
 else$(_util_newline)\
  override $2 := $3$(_util_newline)\
  ,)\
 endif$(_util_newline)\
 ,)\
$(if $2,\
else$(_util_newline)\
 $2 := $4$(_util_newline)\
,)\
endif$(_util_newline)\
)

# $1 = setting name
# $2 = variable to check
# $3 = set '$1' if '$2' defined
# $4 = set '$1' if '$2' undefined
# '$1 = ($2 defined) ? $3 : $4'
_binary_setting = $(eval\
ifndef $1$(_util_newline)\
 ifdef $2$(_util_newline)\
  $1 := $3$(_util_newline)\
 else$(_util_newline)\
  $1 := $4$(_util_newline)\
 endif$(_util_newline)\
endif$(_util_newline)\
)
# Same as _binary_setting, but appends to '$1' rather than replaces
_binary_append = $(eval\
ifdef $2$(_util_newline)\
 $(if $3,$1 := $($1) $3,)$(_util_newline)\
else$(_util_newline)\
 $(if $4,$1 := $($1) $4,)$(_util_newline)\
endif$(_util_newline)\
)
_binary_prepend = $(eval\
ifdef $2$(_util_newline)\
 $(if $3,$1 := $3 $($1),)$(_util_newline)\
else$(_util_newline)\
 $(if $4,$1 := $4 $($1),)$(_util_newline)\
endif$(_util_newline)\
)
_binary_join_right = $(eval\
ifdef $2$(_util_newline)\
 $(if $3,$1 := $($1)$3,)$(_util_newline)\
else$(_util_newline)\
 $(if $4,$1 := $($1)$4,)$(_util_newline)\
endif$(_util_newline)\
)
_binary_join_left = $(eval\
ifdef $2$(_util_newline)\
 $(if $3,$1 := $3$($1),)$(_util_newline)\
else$(_util_newline)\
 $(if $4,$1 := $4$($1),)$(_util_newline)\
endif$(_util_newline)\
)

# $1 = setting name
# $2 = variable to check
# $3 = what to test for
# $4 = set '$1' if equal
# $5 = set '$1' is not equal
# '$1 = ($2 == $3) ? $4 : $5'
_ternary_setting = $(eval\
ifndef $1$(_util_newline)\
 ifeq ($2,$3)$(_util_newline)\
  $1 := $4$(_util_newline)\
 else$(_util_newline)\
  $1 := $5$(_util_newline)\
 endif$(_util_newline)\
endif$(_util_newline)\
)
# Same as _ternary_setting, but appends to '$1' rather than replaces
_ternary_append = $(eval\
ifeq ($2,$3)$(_util_newline)\
 $(if $4,$1 := $($1) $4,)$(_util_newline)\
else$(_util_newline)\
 $(if $5,$1 := $($1) $5,)$(_util_newline)\
endif$(_util_newline)\
)
_ternary_prepend = $(eval\
ifeq ($2,$3)$(_util_newline)\
 $(if $4,$1 := $4 $($1),)$(_util_newline)\
else$(_util_newline)\
 $(if $5,$1 := $5 $($1),)$(_util_newline)\
endif$(_util_newline)\
)
_ternary_join_right = $(eval\
ifeq ($2,$3)$(_util_newline)\
 $(if $4,$1 := $($1)$4,)$(_util_newline)\
else$(_util_newline)\
 $(if $5,$1 := $($1)$5,)$(_util_newline)\
endif$(_util_newline)\
)
_ternary_join_left = $(eval\
ifeq ($2,$3)$(_util_newline)\
 $(if $4,$1 := $4$($1),)$(_util_newline)\
else$(_util_newline)\
 $(if $5,$1 := $5$($1),)$(_util_newline)\
endif$(_util_newline)\
)
