/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrpath_h
#define brrpath_h

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstr.h>

#include <ctype.h>

#define brrpath_name_len_t brru1
#define brrpath_max_path 4095
#define brrpath_max_name 255
#define brrpath_path_len_t brru2
#if brrplat_unix
#define brrpath_case_sensitive 1
#define brrpath_separator '/'
#define brrpath_sep_str "/"
#define brrpath_issep(_c_) ((_c_) == brrpath_separator)
#define brrpath_norm(_c_) (_c_)
typedef struct stat brrpath_stat_t;
#elif brrplat_dos
#include <windows.h>
/* These max values aren't actually accurate to the windows system, but it's what brrpath will use for
 * internal storage of paths
 * */
/*  On windows, a regular path can only be 260 characters long; that's counting drive letter (C:), absolute
 * path (\), and null character, making for an actual max path name to be 256 characters if all of those are
 * present, or 259 characters for a relative path with no drive.
 *  An extended path has an extended limit of 32KiB characters, and they start with '\\?\'; fucking microsoft,
 * making shit weird.
 *  As well, according to MS docs, components of a path have a max length dependent on the type of
 * filesystem.
 *  This api will not care for such limitations until actually interacting with the system; let the OS throw
 *  a fit if a dumb path is too long, that's not my concern.
 * */
#define brrpath_case_sensitive 0
#define brrpath_separator '\\'
#define brrpath_sep_str "\\"
#define brrpath_issep(_c_) ((_c_) == '\\' || (_c_) == '/')
#define brrpath_norm(_c_) (((_c_) > 31 && (_c_) != '<' && (_c_) != '>' && (_c_) != ':' && (_c_) != '"' && (_c_) != '|' && (_c_) != '?' && (_c_) != '*') ? (_c_) : 0)
typedef WIN32_FIND_DATA brrpath_stat_t;
#endif

/* Terminology:
     Canonicalize: Clean the logical path, removing redundant separators and path elements; does not in any
       way query the filesystem and instead cleans up a path according to the rules of the OS. Further details follow
     Follow links: Remove all symlinks from a path
     Realize: Canonicalize and simplify the path; follows all symlinks, simplifies the path, etc. etc. If a
       component doesn't exist, realization stops at that component and a code is returned. Or maybe it's an
       error only optionally.
     Absolute: Return the absolute version of the path, with a relative root.
	 Realpath: Absolute(Realize())
 * */

/* Limit: 3-bit (for now) */
typedef enum brrpath_type {
	brrpath_type_invalid = 0,
	brrpath_type_file    = 1, /* Path is a file */
	brrpath_type_dir     = 2, /* Path is a directory */
	brrpath_type_device  = 3,  /* Path is some generic device */
	brrpath_type_other   = 4,  /* Path is some generic device */
	/* to be extended in future */
#if 0
	brrpath_type_blockdev, /* Path is a block device, POSIX */
	brrpath_type_reparse,  /* Path is a reparse point, Windows */
	brrpath_type_pipe,     /* Path is a pipe, POSIX */
	brrpath_type_socket,   /* Path is a socket, POSIX */
	brrpath_type_chardev,  /* Path is a character device, POSIX */
#endif
} brrpath_type_t;

static BRR_inline const char * BRRCALL
brrpath_type_name(brrpath_type_t type)
{
	switch (type) {
		case brrpath_type_file: return "file";
		case brrpath_type_dir: return "dir";
		case brrpath_type_device: return "device";
		case brrpath_type_invalid: return "invalid";
		default: return "other";
	}
}

/* Limit: 3-bit (for now) */
typedef enum brrpath_flag {
	brrpath_flag_none       = 0x00,
	brrpath_flag_symlink    = 0x01, /* Path is a symlink */
	brrpath_flag_mount      = 0x02, /* Path is a mount point */
	brrpath_flag_broken     = 0x04, /* Path is broken; context-dependent interpretation */
	brrpath_flag_reparse    = brrpath_flag_symlink | brrpath_flag_mount, /* Path is a reparse point */
	brrpath_flag_archive    = 0x08,
	brrpath_flag_compressed = 0x10,
	brrpath_flag_encrypted  = 0x20,
	/* to be extended in future */
} brrpath_flag_t;

#define brrpath_inf \
union {\
	struct {\
		brru1 ext_exists : 1; /* If there is a valid extension, this is set */ \
		brru1 abs       : 1; /* Whether the path is absolutely referenced */ \
		brru1 exists    : 1; /* Whether the path exists */ \
		brru1 type      : 3; /* The type of the path */ \
		brru1 _stat     : 1; /* Whether the path has been statted, if 0 then the other fields here are invalid. */ \
		brru1 _err      : 1; /* Currently unused */ \
		/* 1 byte */ \
		brru1 flags     : 6; /* Specific flags of the filesystem */ \
		/* 2 bits to spare */\
		brru1 size[6];     /* 48-bit size should be big enough, I would think; max of 256 TiB file */\
	};\
	struct {\
		brru4 _;\
		brru4 __;\
	};\
}
typedef brrpath_inf brrpath_inf_t;

static BRR_inline brru8 BRRCALL
brrpath_get_size(const brru1 inf[6])
{
	brru1 a[8] = {inf[0], inf[1], inf[2], inf[3], inf[4], inf[5]};
	return *(brru8*)a;
}
static BRR_inline void BRRCALL
brrpath_set_size(brru1 inf[6], brru8 new_size)
{
	brru1 *a = (brru1 *)&new_size;
	inf[0] = a[0]; inf[1] = a[1]; inf[2] = a[2]; inf[3] = a[3]; inf[4] = a[4]; inf[5] = a[5];
}

typedef struct brrpath {
/*
A 'logical dot' is a dot in the name that separates the basename from the extension; ie:
  * The dot that starts '.this_is_a_dotfile' does not count as a logical dot; it's part of the basename
  * The dot that splits 'this_is_an.extension' is a logical dot, as it separates the base name from the
    extension 'extension'.
  * Only the last dot of those consecutive in 'these_are....consecutive' is a logical dot; the ones
    immediately preceding it are part of the basename.
  * The terminating dot in 'this_ends_with_a_dot.' IS a logical separator, with the extension being an
    empty string.
A name can have multiple extensions, each separated by a logical dot; the minimal extension is the last such
  extension in a name, and the maximal extension starts 1 after the first logical dot and takes up the rest of the
  name.
A name that consists only of dots has no logical dots; it's the same logic that would be applied to something
  like '....this_file_starts_with_a_lot_of_dots'; they are part of the basename.
*/
	brrpath_path_len_t len;
	brrpath_path_len_t dir_len; /* Length of the directory component of 'full'; <= the last logical separator */
	brrpath_name_len_t base_len; /* Length of the basename component of 'full'; > the last logical separator and before the first logical dot. */
	brrpath_name_len_t ext_len;  /* Length of the maximal extension; > the first logical dot */
	union {
		brrpath_inf;
		brrpath_inf_t inf;
	};
	char *full;
} brrpath_t;

static BRR_inline const char *BRRCALL brrpath_dir_start(const brrpath_t *const p)
{ return p ? p->full : NULL; }
/* Returns the start of the basename of the path, just after the last logical separator.
 * If 'p' is NULL, NULL is returned.
 * */
static BRR_inline const char *BRRCALL brrpath_base_start(const brrpath_t *const p)
{ return p && p->full ? p->full + p->dir_len : NULL; }
/* Returns the start of the full extension of the path, just after the first logical dot.
 * If 'p' is NULL or has no extension, NULL is returned.
 * */
static BRR_inline const char *BRRCALL brrpath_ext_start(const brrpath_t *const p)
{ return p && p->full && p->ext_exists ? p->full + p->len - 1 - p->ext_len : NULL; }
/* Returns the end of the path, just before the null terminator.
 * If 'p' is NULL or of 0 length, NULL is returned.
 * */
static BRR_inline const char *BRRCALL brrpath_end(const brrpath_t *const p)
{ return p && p->full && p->len ? p->full + p->len - 1 : NULL; }
/* Returns the start of the minimal extension of the path, just after the last logical dot.
 * If 'p' is NULL or has no extension, NULL is returned.
 * */
static BRR_inline const char *BRRCALL brrpath_short_ext_start(const brrpath_t *const p)
{
	const char *const short_min = brrpath_ext_start(p);
	if (!short_min)
		return NULL;
	const char *short_start = brrpath_end(p) + 1;
	while (short_start > short_min && short_start[-1] != '.') --short_start;
	return short_start;
}

/* TODO doc */
static BRR_inline int BRRCALL
brrpath_cmp(const char *const a, const char *const b, brrsz len)
{
	if (!a)
		return b != NULL;
	else if (!b)
	  return 1;
	else if (!len)
		return 0;

	for (brrsz i = 0; i < len; ++i) {
		if (!(
			a[i] == b[i]
#if !brrpath_case_sensitive
			|| tolower(a[i]) == tolower(b[i])
#endif
			|| (brrpath_issep(a[i]) && brrpath_issep(b[i]))
		   )) {
			return (int)a[i] - (int)b[i];
		}
	}
	return 0;
}

BRRAPI void BRRCALL
brrpath_free(brrpath_t *const view);

static BRR_inline void BRRCALL
brrpath_update_lengths(brrpath_t *const path)
{
	if (!path) return;
	if (!path->full) {
		path->base_len = path->ext_len = path->len = path->dir_len = path->ext_exists = 0;
		return;
	}
	if (!path->len || brrpath_issep(path->full[path->len - 1])) {
		path->base_len = path->ext_len = 0;
		path->dir_len = path->len;
		return;
	}
	brrpath_path_len_t dir_len = 0;
	brrpath_name_len_t base_len = 0, ext_len = 0;

	/* Grabbing the name component */
	brrsz dirlen = path->len;
	while (dirlen && !brrpath_issep(path->full[dirlen - 1])) --dirlen;
	path->dir_len = dirlen;
	char *name = path->full + dirlen;
	dirlen = path->len - dirlen; /* Length of the whole last component */

	/* Measuring the base name */
	while (base_len < dirlen && name[base_len] == '.') ++base_len; // leading dots are not logical
	while (base_len < dirlen && name[base_len] != '.') ++base_len; // count non-dots until the first dot
	if (base_len == dirlen) {
		/* No logical dots */
		path->ext_exists = 0;
	} else {
		/* We have reached a logical dot */
		ext_len = dirlen - base_len - 1;
		path->ext_exists = 1;
	}
	path->base_len = base_len;
	path->ext_len = ext_len;
}

/* Given a NULL-terminated list of strings, compares each one against the extension of 'path'.
 * Each extension to match for should not include the leading dot.
 * If a match is found, the index of the match is returned and if 'matched' is not NULL, '*matched' is set to
 * point to the matched string.
 * If no match is found, -1 is returned and '*matched' is set to NULL.
 * */
BRRAPI int BRRCALL
brrpath_match_extension(const brrpath_t *const path, int short_extension, int case_sensitive, const char **const matched, ...);

/* Changes either the minimal extension or the maximal extension of 'path' to 'new'.
 * Returns 0 on success.
 * If reallocation fails, 'path' is unaffected and -1 is returned.
 * Does not change the previous statted information or the filesystem.
 * If 'len' is 0, the extension's length is set to 0, but the logical dot remains.
 * If 'new' is NULL, the extension is removed entirely.
 * */
BRRAPI int BRRCALL
brrpath_set_extension(brrpath_t *const path, int short_extension, const char *const new, brrsz len);

/* 'Same' means this file points to the same physical data on disk.
 * TODO currently only does a string comparison on the presumably canonical paths of a and b.
 * */
BRRAPI int BRRCALL
brrpath_same(const brrpath_t *const a, const brrpath_t *const b);

BRRAPI brrpath_t BRRCALL
brrpath_canonicalize(const char *const source, brrsz length);

BRRAPI int BRRCALL
brrpath_stat(brrpath_inf_t *const inf, const char *const path);

/* Returns an absolute path representing the current working directory; return value is NULL in case of error.
 * */
BRRAPI const brrpath_t *BRRCALL
brrpath_cwd(void);

/* Initializes 'path' with 'arg', canonicalizing 'arg' and stating the file on disk.
 * Returns 0 on success.
 * If an error occurs, -1 is returned and 'path' is unaffected
 * 'len' is the length of 'arg' (minus null terminator); when 'len' == BRRSZ_MAX, the function computes the
 * length of 'arg' using 'brrstr_len' with an os-dependent max length.
 * */
BRRAPI int BRRCALL
brrpath_init(brrpath_t *const path, const char *const arg, brrsz len);

typedef enum brrpath_walk_code {
	brrpath_walk_flag_consumed      = 0x01, /* The current path was consumed by the walker, do not free it before continuing to the next */
	brrpath_walk_flag_stop          = 0x02, /* Stop walking immediately, for whatever reason */
	brrpath_walk_flag_error         = 0x04, /* An error has occurred that could not be resolved */
	brrpath_walk_flag_mask          = 0x07,
	brrpath_walk_code_mask          = 0xff ^ brrpath_walk_flag_mask,
	brrpath_walk_code_continue      = 0x00, /* The current path was parsed successfully, continue as normal */
	brrpath_walk_code_skip          = 0x00, /* The current path is rejected, continue as normal */
	brrpath_walk_code_skip_siblings = (1 << 3), /* The current path and it's siblings are rejected, go back up to the parent */
	brrpath_walk_code_skip_tree     = (2 << 3), /* The current path and it's children are rejected, continue to the next sibling entry */
} brrpath_walk_code_t;
typedef brrpath_walk_code_t (*brrpath_walker_t)(brrpath_t *restrict const, brrsz depth, void *restrict const user);
typedef struct brrpath_walk_settings {
	brrsz start_depth;
	brrsz max_depth;
	int check_symlink:1; /* If true, checks for symlinks while walking (by stating again, so slower) */
	brrpath_walker_t walker;
	void *user;
	// TODO option to not cross filesystem boundaries (FTW_MOUNT in linux, don't know for windows)
} brrpath_walk_settings_t;

/* Recursively walks the directory entries of 'start', calling 'walker' on each one along the way */
BRRAPI int BRRCALL
brrpath_walk(const brrpath_t *const start, brrpath_walk_settings_t settings);

#endif /* brrpath_h */
