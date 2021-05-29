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

#include <stdarg.h>

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
	brrsz position;
} brrbufferT;

/* Allocates a new buffer of 'size' bytes, set to zero.
 * If an error occurs, the returned buffer's 'opaque' member will be NULL.
 * */
BRRAPI brrbufferT BRRCALL brrbuffer_new(brrsz size);
/* Duplicates and returns a copy of 'other'.
 * If an error occurs, the returned buffer's 'opaque' member will be NULL.
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

/* Returns a pointer to the start of the buffer data.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), NULL is returned.
 * */
BRRAPI const void *BRRCALL brrbuffer_data(const brrbufferT *const buffer);
/* Returns the registered size of a buffe.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_size(const brrbufferT *const buffer);
/* Returns the current actual capacity of a buffer.
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
 * If 'data' is NULL or 'data_size' is 0, nothing is done and 0 is returned.
 * If an error occurs, 'buffer' is left unaffected and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_write(brrbufferT *const buffer, const void *const data, brrsz data_size);
/* Writes all of 'source' into 'destination' at the current buffer position and returns
 * the number of bytes written.
 * If 'destination' or 'source' is invalid (NULL or 'opaque' is NULL), nothing is
 *  done and 0 is returned.
 * If an error occurs, 'buffer' is left unaffected and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_write_to(brrbufferT *const destination, const brrbufferT *const source);
/* Reads 'read_size' bytes from the buffer at the current position into 'destination'
 * and returns number of bytes read.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'data' is NULL or 'read_size' is 0, nothing is done and 0 is returned.
 * If reading 'read_size' bytes would bring the buffer position past its size,
 * then at most 'buffer->size - buffer->position' bytes will be read.
 * */
BRRAPI brrsz BRRCALL brrbuffer_read(brrbufferT *const buffer, void *const destination, brrsz read_size);
/* Reads 'read_size' bytes from 'source' at the current buffer position and writes into
 * 'destination' at the current buffer position and returns the number of bytes transferred.
 * If 'source' or 'destination' is invalid (NULL or 'opaque' is NULL) or 'read_size' is 0, nothing
 * is done and 0 is returned.
 * If transferring 'read_size' bytes would bring 'source' buffer position past its size,
 * then at most 'source->size - source->position' bytes will be transferred.
 * */
BRRAPI brrsz BRRCALL brrbuffer_read_to(brrbufferT *const source, brrbufferT *const destination, brrsz read_size);
/* Reads a segment of bytes from 'buffer' in the range [start, end) into 'destination',
 * without updating buffer position.
 * Returns the number of bytes read.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'end' is before 'start', bytes are read in reverse.
 * 'end' and 'start' are clamped to 'buffer->size'.
 * If 'end == start', nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_segment(brrbufferT *const buffer, void *const destination, brrsz start, brrsz end);
/* Reads a segment of bytes from 'source' in the range [start, end) into 'destination',
 * without updating buffer positions.
 * Returns the number of bytes read.
 * If 'source' or 'destination' is invalid (NULL or 'opaque' is NULL), nothing is done and 0 is returned.
 * If 'end' is before 'start', bytes are read in reverse.
 * 'end' and 'start' are clamped to 'source->size'.
 * If 'end == start', nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_segment_to(brrbufferT *const source, brrbufferT *const destination, brrsz start, brrsz end);
/* Searches 'buffer' for the next occurrence of 'key', of length 'key_length', and puts the buffer position
 * at that occurrence.
 * If 'key' is found within 'buffer', true is returned, otherwise buffer is left unchanged and false is returned.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'key' is invalid (NULL or length == 0), nothing is done
 * and false is returned.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_find_next(brrbufferT *const buffer, const void *const key, brrsz key_length);
/* Searches 'buffer' for the previous occurrence of 'key', of length 'key_length', and puts the buffer position
 * at that occurrence.
 * If 'key' is found within 'buffer', true is returned, otherwise buffer is left unchanged and false is returned.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'key' is invalid (NULL or length == 0), nothing is done
 * and false is returned.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_find_previous(brrbufferT *const buffer, const void *const key, brrsz key_length);

/* Calls 'vsnprintf', printing to the current buffer position and updating the position accordingly.
 * Prints at most 'max_size' bytes into 'buffer'.
 * Returns the result of 'vsnprintf'.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'max_size' is 0 or 'fmt' is NULL, nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_vnprintf(brrbufferT *const buffer, brrsz max_size, const char *const fmt, va_list lptr);
/* Calls 'snprintf', printing to the current buffer position and updating the position accordingly.
 * Prints at most 'max_size' bytes into 'buffer'.
 * Returns the result of 'snprintf'.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'max_size' is 0 or 'fmt' is NULL, nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_nprintf(brrbufferT *const buffer, brrsz max_size, const char *const fmt, ...);
/* Calls 'vsprintf', printing to the current buffer position and updating the position accordingly.
 * Will only print to the end of 'buffer'.
 * Returns the result of 'vsprintf'.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'max_size' is 0 or 'fmt' is NULL, nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_vprintf(brrbufferT *const buffer, const char *const fmt, va_list lptr);
/* Calls 'sprintf', printing to the current buffer position and updating the position accordingly.
 * Will only print to the end of 'buffer'.
 * Returns the result of 'sprintf'.
 * If 'buffer' is invalid (NULL or 'opaque' is NULL) or 'max_size' is 0 or 'fmt' is NULL, nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrbuffer_printf(brrbufferT *const buffer, const char *const fmt, ...);

/* Read all bytes from the regular file at 'file_descriptor' and return a buffer containing them.
 * If 'file_descriptor' is an invalid file descriptor or does not reference a regular file, nothing is done and an
 * empty buffer is returned.
 * If an error occurs, nothing is done and an empty file descriptor is returned.
 * */
BRRAPI brrbufferT BRRCALL brrbuffer_from_file(int file_descriptor);
/* Write s all bytes in 'buffer' to the file referenced by 'file_descriptor'.
 * 'true' is returned on success; if an error occurs or 'buffer' is invalid (is NULL or 'opaque' is NULL)
 * or 'file_descriptor' is invalid, nothing is done and false is returned.
 * */
BRRAPI brrb1 BRRCALL brrbuffer_to_file(const brrbufferT *const buffer, int file_descriptor);

BRRCPPEND

#endif /* BRRBUFFER_H */
