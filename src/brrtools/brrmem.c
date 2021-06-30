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

#include "brrtools/brrmem.h"

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrapi.h"
#include "brrtools/brrlib.h"

#define BFVALID(_d) ((_d) && (_d##_size))

void BRRCALL
brrmem_swap(void *const data0, void *const data1, brrsz data_size)
{
	if (!data0 || !data1 || !data_size)
		return;
	_brrmem_swap(data0, data1, data_size);
}

brrsz BRRCALL
brrmem_next(const void *const data, brrsz data_size, brrby key, brrsz offset)
{
	if (!data) {
		return offset;
	} else if (offset >= data_size) {
		return data_size;
	} else {
		for (;((brrby *)data)[offset] != key && offset < data_size;++offset);
		return offset;
	}
}
brrsz BRRCALL
brrmem_previous(const void *const data, brrsz data_size, brrby key, brrsz offset)
{
	if (!BFVALID(data)) {
		return offset;
	} else if (offset >= data_size) {
		return data_size;
	} else {
		for (;((brrby *)data)[offset] != key && offset < data_size; --offset);
		return offset<data_size?offset:data_size;
	}
}

void *BRRCALL
brrmem_duplicate(const void *const data, brrsz data_size)
{
	void *r = NULL;
	if (!BFVALID(data)) {
		return NULL;
	} else if (!brrlib_alloc(&r, data_size, 1)) {
		return NULL;
	} else {
		return memcpy(r, data, data_size);
	}
}
brrsz BRRCALL
brrmem_copy(const void *const source, brrsz source_size,
    void *const destination, brrsz destination_size)
{
	if (!BFVALID(source) || !BFVALID(destination)) {
		return 0;
	} else {
		brrsz m = source_size<destination_size?source_size:destination_size;
		memmove(destination, source, m);
		return m;
	}
}

brrsz BRRCALL
brrmem_search(const void *const data, brrsz data_size,
    const void *const key, brrsz key_size, brrsz offset)
{
	if (!BFVALID(data) || !BFVALID(key)) {
		return offset;
	} else if (key_size > data_size || offset > data_size - key_size) {
		return data_size;
	} else {
		int cmp = 0;
		for (brrsz tst = data_size - (key_size - 1);!cmp && offset < tst; ++offset)
			cmp = (0 == memcmp((brrby *)data + offset, key, key_size));
		return cmp ? offset - 1 : data_size;
	}
}
brrsz BRRCALL
brrmem_search_reverse(const void *const data, brrsz data_size,
    const void *const key, brrsz key_size, brrsz offset)
{
	if (!BFVALID(data) || !BFVALID(key)) {
		return offset;
	} else if (key_size > data_size || offset > data_size - key_size) {
		return data_size;
	} else {
		int cmp = 0;
		for (;!cmp && offset < data_size; --offset)
			cmp = (0 == memcmp((brrby *)data + offset, key, key_size));
		return cmp ? offset + 1 : data_size;
	}
}

void *BRRCALL
brrmem_static_reverse(void *const data, brrsz data_size)
{
	if (!BFVALID(data)) {
		return NULL;
	} else {
		for (brrsz i = 0; i < data_size/2; ++i) {
			brrby t=((brrby*)data)[i];
			((brrby*)data)[i]=((brrby*)data)[data_size-1-i];
			((brrby*)data)[data_size-1-i]=t;
		}
		return data;
	}
}
void *BRRCALL
brrmem_reverse(const void *const data, brrsz data_size)
{
	void *r = NULL;
	if (!BFVALID(data)) {
		return NULL;
	} else if (!brrlib_alloc(&r, data_size, 0)) {
		return NULL;
	} else {
		for (brrsz i = 0; i < data_size; ++i)
			((brrby *)r)[i] = ((brrby *)data)[data_size - 1 - i];
		return r;
	}
}
void *BRRCALL
brrmem_static_reverse_blocks(void *const data, brrsz block_size, brrsz block_count)
{
	if (!data) {
		return NULL;
	} else if (!block_size || !block_count) {
		return data;
	} else {
		for (brrsz i = 0; i < block_count / 2; ++i) {
			_brrmem_swap(BRRMEM_BLOCK(data, block_size, i),
			    BRRMEM_BLOCK(data, block_size, block_count - i - 1), block_size);
		}
		return data;
	}
}
void *BRRCALL
brrmem_reverse_blocks(const void *const data, brrsz block_size, brrsz block_count)
{
	void *r = NULL;
	if (!data || !block_size || !block_count) {
		return NULL;
	} else if (!brrlib_alloc(&r, block_size * block_count, 0)) {
		return NULL;
	} else {
		for (brrsz i = 0; i < block_count; ++i) {
			memcpy(BRRMEM_BLOCK(r, block_size, i),
			    BRRMEM_BLOCK(data, block_size, block_count - i - 1), block_size);
		}
		return r;
	}
}

void *BRRCALL
brrmem_join(const void *const data_a, brrsz data_a_size,
    const void *const data_b, brrsz data_b_size, brrsz *const new_size)
{
	void *r = NULL;
	if (!BFVALID(data_a) || !BFVALID(data_b)) {
		return NULL;
	} else if (!brrlib_alloc(&r, data_a_size + data_b_size, 1)) {
		return NULL;
	} else {
		memcpy(r, data_a, data_a_size);
		memcpy((brrby *)r + data_a_size, data_b, data_b_size);
		if (new_size)
			*new_size = data_a_size + data_b_size;
		return r;
	}
}
void *BRRCALL
brrmem_append(void *data, brrsz data_size,
    const void *const suffix, brrsz suffix_size, brrsz *const new_size)
{
	if (!BFVALID(data)) {
		return NULL;
	} else if (!BFVALID(suffix)) {
		return data;
	} else if (!brrlib_alloc(&data, data_size + suffix_size, 0)) {
		return NULL;
	} else {
		memmove((brrsz *)data + data_size, suffix, suffix_size);
		if (new_size)
			*new_size = data_size + suffix_size;
		return data;
	}
}
void *BRRCALL
brrmem_prepend(void *data, brrsz data_size,
    const void *const prefix, brrsz prefix_size, brrsz *const new_size)
{
	if (!BFVALID(data)) {
		return NULL;
	} else if (!BFVALID(prefix)) {
		return data;
	} else if (!brrlib_alloc(&data, data_size + prefix_size, 0)) {
		return NULL;
	} else {
		memmove((brrsz *)data + prefix_size, data, data_size);
		memmove((brrsz *)data, prefix, prefix_size);
		if (new_size)
			*new_size = data_size + prefix_size;
		return (void *)data;
	}
}

brrsz BRRCALL
brrmem_slice(const void *const data, brrsz data_size,
    void *const destination, brrsz start, brrsz end_inclusive)
{
	if (!BFVALID(data))
		return 0;

	if (end_inclusive > data_size)
		end_inclusive = data_size;
	if (start > data_size)
		start = data_size;

	if (start == end_inclusive) {
		return 0;
	} else {
		brrsz cp = 0;
		if (start < end_inclusive) {
			for (brrsz i = 0, l = end_inclusive - start; i < l; ++i, ++cp) {
				((brrby *)destination)[i] = ((brrby *)data)[start + i];
			}
		} else {
			for (brrsz i = 0, l = start - end_inclusive; i < l; ++i, ++cp) {
				((brrby *)destination)[i] = ((brrby *)data)[start - i - 1];
			}
		}
		return cp;
	}
}
