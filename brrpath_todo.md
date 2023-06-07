The current form of 'brrpath' is simply far too complicated & unpleasant to use.
It needs split up into separate, distinct components that don't need to interact too much.
Primarily, path statting (for size, type, existence, etc.) and path string manipulation (changing extensions,
splitting path components, inserting new ones, etc.) must be separate.

Scratch this, I've changed my mind about `brrpath` for path manipulation; there are other libraries out there
(`cwalk` for instance) for that task.
The only thing I'll make for now is `brrpath_walk`; I may EVENTUALLY add SOME functionality to it, but
there's no real guarantee.

For statting, I think `brrstat` would work:

```c
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
} brrstat_ltype_t;

typedef struct brrstat {
    brrstat_ltype_t ltype;
    brratat_ftype_t ftype;
    uint64_t size; /* Size on disk */
    /* Maybe some other things, don't know yet */
} brrstat_t;

```

This would be the bear minimum from my understanding of it.

`brrpath` would act on strings and return strings, simple as.
```c

/* Returns non-zero if a path is relative */
int
brrpath_relative(const char *const path);

/* Returns non-zero if a path is absolute */
int
brrpath_absolute(const char *const path);

/*
 * Returns 1 if the path represents '/'
 * Returns 2 if the path represents '.' ('.', './')
 * Returns 3 if the path represents '..' ('..', '../')
 * Returns 0 otherwise.
 */
int
brrpath_special(const char *const path);

size_t
brrpath_cwd(char **const dst);

size_t
brrpath_cwd(char *const dst, size_t maxlen);

/* There is a complication with windows when it comes to relative and absolute paths, that will need to be
explored */

/* Uses system functions to do realpath; 'realpath' on POSIX, some other thing on windows */
size_t
brrpath_realpath(const char *restrict const path, char *restrict *restrict const dst);

size_t
brrpath_realpath2(const char *const path, char *const dst, size_t maxlen);

/* Each of these that returns size_t, returns the length of the destination, or SIZE_MAX on error.
 * For the preallocated ones, SIZE_MAX means that the operation is invalid (calling '_extension' on a path
 * without an extension, for instance); this just means that 'dst' wasn't modified.

 * They also might not return size_t/SIZE_MAX, as on most systems (all?), PATH_MAX isn't greater than what can be
   represented with a uint16.
 */

/* Removes duplicate path separators and canonicalizes './' & '../' elements; optionally removes invalid
 * characters (invalid for the OS, not the underlying filesystem).
 * For example:
 *  './this_file'           -> 'this_file'
 *  '././this_file'         -> 'this_file'
 *  '../this_file'          -> '../this_file'
 *  './../this_file'        -> '../this_file'
 *  '.././this_file'        -> '../this_file'
 *  'this_directory/'       -> 'this_directory'
 *  'this_dir/../this_file' -> 'this_file'
 *  'this//dir///to_a.file..with.....some_dumb.extension' -> 'this/dir/to_a.file..with.....some_dumb.extension'
 *  '/' -> '/'
 *  '///' -> '/'
 * Notice in the last example that the duplicate '.'s are not removed; they are part of the filename, and
 * aren't ancillary to path resolution.
 * A separate module will have a rename functionality to remove these duplicate dots, and only these ones in
 * the file name, not any that may appear in a directory before hand.
 */
size_t
brrpath_canonicalize(const char *restrict const path, int invalid, char *restrict *restrict const dst);

size_t
brrpath_canonicalize2(const char *const path, int invalid, char *const dst, size_t maxlen);

/* Returns the parent of 'path', without the trailing separator.
 * Returns everything before the last logical separator.
 */
size_t
brrpath_dir(const char *restrict const path, char *restrict *restrict const dst);

/* For a preallocated destination string */
size_t
brrpath_dir2(const char *const path, char *const dst, size_t maxlen);

/* Returns the computed parent path for 'path'
 * Examples of what should happen:
 * 'some_file' -> '.'
 * 'some_path/to/a_file' -> 'some_path/to'
 * '/absolute_file' -> '/'
 * '/' -> ''
 */
size_t
brrpath_parent(const char *restrict const path, char *restrict *restrict const dst);

size_t
brrpath_parent2(const char *const path, char *const dst, size_t maxlen);

/* Returns everything after the last logical separator and before the first dot.

 * If the filename starts with one or more dots, then those dots become part of the basename, up until the
   first non-consecutive dot

 * If the path ends in a logical operator (root dir, or, './' or likewise), the empty string is returned.

 * Examples:
 * 'filename' -> 'filename'
 * 'path/filename' -> 'filename'
 * 'path/filename.extension' -> 'filename'
 * 'path/.name_as_extension' -> '.name_as_extension'
 * 'path/......name...extension' -> '......name'
 * 'path/.........' -> '.........'
 * 'path/.name_as_extension.extension2' -> '.name_as_extension'
 * 'path/' -> ''
 * '/' -> ''
 * './' -> ''
 * '.' -> '.'
 * '..' -> '..'
 */
size_t
brrpath_base(const char *restrict const path, char *restrict *restrict const dst);

size_t
brrpath_base2(const char *const path, char *const dst, size_t maxlen);

/* Returns the complete extension of 'path', from the first dot to the end of the string.
 * Returns everything including and after the first dot.
 * Examples:
 * 'filename' -> ''
 * 'path/filename' -> ''
 * 'path/filename.extension' -> '.extension'
 * 'path/.name_as_extension' -> ''
 * 'path/......name...extension' -> '...extension'
 * 'path/.........' -> ''
 * 'path/.name_as_extension.extension2' -> '.extension2'
 * 'path/' -> ''
 * '/' -> ''
 * './' -> ''
 * '.' -> '' # Special, empty returned
 * '..' -> '' # Special, empty returned.
 */
size_t
brrpath_extension(const char *restrict const path, char *restrict *restrict const dst);

size_t
brrpath_extension2(const char *const path, char *const dst, size_t maxlen);

size_t
brrpath_mod_extension(const char *restrict const path, const char *restrict new, char *restrict *restrict const dst);

size_t
brrpath_mod_extension2(const char *const path, const char *new, char **const dst, size_t maxlen);

/* I'm still not sure on how brrpath_split should be done to be honest */
typedef struct brrpath_split_len {
    size_t dirlen;
    size_t baselen;
    size_t extlen;
    int error;
} brrpath_split_len_t;

brrpath_split_len_t
brrpath_split(const char *restrict const path, char *restrict *restrict const dirname, char *restrict *restrict basename, char *restrict *restrict extension);

brrpath_split_len_t
brrpath_split2(const char *restrict const path, char *const dirname, char *basename, char *extension, size_t dirlen, size_t baselen, size_t extlen);

int
brrpath_walk(...);

```
