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

#define BUFFER_CAPACITY_INCREMENT 101

typedef struct {
	void *data;
	brrsz capacity;
} buffint;

static brrsz buffcap(brrsz size) {
	brrsz mul = size / BUFFER_CAPACITY_INCREMENT;
	if (BUFFER_CAPACITY_INCREMENT * mul == size) {
		return size;
	} else {
		return BUFFER_CAPACITY_INCREMENT * (mul + 1);
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
	BRRLOG_DEB("Buffer Capacity %zu", INT->capacity);
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
				INT->data = brrmem_dup(oth->data, oth->capacity);
			}
			buff.opaque = INT;
		}
	}
	return buff;
}

void BRRCALL
brrbuffer_del(brrbufferT *const buffer)
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

void *BRRCALL
brrbuffer_data(const brrbufferT *const buffer)
{
	void *r = NULL;
	if (buffer && buffer->opaque) {
		buffint *INT = (buffint *)buffer->opaque;
		r = INT->data;
	}
	return r;
}
