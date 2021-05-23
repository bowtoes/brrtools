#ifndef BRRBUFFER_H
#define BRRBUFFER_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

typedef struct brrbuffer {
	void *opaque;
	brrsz size;
	brrsz position;
} brrbufferT;

/* Allocates a new buffer of 'size' bytes, set to zero.
 * If an error occures, the returned buffer's 'opaque' member will be NULL. */
BRRAPI brrbufferT BRRCALL brrbuffer_new(brrsz size);
/* Duplicates and returns a copy of 'other'.
 * If an error occures, the returned buffer's 'opaque' member will be NULL. */
BRRAPI brrbufferT BRRCALL brrbuffer_copy(const brrbufferT *const other);
/* Frees all associated memory with buffer. */
BRRAPI void BRRCALL brrbuffer_del(brrbufferT *const buffer);
/* Resizes 'buffer' to contain 'new_size' bytes and returns 'true' on success.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or an error occurs while resizing,
 * 'false' is returned and 'buffer' is left unaffected. */
BRRAPI brrb1 BRRCALL brrbuffer_resize(brrbufferT *const buffer, brrsz new_size);

/* Writes 'data_size' bytes of 'data' into the buffer at the current position
 * and returns the new position.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and the current buffer position
 * is returned.
 * If an error occurs, 'buffer' is left unaffected and 0 is returned. */
BRRAPI brrsz BRRCALL brrbuffer_write(brrbufferT *const buffer, const void *const data, brrsz data_size);
/* Reads 'read_size' bytes from the buffer at the current position into 'destination'
 * and returns number of bytes read.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and 0 is returned.
 * If reading 'read_size' bytes would bring the buffer position past its size,
 * then at most 'buffer->size - buffer->position' bytes will be read. */
BRRAPI brrsz BRRCALL brrbuffer_read(brrbufferT *const buffer, void *const destination, brrsz read_size);

BRRAPI brrbufferT BRRCALL brrbuffer_fromfile(int fd);
BRRAPI brrb1 BRRCALL brrbuffer_tofile(const brrbufferT *const buffer, int fd);

BRRAPI void *BRRCALL brrbuffer_data(const brrbufferT *const buffer);

BRRCPPEND

#endif /* BRRBUFFER_H */
