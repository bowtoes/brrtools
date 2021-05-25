/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef BRRBUFFER_H
#define BRRBUFFER_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* The increment that is used when allocating buffers.
 * Best practice is to use powers of 2 greater than 1.
 * Default is 2048.
 * */
extern brrsz brrbuffer_capacity_increment;

typedef struct brrbuffer {
	void *opaque;
	brrsz size;
	brrsz position;
} brrbufferT;

/* Allocates a new buffer of 'size' bytes, set to zero.
 * If an error occures, the returned buffer's 'opaque' member will be NULL.
 * */
BRRAPI brrbufferT BRRCALL brrbuffer_new(brrsz size);
/* Duplicates and returns a copy of 'other'.
 * If an error occures, the returned buffer's 'opaque' member will be NULL.
 * */
BRRAPI brrbufferT BRRCALL brrbuffer_copy(const brrbufferT *const other);
/* Frees all associated memory with buffer.
 * */
BRRAPI void BRRCALL brrbuffer_delete(brrbufferT *const buffer);
/* Resizes 'buffer' to contain 'new_size' bytes and returns 'true' on success.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or an error occurs while resizing,
 * 'false' is returned and 'buffer' is left unaffected.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_resize(brrbufferT *const buffer, brrsz new_size);

/* Returns a pointer to the start of the buffer data stored in a buffer's 'opaque' member.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), NULL is returned.
 * */
BRRAPI const void *BRRCALL brrbuffer_data(const brrbufferT *const buffer);
/* Returns the current actual capacity of a buffer stored in its 'opaque' member.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_capacity(const brrbufferT *const buffer);
/* Returns a pointer to the data of 'buffer' at the current buffer position.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), NULL is returned.
 * */
BRRAPI const void *BRRCALL brrbuffer_stream(const brrbufferT *const buffer);

/* Writes 'data_size' bytes of 'data' into the buffer at the current position
 * and returns the number of bytes written.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and the current buffer position
 * is returned.
 * If an error occurs, 'buffer' is left unaffected and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_write(brrbufferT *const buffer, const void *const data, brrsz data_size);
/* Reads 'read_size' bytes from the buffer at the current position into 'destination'
 * and returns number of bytes read.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and 0 is returned.
 * If reading 'read_size' bytes would bring the buffer position past its size,
 * then at most 'buffer->size - buffer->position' bytes will be read.
 * */
BRRAPI brrsz BRRCALL brrbuffer_read(brrbufferT *const buffer, void *const destination, brrsz read_size);
/* Searches 'buffer' for the next occurence of 'key', of length 'key_length', and puts the buffer position
 * at that occurence.
 * If 'key' is found within 'buffer', true is returned, otherwise buffer is left unchanged and false is returned.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'key' is invalid (NULL or length == 0), nothing is done
 * and false is returned.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_find_next(brrbufferT *const buffer, const void *const key, brrsz key_length);
/* Searches 'buffer' for the previous occurence of 'key', of length 'key_length', and puts the buffer position
 * at that occurence.
 * If 'key' is found within 'buffer', true is returned, otherwise buffer is left unchanged and false is returned.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'key' is invalid (NULL or length == 0), nothing is done
 * and false is returned.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_find_previous(brrbufferT *const buffer, const void *const key, brrsz key_length);

/*
BRRAPI brrbufferT BRRCALL brrbuffer_fromfile(int fd);
BRRAPI brrb1 BRRCALL brrbuffer_tofile(const brrbufferT *const buffer, int fd);
*/

BRRCPPEND

#endif /* BRRBUFFER_H */
