/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrpath_h
#define brrpath_h

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrstr.h>

#define brrpath_name_len_t brru1
#define brrpath_max_path 4095
#define brrpath_max_name 255
#define brrpath_path_len_t brru2
#if brrplat_unix
#define brrpath_separator '/'
#define brrpath_sep_str "/"
#define brrpath_issep(_c_) ((_c_) == brrpath_separator)
#define brrpath_norm(_c_) (_c_)
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
#define brrpath_separator '\\'
#define brrpath_sep_str "\\"
#define brrpath_issep(_c_) ((_c_) == '\\' || (_c_) == '/')
#define brrpath_norm(_c_) (((_c_) > 31 && (_c_) != '<' && (_c_) != '>' && (_c_) != ':' && (_c_) != '"' && (_c_) != '|' && (_c_) != '?' && (_c_) != '*') ? (_c_) : 0)
#endif

/* TERMS:
	 Canonicalize: Clean the logical path, removing redundant separators and path elements; does not in any
	   way query the filesystem and instead cleans up a path according to the rules of the OS. Further details follow
	 Follow links: Remove all symlinks from a path
     Realize: Canonicalize and simplify the path; follows all symlinks, simplifies the path, etc. etc. If a
       component doesn't exist, realization stops at that component and a code is returned. Or maybe it's an
       error only optionally.
     Absolute: Return the absolute version of the path, with a relative root.
	 Realpath: Absolute(Realize())
 * */

/* Limit: 4-bit (for now) */
typedef enum brrpath_type {
	brrpath_type_invalid  = 0x00,
	brrpath_type_file     = 0x01, /* Path is a file */
	brrpath_type_dir      = 0x02, /* Path is a directory */
	brrpath_type_reparse  = 0x04, /* Path is a reparse point, Windows */
	brrpath_type_pipe     = 0x05, /* Path is a pipe, POSIX */
	brrpath_type_socket   = 0x06, /* Path is a socket, POSIX */
	brrpath_type_chardev  = 0x07, /* Path is a character device, POSIX */
	brrpath_type_device   = 0x08, /* Path is some generic device */
} brrpath_type_t;
/* Limit: 5-bit (for now) */
typedef enum brrpath_flag {
	brrpath_flag_none       = 0x00,
	brrpath_flag_broken     = 0x01, /* Path is broken; context-dependent interpretation */
	brrpath_flag_mount      = 0x02, /* Path is a reparse/mount point */
	brrpath_flag_sparse     = 0x04, /* Path is sparse (Windows, there isn't a POSIX way to detect this) */
	brrpath_flag_compressed = 0x08, /* Path is compressed (Windows) */
	brrpath_flag_encrypted  = 0x10, /* Path is encrypted (Windows) */
	brrpath_type_blockdev = 0x03, /* Path is a block device, POSIX */
} brrpath_flag_t;
#define brrpath_inf \
union {\
	struct {\
		brru4 ext_valid:1; /* If there is a valid extension, this is set */ \
		brru4 abs:1;       /* Whether the path is absolutely referenced */ \
		brru4 extists:1;   /* Whether the path exists */ \
		brru4 symlink:1;   /* Whether the path is a symlink */ \
		brru4 type:4;      /* The type of the path; if a symlink, the type of the target */ \
		brru4 flags:5;     /* Specific flags of the filesystem */ \
	};\
	brru4 _;\
}
typedef brrpath_inf brrpath_inf_t;

BRRAPI int BRRCALL
brrpath_cmp(const char *const a, const char *const b, brrsz n);

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
	A name can have multiple extensions, each separated by a logical dot; the minimal extension is the last
	  such extension in a name, and the maximal extension starts at the first logical dot and takes up the
	  rest of the name.
	A name that consists only of dots has no logical dots; it's the same logic that would be applied to
	something like '....this_file_starts_with_a_lot_of_dots'; they are part of the basename.
	*/
	char *full;
	brrpath_path_len_t len;
	brrpath_name_len_t base_len; /* Length of the basename component of 'full', excluding any extension */
	brrpath_name_len_t ext_len;  /* Length of the maximal extension; everything after the first logical dot */
	union {
		brrpath_inf;
		brrpath_inf_t inf;
	};
	brrsz size; /* Size of the source in bytes, or if a symlink and has been statted, size of the target in bytes. */
} brrpath_t;

BRRAPI void BRRCALL
brrpath_free(brrpath_t *const view);

BRRAPI brrpath_t BRRCALL
brrpath_canonicalize(const char *const source, brrsz length);

BRRAPI int BRRCALL
brrpath_stat(brrpath_t *const path);

BRRAPI brrpath_path_len_t BRRCALL
brrpath_dirlen(const brrpath_t *const path);

/* Returns an absolute path representing the current working directory; return value is NULL in case of error. */
BRRAPI brrpath_t BRRCALL
brrpath_cwd(void);

typedef struct brrpath_walked {
	brrpath_t path;
	brrsz depth;
} brrpath_walked_t;

typedef struct brrpath_walk_settings {
	brrsz start_depth;
	brrsz max_depth;
	char *start;
} brrpath_walk_settings_t;

BRRAPI int BRRCALL
brrpath_walk(brrpath_t **restrict const paths, brrsz *n_paths, const char *restrict const start, brrsz max_depth);

#endif /* brrpath_h */
