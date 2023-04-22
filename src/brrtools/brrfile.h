/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrfile_h
#define brrfile_h

#include <brrtools/brrpath.h>

/* This interface isn't very complicated or densely functional, I'm thinking about doing some kind of
 * buffering thing at some point? Problem is I don't really see a massive use case for that in any of the
 * stuff I do.
 * Maybe a generic de/serializer system? Give it a pointer to data, a de/serialization function, and an
 * output buffer? The tough part's the output buffer though.
 * Idk, todo eventually
 * */

/* Reads all of 'path' into the buffer pointed to by 'buffer'.
 * Returns the number of bytes read on success.
 * If an error occurs or 'path' is not a file, BRRSZ_MAX is returned and 'buffer' is unaffected.
 * If 'inf' is NULL, 'path' will be statted to determine type and size.
 * */
brrsz
brrfile_read(const char *const path, const brrpath_inf_t *const inf, void **const buffer);

/* Returns number of bytes written on success (will usually be input 'size').
 * Returns BRRSZ_MAX on error.
 * The only real error conditiions are a non-existing path/bad permissions/destination not a file.
 * 'inf' is optional; if not NULL, type-checking is done and incorrect destination path types are reported
 * and not tried.
 * 'path' is not automatically statted after writing; i.e. 'inf' is not updated automatically.
 * */
brrsz
brrfile_write(const char *const path, const brrpath_inf_t *const inf, const void *const buffer, brrsz size);

#endif /* brrfile_h */
