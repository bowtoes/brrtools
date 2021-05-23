#ifndef BRRPATH_H
#define BRRPATH_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

typedef struct brrpath_stat {
	brru8 size;
	brru1 type;
	brrb1 exists;
	brrb1 absolute;
	brru1 stat_error;
} brrpath_statT;

typedef enum brrpath_type {
	brrpath_irregular = 0x00,
	brrpath_file      = 0x01,
	brrpath_directory = 0x02,
	brrpath_link      = 0x04,
} brrpath_typeT;

typedef enum brrpath_statcode {
	brrpath_statsuccess     = 0x00,
	brrpath_statinvalidtype = 0x01,
	brrpath_staterr         = 0x02,
	brrpath_statinvalidpath = 0x04,
} brrpath_statcodeT;

/* Determines the absolute, real path of 'path' and returns a pointer to a
 * malloced string containing the absolute path.
 * If an error occurs or 'path' is invalid (NULL or length == 0), NULL is returned.
 * Return value must be freed. */
BRRAPI char *BRRCALL brrpath_realpath(const char *const path);
/* Determines if 'path' represents an absolute system path or not.
 * If 'path' is invalid (NULL or length == 0), false is returned. */
BRRAPI brrb1 BRRCALL brrpath_absolute(const char *const path);
/* Gathers various information about the given path, optionally following the
 * symbolic link 'path' if it is one.
 * If an error occurs or 'path' is invalid, the returned struct 'stat_error' will
 * contain flags indicating the type of error.
 * If statting succeeds, the returned struct 'stat_error' will be 0. */
BRRAPI brrpath_statT BRRCALL brrpath_stat(const char *const path, brrb1 followlink);
/* Gathers various information about the given file descriptor.
 * If an error occurs or 'fd' is invalid, the returned struct 'stat_error' will
 * contain flags indicating the type of error.
 * If statting succeeds, the returned struct 'stat_error' will be 0. */
BRRAPI brrpath_statT BRRCALL brrpath_fstat(int fd);
/* Returns 'true' if 'path' is an existing path, false otherwise.
 * If 'path' is an invalid string (NULL or length == 0), returns false. */
BRRAPI brrb1 BRRCALL brrpath_exists(const char *const path, brrb1 followlink);
/* Returns the type of 'path'.
 * If 'path' is an invalid string (NULL or length == 0) or invalid path,
 * returns 'brrpath_irregular'.
 * If 'path' doesn't exist, returns 'brrpath_directory'. */
BRRAPI brrpath_typeT BRRCALL brrpath_type(const char *const path, brrb1 followlink);
/* Creates the directory path 'path' if it doesn't exist and returns 'true' on
 * success, else 'false'.
 * Will optionally clean 'path' of any invalid characters before creation.
 * If 'path' exists, nothing is done and 'true' is returned.
 * If 'parents' is not 'false', all directory elements in 'path' that do not exist
 * will be created.
 * If 'path' is an invalid string (NULL or of 0 length) or has invalid characters
 * and 'clean' is false, nothing is done and 'false'
 * is returned. */
BRRAPI brrb1 BRRCALL brrpath_makepath(const char *const path, brrb1 parents, brrb1 clean);

BRRCPPEND

#endif /* BRRPATH_H */
