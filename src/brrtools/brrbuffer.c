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

#include "brrtools/brrbuffer.h"

#include <stdio.h>
#include <string.h>

#include "brrtools/noinstall/utils.h"
#include "brrtools/brrapi.h"
#include "brrtools/brrplatform.h"
#include "brrtools/brrlib.h"
#include "brrtools/brrlog.h"
#include "brrtools/brrmem.h"
#include "brrtools/brrpath.h"

#if defined(BRRPLATFORMTYPE_WINDOWS)
# include <io.h>
#else
# include <unistd.h>
#endif

#define BRRBUFFER_CAP_INC_DEFAULT 2048
brrsz brrbuffer_capacity_increment = BRRBUFFER_CAP_INC_DEFAULT;

typedef struct {
	void *data;
	brrsz size;
	brrsz capacity;
} buffint;

static brrsz BRRCALL buffcap(brrsz size) {
	brrsz mul = 0;
	if (!brrbuffer_capacity_increment)
		brrbuffer_capacity_increment = BRRBUFFER_CAP_INC_DEFAULT;
	mul = size / brrbuffer_capacity_increment;
	if (brrbuffer_capacity_increment * mul == size) {
		return size;
	} else {
		return brrbuffer_capacity_increment * (mul + 1);
	}
}

brrbufferT BRRCALL
brrbuffer_new(brrsz size)
{
	brrbufferT buff = {0};
	buffint *INT = NULL;
	if (!brrlib_alloc((void **)&INT, sizeof(buffint), 1))
		return buff;
	INT->capacity = buffcap(size);
	if (!brrlib_alloc(&INT->data, INT->capacity, 1)) {
		brrlib_alloc((void **)&INT, 0, 0);
	} else {
		INT->size = size;
	}
	buff.opaque = INT;
	return buff;
}

brrbufferT BRRCALL
brrbuffer_copy(const brrbufferT *const other)
{
	brrbufferT buff = {0};
	if (other) {
		const buffint *const oth = (buffint *)other->opaque;
		buff.position = other->position;
		if (oth) {
			buffint *INT = NULL;
			if (brrlib_alloc((void **)&INT, sizeof(buffint), 1)) {
				INT->capacity = oth->capacity;
				INT->size = oth->size;
				INT->data = brrmem_duplicate(oth->data, oth->capacity);
			}
			buff.opaque = INT;
		}
	}
	return buff;
}

void BRRCALL
brrbuffer_delete(brrbufferT *const buffer)
{
	if (buffer) {
		buffint *INT = (buffint *)buffer->opaque;
		if (INT) {
			brrlib_alloc((void **)&INT->data, 0, 0);
		}
		brrlib_alloc((void **)&INT, 0, 0);
		buffer->opaque = NULL;
		buffer->position = 0;
	}
}

brrb1 BRRCALL
brrbuffer_resize(brrbufferT *const buffer, brrsz new_size)
{
	brrb1 r = false;
	if (buffer && buffer->opaque) {
		brrsz cap = buffcap(new_size);
		buffint *INT = (buffint *)buffer->opaque;
		if (cap != INT->capacity) {
			if ((r = brrlib_alloc(&INT->data, INT->capacity, 0))) {
				INT->size = new_size;
				INT->capacity = cap;
			}
		} else {
			INT->size = new_size;
			r = true;
		}
	}
	return r;
}

const void *BRRCALL
brrbuffer_data(const brrbufferT *const buffer)
{
	return (buffer && buffer->opaque)?((buffint *)buffer->opaque)->data:NULL;
}

brrsz BRRCALL
brrbuffer_size(const brrbufferT *const buffer)
{
	return (buffer && buffer->opaque)?((buffint *)buffer->opaque)->size:0;
}

brrsz BRRCALL
brrbuffer_capacity(const brrbufferT *const buffer)
{
	return (buffer && buffer->opaque)?((buffint *)buffer->opaque)->capacity:0;
}

const void *BRRCALL
brrbuffer_stream(const brrbufferT *const buffer)
{
	return (buffer && buffer->opaque)?(brrby *)(((buffint *)buffer->opaque)->data) + buffer->position:NULL;
}

static brrsz BRRCALL buffint_write(brrbufferT *const buffer, const void *const data, brrsz data_size) {
	buffint *INT = (buffint *)buffer->opaque;
	brrsz np = buffer->position + data_size;
	if (np > INT->size) {
		INT->size = np;
		if (np > INT->capacity) {
			brrsz cap = buffcap(np);
			// Do nothing on failure.
			if (!brrlib_alloc(&INT->data, cap, 0))
				return 0;
			INT->capacity = cap;
		}
	}
	memmove((brrby *)INT->data + buffer->position, data, data_size);
	buffer->position = np;
	return data_size;
}

brrsz BRRCALL
brrbuffer_write(brrbufferT *const buffer, const void *const data, brrsz data_size)
{
	if (!buffer || !buffer->opaque)
		return 0;
	if (!data || !data_size)
		return 0;
	return buffint_write(buffer, data, data_size);
}

brrsz BRRCALL
brrbuffer_write_to(brrbufferT *const destination, const brrbufferT *const source)
{
	buffint *INT;
	if (!destination || !destination->opaque || !source || !source->opaque)
		return 0;
	INT = (buffint *)source->opaque;
	return buffint_write(destination, INT->data, INT->size);
}

static brrsz BRRCALL buffint_read(brrbufferT *const buffer, void *const destination, brrsz read_size) {
	buffint *INT = (buffint *)buffer->opaque;
	brrsz np = buffer->position + read_size;
	if (np > INT->size) {
		read_size = INT->size - buffer->position;
		np = INT->size;
	}
	memmove(destination, (brrb1 *)INT->data + buffer->position, read_size);
	buffer->position = np;
	return read_size;
}
brrsz BRRCALL
brrbuffer_read(brrbufferT *const buffer, void *const destination, brrsz read_size)
{
	if (!buffer || !buffer->opaque)
		return 0;
	if (!destination || !read_size)
		return 0;
	return buffint_read(buffer, destination, read_size);
}

brrsz BRRCALL
brrbuffer_read_to(brrbufferT *const source, brrbufferT *const destination, brrsz read_size)
{
	brrby *tmp = NULL;
	if (!source || !source->opaque || !destination || !destination->opaque)
		return 0;
	if (!read_size)
		return 0;

	if (!brrlib_alloc((void **)&tmp, read_size, 1))
		return 0;

	read_size = buffint_read(source, tmp, read_size);
	read_size = buffint_write(destination, tmp, read_size);

	brrlib_alloc((void **)&tmp, 0, 0);

	return read_size;
}

static brrsz BRRCALL buffint_segment(buffint *const INT, void *const destination, brrsz start, brrsz end) {
	brrsz rd = 0;
	if (end < start) {
		for (brrsz rv = start - end; rd < rv; ++rd) {
			((brrby *)destination)[rd] = ((brrby *)(INT->data))[start - 1 - rd];
		}
	} else {
		for (brrsz rv = end - start; rd < rv; ++rd) {
			((brrby *)destination)[rd] = ((brrby *)(INT->data))[start + rd];
		}
	}
	return rd;
}
brrsz BRRCALL
brrbuffer_segment(brrbufferT *const buffer, void *const destination, brrsz start, brrsz end)
{
	buffint *INT = NULL;
	if (!buffer || !buffer->opaque || !destination)
		return 0;
	INT = (buffint *)buffer->opaque;
	if (start > INT->size)
		start = INT->size;
	if (end > INT->size)
		end = INT->size;
	if (start == end)
		return 0;

	return buffint_segment(INT, destination, start, end);
}

brrsz BRRCALL
brrbuffer_segment_to(brrbufferT *const source, brrbufferT *const destination, brrsz start, brrsz end)
{
	brrsz rd = 0;
	buffint *INT = NULL;
	brrby *tmp = NULL;
	if (!source || !source->opaque || !destination || !destination->opaque)
		return 0;

	INT = (buffint *)source->opaque;
	if (start > INT->size)
		start = INT->size;
	if (end > INT->size)
		end = INT->size;
	if (start == end)
		return 0;

	if (end < start) {
		rd = start - end;
	} else {
		rd = end - start;
	}

	if (!brrlib_alloc((void **)&tmp, rd, 1))
		return 0;
	rd = buffint_segment(INT, tmp, start, end);
	rd = buffint_write(destination, tmp, rd);
	destination->position -= rd;
	brrlib_alloc((void **)&tmp, 0, 0);

	return rd;
}

brrb1 BRRCALL
brrbuffer_find_next(brrbufferT *const buffer, const void *const key, brrsz key_length)
{
	if (!buffer || !buffer->opaque || !key || !key_length) {
		return false;
	} else {
		buffint *INT = (buffint *)buffer->opaque;
		if (!INT->size || key_length > (INT->size - buffer->position))
			return false;
		if (INT->data) {
			brrby *block = NULL;
			if (!brrlib_alloc((void **)&block, key_length, 1))
				return false;
			for (brrsz pos = buffer->position, len = INT->size - key_length; pos < len; ++pos) {
				memcpy(block, (brrby *)INT->data + pos, key_length);
				if (0 == memcmp(block, key, key_length)) {
					buffer->position = pos;
					return true;
				}
			}
		}
	}
	return false;
}

brrb1 BRRCALL
brrbuffer_find_previous(brrbufferT *const buffer, const void *const key, brrsz key_length)
{
	if (!buffer || !buffer->opaque || !key || !key_length) {
		return false;
	} else {
		buffint *INT = (buffint *)buffer->opaque;
		if (!INT->size || key_length > (INT->size - buffer->position))
			return false;
		if (INT->data) {
			brrby *block = NULL;
			if (!brrlib_alloc((void **)&block, key_length, 1))
				return false;
			for (brrsz pos = 0; pos <= buffer->position; ++pos) {
				memcpy(block, (brrby *)INT->data + buffer->position - pos, key_length);
				if (0 == memcmp(block, key, key_length)) {
					buffer->position -= pos;
					return true;
				}
			}
		}
	}
	return false;
}

brrsz BRRCALL
brrbuffer_vnprintf(brrbufferT *const buffer, brrsz max_size, const char *const fmt, va_list lptr)
{
	char *c = NULL;
	brrsz wt = 0;

	if (!buffer || !buffer->opaque || !max_size || !fmt)
		return 0;

	if (brrlib_alloc((void **)&c, max_size, 1)) {
		wt = vsnprintf(c, max_size, fmt, lptr);
		wt = brrbuffer_write(buffer, c, wt + 1);
		brrlib_alloc((void **)&c, 0, 0);
	}

	return wt;
}

brrsz BRRCALL
brrbuffer_nprintf(brrbufferT *const buffer, brrsz max_size, const char *const fmt, ...)
{
	va_list lptr;
	brrsz wt = 0;
	va_start(lptr, fmt);
	wt = brrbuffer_vnprintf(buffer, max_size, fmt, lptr);
	va_end(lptr);
	return wt;
}

brrsz BRRCALL
brrbuffer_vprintf(brrbufferT *const buffer, const char *const fmt, va_list lptr)
{
	brrsz max_len;
	if (!buffer || !buffer->opaque || !fmt)
		return 0;
	max_len = ((buffint *)buffer->opaque)->size - buffer->position;
	return brrbuffer_vnprintf(buffer, max_len, fmt, lptr);
}

brrsz BRRCALL
brrbuffer_printf(brrbufferT *const buffer, const char *const fmt, ...)
{
	va_list lptr;
	brrsz wt = 0;
	va_start(lptr, fmt);
	wt = brrbuffer_vprintf(buffer,fmt, lptr);
	va_end(lptr);
	return wt;
}

brrbufferT BRRCALL
brrbuffer_from_file(int file_descriptor)
{
	brrbufferT buff = {0};
	buffint *INT = NULL;
	brrpath_statT st = brrpath_fstat(file_descriptor);
	if (st.error == 0) {
		if (!brrlib_alloc((void **)&INT, sizeof(buffint), 1))
			return buff;
		if (st.type & brrpath_type_file) {
			int er = 0;
			brrs8 start = 0;
			INT->capacity = buffcap(st.size);
			if (!brrlib_alloc((void **)&INT->data, INT->capacity, 1)) {
				brrlib_alloc((void **)&INT, 0, 0);
			} else {
				/* 32-bit vs 64-bit files, filesystems, operating systems, complicated mess */
				er = (-1 == (start = brr_lseek(file_descriptor, 0, SEEK_CUR)));
				if (!er)
					er = (-1 == brr_lseek(file_descriptor, 0, SEEK_SET));
				if (!er)
					er = (-1 == brr_read(file_descriptor, INT->data, st.size));
				if (!er)
					er = (-1 == brr_lseek(file_descriptor, start, SEEK_SET));
				if (!er) {
					INT->size = st.size;
				} else {
					brrlib_alloc((void **)&INT->data, 0, 0);
					brrlib_alloc((void **)&INT, 0, 0);
				}
				buff.opaque = INT;
			}
		}
	}
	return buff;
}

brrb1 BRRCALL
brrbuffer_to_file(const brrbufferT *const buffer, int file_descriptor)
{
	brrb1 r = false;
	if (buffer && buffer->opaque) {
		buffint *INT = (buffint *)buffer->opaque;
		if (INT->data && INT->size) {
			r = (-1 != brr_write(file_descriptor, INT->data, INT->size));
		} else {
			r = true;
		}
	}
	return r;
}
