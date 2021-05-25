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

#include <string.h>

#include "brrtools/brrapi.h"
#include "brrtools/brrlib.h"

#define BFVALID(_d,_s) ((_d) && (_s))

brrsz BRRCALL
brrmem_next(const void *const data, brrsz data_size, brrby key, brrsz offset)
{
	if (offset > data_size) {
		return data_size;
	} else if (BFVALID(data, data_size)) {
		for (;((brrby *)data)[offset] != key && offset < data_size;++offset);
	}
	return offset;
}

brrsz BRRCALL
brrmem_prev(const void *const data, brrsz data_size, brrby key, brrsz offset)
{
	if (offset > data_size) {
		return data_size;
	} else if (BFVALID(data, data_size)) {
		for (;((brrby *)data)[offset] != key && offset > 0; --offset);
	}
	return offset;
}

void *BRRCALL
brrmem_dup(const void *const data, brrsz data_size)
{
	void *r = NULL;
	if (BFVALID(data, data_size)) {
		if (brrlib_alloc(&r, data_size, true)) {
			memcpy(r, data, data_size);
		}
	}
	return r;
}

void *BRRCALL
brrmem_join(const void *const data_a, brrsz data_a_size,
    const void *const data_b, brrsz data_b_size, brrsz *const new_length)
{
	void *r = NULL;
	brrsz l = 0;
	if (!BFVALID(data_a,data_a_size) && !BFVALID(data_b,data_b_size)) {
		r = NULL;
	} else if (!BFVALID(data_a,data_a_size)) {
		r = brrmem_dup(data_b, data_b_size);
	} else if (!BFVALID(data_b,data_b_size)) {
		r = brrmem_dup(data_a, data_a_size);
	} else {
		l = data_a_size + data_b_size;
		if (brrlib_alloc(&r, l, true)) {
			memcpy(r, data_a, data_a_size);
			memcpy((brrby *)r + data_a_size, data_b, data_b_size);
		}
	}
	if (new_length) *new_length = l;
	return r;
}

void *BRRCALL
brrmem_append(void *data, brrsz data_size,
    const void *const suffix, brrsz suffix_size, brrsz *const new_length)
{
	if (!BFVALID(data, data_size)) {
		if (new_length) *new_length = 0;
		return NULL;
	} else if (!BFVALID(suffix, suffix_size)) {
		if (new_length) *new_length = data_size;
		return (void *)data;
	} else {
		brrsz ns = data_size + suffix_size;
		if (brrlib_alloc(&data, ns, false)) {
			// memmove just in case 'suffix' is 'data'.
			memmove((brrsz *)data + data_size, suffix, suffix_size);
		} else {
			ns = data_size;
		}
		if (new_length) *new_length = ns;
		return (void *)data;
	}
}

void *BRRCALL
brrmem_prepend(void *data, brrsz data_size,
    const void *const prefix, brrsz prefix_size, brrsz *const new_length)
{
	if (!BFVALID(data, data_size)) {
		if (new_length) *new_length = 0;
		return NULL;
	} else if (!BFVALID(prefix, prefix_size)) {
		if (new_length) *new_length = data_size;
		return (void *)data;
	} else {
		brrsz ns = data_size + prefix_size;
		if (brrlib_alloc(&data, ns, false)) {
			memmove((brrsz *)data + prefix_size, data, data_size);
			memmove((brrsz *)data, prefix, prefix_size);
		} else {
			ns = data_size;
		}
		if (new_length) *new_length = ns;
		return (void *)data;
	}
}

void *BRRCALL
brrmem_static_reverse(void *const data, brrsz data_size)
{
	if (!BFVALID(data, data_size))
		return NULL;
	for (brrsz i = 0; i < data_size/2; ++i) {
		brrby t=((brrby*)data)[i];
		((brrby*)data)[i]=((brrby*)data)[data_size-1-i];
		((brrby*)data)[data_size-1-i]=t;
	}
	return data;
}

void *BRRCALL
brrmem_reverse(const void *const data, brrsz data_size)
{
	void *r = NULL;
	if (BFVALID(data, data_size)) {
		if (brrlib_alloc(&r, data_size, 0)) {
			for (brrsz i = 0; i < data_size; ++i) {
				((brrby *)r)[i] = ((brrby *)data)[data_size - 1 - i];
			}
		}
	}
	return r;
}

#define ELIDX(data, idx, elementsize) (((brrby *)(data)) + ((idx) * (elementsize)))
void *BRRCALL
brrmem_static_reverse_elements(void *const data, brrsz element_size, brrct element_count)
{
	if (!data || !element_size)
		return NULL;
	if (!element_count)
		return data;
	for (brrct i = 0; i < element_count / 2; ++i) {
		brrby *a = ELIDX(data, i, element_size);
		brrby *b = ELIDX(data, element_count - 1 - i, element_size);
		for (brrsz j = 0; j < element_size; ++j) {
			brrby c = a[j];
			a[j] = b[j];
			b[j] = c;
		}
	}
	return data;
}

void *BRRCALL
brrmem_reverse_elements(const void *const data, brrsz element_size, brrct element_count)
{
	void *r = NULL;
	if (BFVALID(data, element_size && element_count)) {
		if (brrlib_alloc(&r, element_size * element_count, true)) {
			for (brrct i = 0; i < element_count; ++i) {
				brrby *a = ELIDX(r, i, element_size);
				brrby *b = ELIDX(data, element_count - 1 - i, element_size);
				for (brrsz j = 0; j < element_size; ++j) {
					a[j] = b[j];
				}
			}
		}
	}
	return r;
}
