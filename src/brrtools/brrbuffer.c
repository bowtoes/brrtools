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

#include <string.h>

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

brrsz brrbuffer_capacity_increment = 2048;

typedef struct {
	void *data;
	brrsz capacity;
} buffint;

static brrsz buffcap(brrsz size) {
	brrsz mul = size / brrbuffer_capacity_increment;
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
	buff.size = size;
	INT->capacity = buffcap(buff.size);
	if (!brrlib_alloc(&INT->data, INT->capacity, 1)) {
		brrlib_alloc((void **)&INT, 0, 0);
		buff.size = 0;
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
		buff.size = other->size;
		if (oth) {
			buffint *INT = NULL;
			if (brrlib_alloc((void **)&INT, sizeof(buffint), 1)) {
				INT->capacity = oth->capacity;
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
				buffer->size = new_size;
				INT->capacity = cap;
			}
		} else {
			r = true;
		}
	}
	return r;
}

const void *BRRCALL
brrbuffer_data(const brrbufferT *const buffer)
{
	const void *r = NULL;
	if (buffer && buffer->opaque) {
		buffint *INT = (buffint *)buffer->opaque;
		r = INT->data;
	}
	return r;
}

brrsz BRRCALL
brrbuffer_write(brrbufferT *const buffer, const void *const data, brrsz data_size)
{
	buffint *INT = NULL;
	brrsz np = 0;
	if (!buffer || !buffer->opaque)
		return 0;
	if (!data || !data_size)
		return buffer->position;
	INT = (buffint *)buffer->opaque;
	np = buffer->position + data_size;
	if (np > INT->capacity) {
		brrsz cap = buffcap(np);
		// Do nothing on failure.
		if (!brrlib_alloc(&INT->data, cap, 0))
			return 0;
		INT->capacity = cap;
	}
	if (np > buffer->size)
		buffer->size = np;
	memmove((brrby *)INT->data + buffer->position, data, data_size);
	buffer->position = np;
	return np;
}

brrsz BRRCALL
brrbuffer_read(brrbufferT *const buffer, void *const destination, brrsz read_size)
{
	buffint *INT = NULL;
	brrsz np = 0;
	if (!buffer || !buffer->opaque)
		return 0;
	if (!destination || !read_size)
		return 0;
	INT = (buffint *)buffer->opaque;
	np = buffer->position + read_size;
	if (np > buffer->size) {
		read_size = buffer->size - buffer->position;
		np = buffer->size;
	}
	memmove(destination, (brrb1 *)INT->data + buffer->position, read_size);
	buffer->position = np;
	return np;
}

/* TODO needs testing */
brrb1 BRRCALL
brrbuffer_find_next(brrbufferT *const buffer, const void *const key, brrsz key_length)
{
	if (!buffer || !buffer->opaque || !buffer->size ||
        !key    || !key_length     || key_length > (buffer->size - buffer->position)) {
		return false;
	} else {
		buffint *INT = (buffint *)buffer->opaque;
		if (INT->data) {
			brrby *block = NULL;
			if (!brrlib_alloc((void **)&block, key_length, 1))
				return false;
			for (brrsz pos = buffer->position, len = buffer->size - key_length; pos < len; ++pos) {
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

/* TODO needs testing */
brrb1 BRRCALL
brrbuffer_find_previous(brrbufferT *const buffer, const void *const key, brrsz key_length)
{
	if (!buffer || !buffer->opaque || !buffer->size ||
        !key    || !key_length     || key_length > (buffer->size - buffer->position)) {
		return false;
	} else {
		buffint *INT = (buffint *)buffer->opaque;
		if (INT->data) {
			brrby *block = NULL;
			if (!brrlib_alloc((void **)&block, key_length, 1))
				return false;
			for (brrsz pos = 0; pos <= buffer->position; ++pos) {
				memcpy(block, (brrby *)INT->data + buffer->position - pos, key_length);
				if (0 == memcmp(block, key, key_length)) {
					buffer->position = pos;
					return true;
				}
			}
		}
	}
	return false;
}

#if 0
brrbufferT BRRCALL
brrbuffer_fromfile(int fd)
{
	brrbufferT buff = {0};
	buffint *INT = NULL;
	brrpath_statT st = brrpath_fstat(fd);
	if (st.stat_error == 0) {
		if (!brrlib_alloc((void **)&INT, sizeof(buffint), 1))
			return buff;
		if (st.exists && st.type & brrpath_file) {
			int er = 0;
			buff.size = st.size;
			INT->capacity = buffcap(buff.size);
			if (!brrlib_alloc((void **)&INT->data, INT->capacity, 1)) {
				brrlib_alloc((void **)&INT, 0, 0);
				return buff;
			}
#if defined(BRRPLATFORMTYPE_WINDOWS)
			er = (-1 == _read(fd, INT->data, buff.size));
#else
			er = (-1 == read(fd, INT->data, buff.size));
#endif
			if (er) {
				brrlib_alloc((void **)&INT->data, 0, 0);
				brrlib_alloc((void **)&INT, 0, 0);
				buff.size = 0;
			}
			buff.opaque = INT;
		}
	}
	return buff;
}

brrb1 BRRCALL
brrbuffer_tofile(const brrbufferT *const buffer, int fd)
{
	brrb1 r = false;
	if (buffer && buffer->opaque) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		r = (-1 != _write(fd, ((buffint *)buffer->opaque)->data, buffer->size));
#else
		r = (-1 != write(fd, ((buffint *)buffer->opaque)->data, buffer->size));
#endif
	}
	return r;
}
#endif
