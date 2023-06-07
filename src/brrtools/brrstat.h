#ifndef brrtools_brrstat_h
#define brrtools_brrstat_h

#include <stdint.h>

typedef enum brrstat_ftype {
    brrstat_ftype_unknown   = 0, /* Invalid file type (pipe, socket, something else) */
    brrstat_ftype_file      = 1, /* File is a file (wow) */
    brrstat_ftype_directory = 2, /* File is a directory */
} brrstat_ftype_t;

typedef enum brrstat_ltype {
    brrstat_ltype_none     = 0, /* Link type is none (path doesn't exist). */
    brrstat_ltype_hardlink = 1, /* Hardlink, stat information is of the file on disk */
    brrstat_ltype_softlink = 2, /* Symlink, stat information may be about the file on disk or the link itself */
    brrstat_ltype_broken   = 3, /* Broken link, stat information is about the link itself */
	brrstat_ltype_reparse  = 4, /* Windows specific, for things like mountpoints */
} brrstat_ltype_t;

#ifndef BRRBUFFER_MINIMUM_SIZE
#define BRRBUFFER_MINIMUM_SIZE 256
#endif
#ifndef BRRBUFFER_MAXIMUM_SIZE
#define BRRBUFFER_MAXIMUM_SIZE UINT16_MAX
#endif
#ifndef BRRFILE_MAXIMUM_SIZE
#define BRRFILE_MAXIMUM_SIZE ((INT64_MAX)>>1)
#endif

typedef int      brrbsize_t; /* Size of a file buffer, in bytes (limited to 64 KiB) */
typedef int      brrbpos_t;  /* Byte-position within a buffer */
typedef uint64_t brrfsize_t; /* Size of a file, in bytes (limited to INT64_MAX/2 bytes, 4 EiB (should be enough, right?))*/
typedef int64_t  brrfpos_t;  /* Byte-position within a file */

typedef struct brrstat {
    brrstat_ltype_t ltype;
    brrstat_ftype_t ftype;
    brrfsize_t size; /* Size on disk */
    /* Maybe some other things, don't know yet */
} brrstat_t;

/* Stats 'path' and stores the relevant information in 'stat'.
 * Returns 0 on success, -1 on error.
 * */
int
brrstat(brrstat_t *restrict const Stat, const char *restrict const path);

#endif /* brrtools_brrstat_h */
