#ifndef brrpath_h
#define brrpath_h

/*
   'brrpath' will consist of three major components:
     brrstr_t brrpath_canonical(brrstr_t p):
        Canonicalizes 'p', reducing it down to it's shortest representation. Removes '..' and '.' that can be
        removed, and condenses down repeated '/'
     brrpath_info_t brrpath_info(brrstr_t p):
        Stats the type, size, and a couple other stats of 'p', as well as storing in the return views onto 'p'
        of the extension, base name, and directory components. (maybe an 'i_build_stat(brrstr_t p, type,
        size, ...)' for the walk)
     int brrpath_walk(brrstr_t root, parser, min_depth, max_depth):
        Walks 'root', parsing each 'brrpath_info_t' along the way with 'parser'; for 'min_depth' to work on
        unix, the easiest way is to require GNUC nftw, but the most portable way, I think, will be for me to
        walk it all myself; what a bummer.
    Maybe some more, like 'brrstr_t brrpath_join(brrstr_t a, brrstr_t b)', which will canonicalize the
    output.
    Maybe 'brrstr_t brrpath_vjoin(brrstr_t root, ...)' for joining many in a single call.
    And possibly, 'int brrpath_maketree(brrstr_t tree)'; file handling is part of C-standard (fopen, fclose,
    ...), so I need only worry about directories.
    'int brrpath_move(brrstr_t start, brrstr_t end)' will move 'start' into 'end' if 'end' is existing dir,
    else rename 'start' to 'end' if 'end' doesn't already exist, error otherwise.
    'int brrpath_remove(brrstr_t path)' removes a single file if file, else empty dir if dir, error otherwise
    'int brrpath_removetree(brrstr_t tree, int start_depth)' removes recursively from 'start_depth' downwards
 * */

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

#if brrplat_unix
typedef int brrpath_fd_t;
#elif brrplat_dos
#include <windows.h>
typedef HANDLE brrpath_fd_t;
#endif

typedef enum brrpath_type {
	brrpath_type_unknown    = 0x00,
	brrpath_type_file       = 0x01, /* Path is a file */
	brrpath_type_dir        = 0x02, /* Path is a directory */
	brrpath_type_symlink    = 0x03, /* Path is a symlink */
	brrpath_type_blockdev   = 0x04, /* Path is a block device, POSIX */
	brrpath_type_reparse    = 0x05, /* Path is a reparse point, Windows */
	brrpath_type_pipe       = 0x06, /* Path is a pipe, POSIX */
	brrpath_type_socket     = 0x07, /* Path is a socket, POSIX */
	brrpath_type_chardev    = 0x08, /* Path is a character device, POSIX */
	brrpath_type_device     = 0x0c, /* Path is some generic device */
} brrpath_type_t;
typedef enum brrpath_flags {
	brrpath_flag_none       = 0x00,
	brrpath_flag_sparse     = 0x08, /* Path is a 'sparse' file; Windows-only for now, there isn't a POSIX way to detect this */
	brrpath_flag_compressed = 0x10, /* Path is compressed, Windows-only */
	brrpath_flag_encrypted  = 0x20, /* Path is encrypted, Windows-only */
	brrpath_flag_mount      = 0x40, /* Path is a mount-point */
	brrpath_flag_broken     = 0x80, /* Broken/invalid symlink/reparse */
} brrpath_flags_t;
typedef enum brrpath_perms {
	brrpath_perm_none   = 0x000,
	brrpath_perm_ur = 0x001, /* User read permissions */
	brrpath_perm_gr = 0x002, /* Group read permissions */
	brrpath_perm_or = 0x004, /* Others read permissions */
	brrpath_perm_read = brrpath_perm_ur | brrpath_perm_gr | brrpath_perm_or, /* General read permissions */
	brrpath_perm_uw = 0x010, /* User write permissions */
	brrpath_perm_gw = 0x020, /* Group write permissions */
	brrpath_perm_ow = 0x040, /* Others write permissions */
	brrpath_perm_write = brrpath_perm_uw | brrpath_perm_gw | brrpath_perm_ow, /* General write permssions */
	brrpath_perm_ux = 0x100, /* User execute permissions */
	brrpath_perm_gx = 0x200, /* Group execute permissions */
	brrpath_perm_ox = 0x400, /* Others execute permissions */
	brrpath_perm_exec = brrpath_perm_ux | brrpath_perm_gx | brrpath_perm_ox, /* General execute permissions */
	/* TODO more, eventually */
	/* Also TODO, permissions are very hard; for now, no permission information is actually gathered */
} brrpath_perms_t;
typedef struct brrpath_info {
	int exists; /* Whether the path exists */
	brrsz size; /* The size of the path in bytes; if the path is a valid symlink, the size of the target. */
	brrpath_type_t type; /* The type of path. */
	brrpath_flags_t flags; /* Relevant details of the path; if the path is a valid symlink, relevant details of the target .*/
	brrpath_perms_t perms; /* The permissions on the path; if the path is a valid symlink, permissions of the target. */
} brrpath_info_t;

/* Stats the file referenced by 'path' and returns the a 'brrpath_info_t' object with information regarding
 * the file.
 * If an error occurs, the returned info will have its 'size' field set to BRRSZ_MAX, and brrapi_err will be
 * set. */
BRRAPI brrpath_info_t BRRCALL
brrpath_stat(const char *const path);

/* Stats the file referred to by 'fd' and returns the a 'brrpath_info_t' object with information regarding
 * the file.
 * If an error occurs, the returned info will have its 'size' field set to BRRSZ_MAX, and brrapi_err will be
 * set.
 * NOTE: By the nature of file-descriptors, this cannot function distinguish symlinks.
 * On POSIX-like systems, 'fd' is the file descriptor ('fileno') of the file.
 * On Windows, 'fd' is the HANDLE for the file. */
BRRAPI brrpath_info_t BRRCALL
brrpath_fstat(brrpath_fd_t fd);

#endif /* brrpath_h */
