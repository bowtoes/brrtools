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

#ifndef BRRHEAP_H
#define BRRHEAP_H

/* brrheap - Heap-allocated array modification for fixed-sized elements */

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Resizes 'array' of size 'n_elements' to size of 'new_elements', each element
 * of 'data_size' bytes.
 * Returns 0 on success.
 * If the array size is increased, the new data is initialized to 0.
 * If an error occurs or 'array' is NULL, 'array' is left unaffected and -1 is
 * returned.
 * */
BRRAPI int BRRCALL
brrheap_resize(void **const array, brrsz n_elements, brrsz data_size, brrsz new_elements);
/* Appends 'new_data' of 'data_size' bytes to the end of 'array' of size
 * 'n_elements', incrementing 'n_elements'.
 * Returns 0 on success.
 * If an error occurs or 'array', 'n_elements' or 'new_data' are NULL or
 * 'data_size' is 0, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_append(void **const array, brrsz *const n_elements, brrsz data_size, const void *const new_data);
/* Prepends 'new_data' of 'data_size' bytes to the start of 'array' of size
 * 'n_elements', incrementing 'n_elements'.
 * Returns 0 on success.
 * If an error occurs or 'array', 'n_elemnts' or 'new_data' are NULL or
 * 'data_size' is 0, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_prepend(void **const array, brrsz *const n_elements, brrsz data_size, const void *const new_data);
/* Joins 'other' of size 'other_elements' to the end of 'array' of size
 * n_elements', incrementing 'n_elements' appropriately, each element of
 * 'data_size' bytes.
 * Returns 0 on success.
 * If an error occurs or 'array', 'n_elements', or 'other' are NULL or
 * 'data_size' is 0, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_join(void **const array, brrsz *const n_elements, brrsz data_size, const void *const other, brrsz other_elements);
/* Splits 'array' of size 'n_elements' into 'array_1' of size 'split_element'
 * and 'array_2' of size 'n_elements - split_element', each element of
 * 'data_size' bytes, setting 'elements_1' and 'elements_2' appropriately.
 * Returns 0 on success.
 * If an error occurs or 'array', 'array_1', 'elements_1', 'array_2', or
 * 'elements_2' are NULL or 'data_size' is 0 or 'split_element' is out of
 * range, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_split(const void *const array, brrsz n_elements, brrsz data_size,
    void **const array_1, brrsz *const elements_1,
    void **const array_2, brrsz *const elements_2,
    brrsz split_element);
/* Inserts 'new_data' of 'data_size' bytes into 'array' of size 'n_elements' at
 * position 'element', shifting all elements past it forward and incrementing
 * 'n_elements'.
 * Returns 0 on success.
 * If an error occurs or 'array', 'n_elements', or 'new_data' are NULL or
 * 'data_size' is 0 or 'element' is out of range, 'array' is unaffected and -1
 * is returned.
 * */
BRRAPI int BRRCALL
brrheap_insert(void **const array, brrsz *const n_elements, brrsz data_size, brrsz element, const void *const new_data);
/* Removes 'element' of 'data_size' bytes from 'array' of size 'n_elements',
 * shifting all elements past it backward and decrementing 'n_elements'.
 * Returns 0 on success.
 * If an error occurs or 'array' or 'n_elements' are NULL or 'data_size' is 0
 * or element is out of range, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_remove(void **const array, brrsz *const n_elements, brrsz data_size, brrsz element);
BRRAPI int BRRCALL
brrheap_swap(void *const array, brrsz n_elements, brrsz data_size, brrsz a, brrsz b);
/* Copies 'element' of 'data_size' bytes from 'array' of 'n_elements' into
 * 'destination'.
 * Returns 0 on success.
 * If 'array' or 'destination' are NULL or 'data_size' is 0 or 'element' is out
 * of range, 'destination' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_read(const void *const array, brrsz n_elements, brrsz data_size, brrsz element, void *const destination);
/* Copies 'source' of 'data_size' bytes into 'element' of 'array' of size
 * 'n_elements'.
 * Returns 0 on success.
 * If 'array' or 'source' are NULL or 'data_size' is 0 or 'element' is out of
 * range, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_write(void *const array, brrsz n_elements, brrsz data_size, brrsz element, const void *const source);
/* Copies 'n' elements of 'data_size' bytes from 'array' of size 'n_elements' at
 * 'start_element' into 'destination'.
 * Returns 0 on success.
 * If 'array' or 'destination' are NULL or 'data_size' is 0 or 'start_element + n'
 * is out of range, 'destination' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_copy(const void *const array, brrsz n_elements, brrsz data_size, brrsz start_element, brrsz n, void *const destination);
/* Removes the last 'n_remove' elements of 'data_size' bytes from 'array' of size 'n_elements'
 * and decrements 'n_elements' appropriately.
 * Returns 0 on success.
 * If an error occurs or 'array', 'n_elements', or '*array' are NULL or 'data_size' is 0,
 * 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrheap_trim(void **const array, brrsz *const n_elements, brrsz data_size, brrsz n_remove);

/* Calls 'clearer' on each element of 'data_size' bytes of 'array' of size 'n_elements'
 * and frees 'array'.
 * Does not call 'clearer' if it is NULL.
 * If 'array' or 'n_elements' are NULL, nothing is done.
 * */
BRRAPI void BRRCALL
brrheap_clear(void **const array, brrsz *const n_elements, brrsz data_size, void (*clearer)(void *));

typedef int (*brrheap_comparer_t)(const void *, const void *, void*);
BRRAPI int BRRCALL
brrheap_sort(void *const array, brrsz n_elements, brrsz data_size, brrheap_comparer_t compare, void *parameter);

/* Reverses 'n_elements' elements of 'data_size' bytes in 'array'.
 * If 'array' is NULL or 'data_size' is 0, 'array' is unaffected.
 * */
BRRAPI void BRRCALL
brrheap_reverse(void *const array, brrsz n_elements, brrsz data_size);
/* Reverses 'total_bytes' bytes of array.
 * If 'array' is NULL, 'array' is unaffected.
 * */
BRRAPI void BRRCALL
brrheap_reverse_bytes(void *const array, brrsz total_bytes);

#endif /* BRRHEAP_H */
