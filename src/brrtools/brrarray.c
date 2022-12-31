/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrarray.h"

#include <stdlib.h>
#include <string.h>

#define _check_narray(_array_, _elsize_, _name_) do {\
	if (!(_array_)) {\
		brrapi_error(BRRAPI_E_ARGERR, "%s given NULL " _name_, __func__);\
		return -1;\
	} else if (!(_elsize_)) {\
		brrapi_error(BRRAPI_E_ARGERR, "%s given invalid element size", __func__);\
		return -1;\
	}\
} while (0)
#define _check_array(_array_, _name_) do {\
	if (!(_array_)) {\
		brrapi_error(BRRAPI_E_ARGERR, "%s given NULL " _name_, __func__);\
		return -1;\
	} else if (!(_array_)->size) {\
		brrapi_error(BRRAPI_E_ARGERR, "%s given invalid " _name_, __func__);\
		return -1;\
	}\
} while (0)

static BRR_inline int BRRCALL
i_resize_array(brrarray_t *const array, brrsz new_count)
{
	if (new_count == array->count)
		return 0;
	brrarray_t n = {
		.count = new_count,
		.size = array->size,
		._bytes = n.count * n.size,
	};
	if (!new_count) {
		if (n.data)
			free(n.data);
		*array = n;
		return 0;
	} else if (!(n.data = realloc(array->data, new_count * n.size))) {
		brrapi_error(BRRAPI_E_MEMERR, "Failed to resize array to %zu bytes (%zu + 1 elements of %zu bytes)", new_count * n.size, n.count, n.size);
		return -1;
	}
	return 0;
}
static BRR_inline int BRRCALL
i_new_array(brrarray_t *const arr, brrsz size, brrsz count, const void *const data)
{
	brrarray_t n = {
		.size = size,
		.count = count,
		._bytes = count * size,
	};
	if (n._bytes) {
		if (!(n.data = malloc(n._bytes))) {
			brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate space for new array of %zu bytes (%zu elements of %zu bytes)", n._bytes, count, size);
			return -1;
		}
		if (data) {
			memcpy(n.data, data, n._bytes);
		}
	}
	*arr = n;
	return 0;
}

int BRRCALL
brrarray_new(brrarray_t *array, const void *const elements, brrsz element_size, brrsz n_elements)
{
	_check_narray(array, element_size, "destination array");
	return i_new_array(array, element_size, n_elements, elements);
}
int BRRCALL
brrarray_new_data(brrarray_t *array, const void *const data, brrsz data_size, brrsz element_size)
{
	_check_narray(array, element_size, "destination array");
	return i_new_array(array, element_size, data_size / element_size, data);
}
void BRRCALL
brrarray_free(brrarray_t *const array, brrarray_freer_t freer)
{
	if (!array)
		return;
	if (array->data) {
		if (freer) {
			brrsz s = array->size;
			char *d = array->data;
			for (brrsz i = 0; i < array->_bytes; i += s)
				freer(d + i);
		}
		free(array->data);
	}
	memset(array, 0, sizeof(*array));
}

int BRRCALL
brrarray_resize(brrarray_t *const array, brrsz new_count)
{
	_check_array(array, "array");
	brrarray_t old = *array;
	if (i_resize_array(array, new_count))
		return -1;
	if (new_count > old.count)
		memset((char *)array->data + old._bytes, 0, array->_bytes - old._bytes);
	return 0;
}

int BRRCALL
brrarray_append(brrarray_t *const array, const void *const elements, brrsz count)
{
	_check_array(array, "array");
	if (!elements || !count)
		return 0;
	brrarray_t old = *array;
	if (i_resize_array(array, old.count + count))
		return -1;
	brrsz nsize = count * old.size;
	memcpy((char *)array->data + old._bytes, elements, nsize);
	return 0;
}
int BRRCALL
brrarray_prepend(brrarray_t *const array, const void *const elements, brrsz count)
{
	_check_array(array, "array");
	if (!elements || !count)
		return 0;
	brrarray_t old = *array;
	if (i_resize_array(array, old.count + count))
		return -1;
	brrsz nsize = count * old.size;
	memmove((char *)array->data + nsize, array, old._bytes);
	memcpy(array->data, elements, nsize);
	return 0;
}

int BRRCALL
brrarray_split(const brrarray_t *const array, brrsz split, brrarray_t *const a, brrarray_t *const b)
{
	_check_array(array, "source array");
	if (split >= array->count) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_split given split location %zu out of range for array of %zu elements", split, array->count);
		return -1;
	}
	if (!a && !b)
		return 0;

	brrsz s = array->size;
	char *d = array->data;

	brrarray_t A = {
		.size = s,
		.count = split + 1,
	};
	A._bytes = A.count * s;
	if (a) {
		if (!(A.data = malloc(A._bytes))) {
			brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate space for array split A");
			return -1;
		}
		memcpy(A.data, d, A._bytes);
	}

	brrarray_t B = {
		.size = s,
		.count = array->count - A.count,
	};
	B._bytes = B.count * s;
	if (b) {
		if (!(B.data = malloc(B._bytes))) {
			if (A.data)
				free(A.data);
			brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate space for array split B");
			return -1;
		}
		memcpy(B.data, d + A._bytes, B._bytes);
		*b = B;
	}
	if (a)
		*a = A;

	return 0;
}
int BRRCALL
brrarray_partition(const brrarray_t *const array, brrsz part, brrarray_t *const a, brrarray_t *const b)
{
	_check_array(array, "source array");
	if (part >= array->count) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_partition given partition location %zu out of range for array of %zu elements", part, array->count);
		return -1;
	}
	if (!a && !b)
		return 0;

	brrsz s = array->size;
	char *d = array->data;
	brrarray_t A = {
		.size = s,
		.count = part + 1,
	};
	A._bytes = A.count * s;
	A.data = d;
	brrarray_t B = {
		.size = s,
		.count = array->count - A.count,
	};
	B._bytes = B.count * s;
	B.data = d + A._bytes;

	if (a)
		*a = A;
	if (b)
		*b = B;
	return 0;
}

int BRRCALL
brrarray_insert(brrarray_t *const array, const brrarray_t new, brrsz start)
{
	_check_array(array, "array");
	_check_array(&new, "new array");
	if (start > array->count){
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_insert given insert position %zu out of range for array of %zu elements", start, array->count);
		return -1;
	}
	if (!new.count)
		return 0;

	brrsz ocount = array->count, size = array->size;
	if (i_resize_array(array, ocount + new.count))
		return -1;

	char *d = array->data;
	memmove(d + (new.count + start) * size, d + start * size, (ocount - start) * size);
	memcpy(d + start * size, new.data, new.count * size);
	return 0;
}
int BRRCALL
brrarray_remove(brrarray_t *const array, brrsz count, brrsz start, brrarray_freer_t freer)
{
	_check_array(array, "array");
	if (start > array->count) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_remove given start position %zu out of range for array of %zu elements", start, array->count);
		return -1;
	} else if (count > array->count - start) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_remove given more elements to remove %zu than are available in the rest of array %zu", count, array->count - start);
		return -1;
	}
	if (!count || start == array->count)
		return 0;

	brrarray_t n = {
		.count = array->count - count,
		.size = array->size,
	};
	n._bytes = n.count * n.size;
	if (!(n.data = malloc(n._bytes))) {
		brrapi_error(BRRAPI_E_MEMERR, "Failed to allocate shrunken space for array when removing %zu elements (%zu bytes)", count, array->size);
		return -1;
	}
	char *beforedata = array->data;
	brrsz beforelen = start * n.size;
	if (beforelen)
		memcpy(n.data, beforedata, beforelen);
	char *rmdata = beforedata + beforelen;
	brrsz rmlen = count * n.size;
	if (freer) {
		for (brrsz i = 0; i < rmlen; i += n.size)
			freer(rmdata + i);
	}
	char *afterdata = rmdata + rmlen;
	brrsz afterlen = array->_bytes - beforelen - rmlen;
	if (afterlen)
		memcpy((char*)n.data + beforelen, afterdata, afterlen);
	free(array->data);
	*array = n;
	return 0;
}

int BRRCALL
brrarray_swap(brrarray_t *const array, brrsz count, brrsz region_a, brrsz region_b)
{
	_check_array(array, "array");
	if (region_a > array->count) {
		brrapi_error(BRRAPI_E_ARGERR,
		    "brrarray_swap Region A start %zu is out of bounds in array with %zu elements",
		    region_a, array->count);
		return -1;
	} else if (region_b > array->count) {
		brrapi_error(BRRAPI_E_ARGERR,
		    "brrarray_swap Region B start %zu is out of bounds in array with %zu elements",
		    region_b, array->count);
		return -1;
	} else if (region_a + count > array->count) {
		brrapi_error(BRRAPI_E_ARGERR,
		    "brrarray_swap Region A %zu of %zu elements is out of bounds in array with %zu elements",
		    region_a, count, array->count);
		return -1;
	} else if (region_b + count > array->count) {
		brrapi_error(BRRAPI_E_ARGERR,
		    "brrarray_swap Region B %zu of %zu elements is out of bounds in array with %zu elements",
		    region_b, count, array->count);
		return -1;
	}
	if (!count || region_a == region_b)
		return 0;
	if (region_a < region_b) {
		brrsz a = region_a;
		region_a = region_b;
		region_b = a;
	}
	if (region_a + count >= region_b) {
		/* Overlap, swap only the bits that don't overlap */
		brrsz overlen = region_a + count - region_b;
		count -= overlen;
		region_b += overlen;
	}
	char *a = (char *)array->data + region_a * array->size;
	char *b = (char *)array->data + region_b * array->size;
	brrsz len = count * array->size;
	for (brrsz i = 0; i < len; ++i) {
		char t = a[i];
		a[i] = b[i];
		b[i] = t;
	}

	return 0;
}

int BRRCALL
brrarray_read(const brrarray_t *const array, brrsz element, void *const dst)
{
	_check_array(array, "source array");
	if (element >= array->count) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_read element %zu is out of bounds in array with %zu elements", array->count);
		return -1;
	}
	if (dst)
		memcpy(dst, (char*)array->data + element * array->size, array->size);
	return 0;
}
int BRRCALL
brrarray_write(brrarray_t *const array, brrsz element, const void *const src)
{
	_check_array(array, "destination array");
	if (element >= array->count) {
		brrapi_error(BRRAPI_E_ARGERR, "brrarray_write element %zu is out of bounds in array with %zu elements", array->count);
		return -1;
	}
	memcpy((char*)array->data + element * array->size, src, array->size);
	return 0;
}

void BRRCALL
brrarray_sort(brrarray_t *const array, brrdata_comparer_t compare, void *parameter)
{
	if (brrarray_check(array) || !compare || array->count < 2)
		return;

	char *d = array->data;
	char *e = d + array->_bytes;
	brrsz s = array->size;
	for (char *i = d; i < e; i += s) {
		char *min = i;
		for (char *j = i; j < e; j += s) {
			if (compare(j, min, parameter))
				min = j;
		}
		for (brrsz b = 0; b < s; ++b) {
			char t = i[b];
			i[b] = min[b];
			min[b] = t;
		}
	}
}

void BRRCALL
brrarray_reverse(brrarray_t *const array, brrsz start, brrsz count)
{
	if (!array || start >= array->count)
		return;
	if (count > array->count - start)
		count = array->count - start;

	brrsz s = array->size;
	char *d = (char*)array->data + start * s;
	for (brrsz i = 0; i < count / 2; ++i) {
		char *a = d + i * s;
		char *b = d + (count - i) * s;
		for (brrsz j = 0; j < s; ++j) {
			char t = a[j];
			a[j] = b[j];
			b[j] = t;
		}
	}
}
void BRRCALL
brrarray_reverse_bytes(brrarray_t *const array, brrsz start, brrsz length)
{
	if (!array || start >= array->_bytes)
		return;
	if (length > array->_bytes - start)
		length = array->_bytes - start;

	char *d = (char *)array->data + start;
	for (brrsz i = 0; i < length / 2; ++i) {
		char t = d[i];
		d[i] = d[length - i];
		d[length - i] = t;
	}
}
