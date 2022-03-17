/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

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

#include "brrtools/brrheap.h"

#include <stdlib.h>
#include <string.h>

static int BRRCALL
i_resize_array(void **const array, brrsz new_size, brrsz old_size)
{
	void *a = NULL;
	if (new_size < old_size) {
		if (new_size == 0) {
			if (*array)
				free(*array);
			*array = NULL;
		} else {
			if (!(a = realloc(*array, new_size)))
				return -1;
			*array = a;
		}
	} else if (new_size > old_size) {
		if (!*array)
			a = malloc(new_size);
		else
			a = realloc(*array, new_size);
		if (!a)
			return -1;
		*array = a;
	}
	return 0;
}
int BRRCALL
brrheap_resize(void **const array, brrsz n_elements, brrsz data_size, brrsz new_elements)
{
	if (!array)
		return -1;
	else if (i_resize_array(array, new_elements * data_size, n_elements * data_size))
		return -1;
	if (new_elements > n_elements)
		memset((char *)*array + (n_elements)*data_size, 0, (new_elements - n_elements)*data_size);
	return 0;
}

int BRRCALL
brrheap_append(void **const array, brrsz *const n_elements, brrsz data_size, const void *const new_data)
{
	if (!array || !n_elements || !new_data || !data_size)
		return -1;
	if (i_resize_array(array, (*n_elements + 1) * data_size, *n_elements * data_size))
		return -1;
	memcpy((char *)*array + *n_elements * data_size, new_data, data_size);
	(*n_elements)++;
	return 0;
}
int BRRCALL
brrheap_prepend(void **const array, brrsz *const n_elements, brrsz data_size, const void *const new_data)
{
	if (!array || !n_elements || !new_data || !data_size)
		return -1;
	if (i_resize_array(array, (*n_elements + 1) * data_size, *n_elements * data_size))
		return -1;
	memmove((char *)*array + data_size, *array, *n_elements * data_size);
	memcpy(*array, new_data, data_size);
	(*n_elements)++;
	return 0;
}

int BRRCALL
brrheap_join(void **const array, brrsz *const n_elements, brrsz data_size, const void *const other, brrsz other_elements)
{
	if (!array || !n_elements || (!other && other_elements) || !data_size)
		return -1;
	else if (!other_elements)
		return 0;
	if (i_resize_array(array, (*n_elements + other_elements) * data_size, *n_elements * data_size))
		return -1;
	memcpy((char *)*array + *n_elements * data_size, other, other_elements * data_size);
	*n_elements += other_elements;
	return 0;
}
int BRRCALL
brrheap_split(const void *const array, brrsz n_elements, brrsz data_size,
    void **const array_1, brrsz *const elements_1,
    void **const array_2, brrsz *const elements_2,
    brrsz split_element)
{
	void *a1 = NULL, *a2 = NULL;
	brrsz e1, e2;
	if (!array ||
		!array_1 || !elements_1 ||
		!array_2 || !elements_2 ||
		!data_size || split_element >= n_elements)
		return -1;
	e1 = split_element;
	e2 = n_elements - e1 - 1;
	if (!(a1 = malloc(e1 * data_size))) {
		return -1;
	} else if (!(a2 = malloc(e2 * data_size))) {
		free(a1);
		return -1;
	}
	memcpy(a1, array, e1 * data_size);
	memcpy(a2, (char *)array + e1 * data_size, e2 * data_size);
	*array_1 = a1;
	*array_2 = a2;
	*elements_1 = e1;
	*elements_2 = e2;
	return 0;
}

int BRRCALL
brrheap_insert(void **const array, brrsz *const n_elements, brrsz data_size, brrsz element, const void *const new_data)
{
	if (!array || !n_elements || !new_data || !data_size || element >= *n_elements)
		return -1;
	if (i_resize_array(array, (*n_elements + 1) * data_size, *n_elements * data_size))
		return -1;
	memmove((char *)*array + (element + 1) * data_size,
		(char *)*array + element * data_size,
	    (*n_elements - element - 1) * data_size);
	memcpy((char *)*array + element * data_size, new_data, data_size);
	(*n_elements)++;
	return 0;
}
int BRRCALL
brrheap_remove(void **const array, brrsz *const n_elements, brrsz data_size, brrsz element)
{
	void *copy = NULL;
	if (!array || !n_elements || !data_size || element >= *n_elements)
		return -1;
	if (!(copy = malloc(data_size)))
		return -1;
	memcpy(copy, (char *)*array + element * data_size, data_size);
	/* In the case of element == *n_elements - 1, it will copy 0 bytes, which is valid */
	memmove((char *)*array + element * data_size,
	    (char *)*array + (element + 1) * data_size,
	    (*n_elements - element - 1) * data_size);
	if (i_resize_array(array, (*n_elements - 1) * data_size, *n_elements * data_size)) {
		memmove((char *)*array + (element + 1) * data_size,
		    (char *)*array + element * data_size,
		    (*n_elements - element - 1) * data_size);
		memcpy((char *)*array + element * data_size, copy, data_size);
		free(copy);
		return -1;
	}
	free(copy);
	(*n_elements)--;
	return 0;
}

static void BRRCALL
i_swap(void *const array, brrsz data_size, brrsz a, brrsz b)
{
	char *const A = (char*)array + a * data_size;
	char *const B = (char*)array + b * data_size;
	for (brrsz i = 0; i < data_size; ++i) {
		char t = A[i];
		A[i] = B[i];
		B[i] = t;
	}
}
int BRRCALL
brrheap_swap(void *const array, brrsz n_elements, brrsz data_size, brrsz a, brrsz b)
{
	if (!array || !data_size || a >= n_elements || b >= n_elements)
		return -1;
	i_swap(array, data_size, a, b);
	return 0;
}

int BRRCALL
brrheap_read(const void *const array, brrsz n_elements, brrsz data_size, brrsz element, void *const destination)
{
	if (!array || !destination || !data_size || element >= n_elements)
		return -1;
	memcpy(destination, (char *)array + element * data_size, data_size);
	return 0;
}
int BRRCALL
brrheap_write(void *const array, brrsz n_elements, brrsz data_size, brrsz element, const void *const source)
{
	if (!array || !source || !data_size || element >= n_elements)
		return -1;
	memcpy((char *)array + element * data_size, source, data_size);
	return 0;
}
int BRRCALL
brrheap_copy(const void *const array, brrsz n_elements, brrsz data_size, brrsz start_element, brrsz n, void *const destination)
{
	if (!array || !destination || !data_size || start_element + n >= n_elements)
		return -1;
	memcpy(destination, (char *)array + start_element * data_size, n * data_size);
	return 0;
}
int BRRCALL
brrheap_trim(void **const array, brrsz *const n_elements, brrsz data_size, brrsz n_remove)
{
	if (!array || !n_elements || !*array || !data_size)
		return -1;
	else if (!n_remove)
		return 0;
	else if (n_remove > *n_elements)
		return -1;
	n_remove = *n_elements - n_remove;
	if (i_resize_array(array, n_remove * data_size, *n_elements * data_size))
		return -1;
	*n_elements = n_remove;
	return 0;
}
void BRRCALL
brrheap_clear(void **const array, brrsz *const n_elements, brrsz data_size, void (*clearer)(void *))
{
	if (array) {
		if (*array) {
			if (clearer && n_elements) {
				for (brrsz i = 0; i < *n_elements; ++i)
					clearer((char *)*array + i * data_size);
			}
			free(*array);
			*array = NULL;
		}
	}
	if (n_elements)
		*n_elements = 0;
}

static int BRRCALL
i_sort(void *const array, brrsz n_elements, brrsz data_size, brrheap_comparer_t compare, void *parameter)
{
	char *a = (char *)array;
	for (brrsz i = 0, ofs = 0; i < n_elements; ++i, ofs += data_size) {
		brrsz idx = i;
		char *min = a + ofs;
		for (brrsz j = i, jofs = ofs; j < n_elements; ++j, jofs += data_size) {
			char *v = a + jofs;
			if (compare(v, min, parameter)) {
				min = v;
				idx = j;
			}
		}
		i_swap(array, data_size, i, idx);
	}
	return 0;
}
int BRRCALL
brrheap_sort(void *const array, brrsz n_elements, brrsz data_size, brrheap_comparer_t compare, void *parameter)
{
	if (!array || !data_size)
		return -1;
	else if (!n_elements || !compare)
		return 0;
	return i_sort(array, n_elements, data_size, compare, parameter);
}

void BRRCALL
brrheap_reverse(void *const array, brrsz n_elements, brrsz data_size)
{
	if (array && data_size) {
		for (brrsz i = 0; i < n_elements / 2; ++i)
			i_swap(array, data_size, i, n_elements - i);
	}
}
void BRRCALL
brrheap_reverse_bytes(void *const array, brrsz total_bytes)
{
	if (array) {
		char *i = (char *)array;
		char *j = (char *)array + total_bytes - 1;
		char *max = (char *)array + total_bytes / 2;
		for (; i < max; ++i, --j) {
			char t = *i;
			*i = *j;
			*j = t;
		}
	}
}
