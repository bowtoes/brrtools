/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrtools_brrarray
#define brrtools_brrarray

#ifdef __cplusplus
extern "C" {
#endif

/* brrdata - Heap-allocated array modification for fixed-sized elements */

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

typedef void (*brrarray_freer_t)(void*);

typedef struct brrarray {
	void *data; /* Raw pointer to the array data */
	brrsz count; /* Length of the array in elements. */
	brrsz size; /* Size, in bytes, of an individual array element. */
	brrsz _bytes;
} brrarray_t;

/* Returns 0 if 'array' is valid, or -1 otherwise.
 * 'array' is invalid if it is NULL or 'array->size' == 0.
 * */
static BRR_inline int BRRCALL
brrarray_check(const brrarray_t *const array)
{
	if (!array || !array->size)
		return -1;
	return 0;
}

/* Initializes 'array' with 'n_elements' of 'elements', each 'element_size' bytes.
 * Returns 0 on success.
 * If 'elements' is NULL or 'n_elements' is 0, 'array.data' is not allocated.
 * If 'array' is NULL or 'element_size' is 0 or an error occurs, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_new(brrarray_t *const array, const void *const elements, brrsz element_size, brrsz n_elements);
/* Initializes 'array' from 'data' of size 'data_size' bytes, with elements of size 'element_size' bytes.
 * Returns 0 on success.
 * If 'data' is NULL or 'data_size' is 0, 'array.data' is not allocated.
 * If 'array' is NULL or 'element_size' is 0 or an error occurs, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_new_data(brrarray_t *const array, const void *const data, brrsz data_size, brrsz element_size);
/* Frees the data associated with 'array', calling 'freer' on each element before removal if 'freer' isn't
 * NULL. */
BRRAPI void BRRCALL
brrarray_free(brrarray_t *const array, brrarray_freer_t freer);

static BRR_inline brrarray_t BRRCALL
brrarray_init(void *const elements, brrsz element_size, brrsz n_elements)
{
	return (brrarray_t){
		.data = elements,
		.count = n_elements,
		.size = element_size,
		._bytes = element_size * n_elements,
	};
}
static BRR_inline brrarray_t BRRCALL
brrarray_init_data(void *const data, brrsz data_size, brrsz element_size)
{
	return brrarray_init(data, element_size, data_size / element_size);
}

/* Resizes 'array' to fit 'new_count' of elements.
 * Returns 0 on success.
 * If the array size is increased, the new data is initialized to 0.
 * If an error occurs or 'array' is invalid, 'array' is left unaffected and -1 is
 * returned.
 * */
BRRAPI int BRRCALL
brrarray_resize(brrarray_t *const array, brrsz new_count);
/* Appends 'elements' of 'count' elements to the end of 'array'.
 * Returns 0 on success.
 * If 'elements' is NULL or 'count' is 0, 'array' is unaffected and 0 is returned.
 * If an error occurs or 'array' is invalid, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_append(brrarray_t *const array, const void *const elements, brrsz count);
/* Prepends 'elements' of 'count' elements to the start of 'array'.
 * Returns 0 on success.
 * If 'elements' is NULL or 'count' is 0, 'array' is unaffected and 0 is returned.
 * If an error occurs or 'array' is invalid, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_prepend(brrarray_t *const array, const void *const elements, brrsz count);
/* Splits 'array' into 'a' and 'b', where the elements before and including 'split' are copied into 'a', and
 * the elements after are copied into 'b'.
 * Returns 0 on success.
 * If 'a' and 'b' are NULL, nothing is done and 0 is returned.
 * If one of 'a' or 'b' is NULL, nothing is done and -1 is returned.
 * If 'split' is greater than 'array.count - 1', nothing is done and -1 is returned, even if 'array' is empty.
 * If 'split' is 0, 'a' will consist of only the first element of 'array', and b will consist of the rest.
 * If an error occurs or 'array' is invalid, 'a' & 'b' are unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_split(const brrarray_t *const array, brrsz split, brrarray_t *const a, brrarray_t *const b);
/* Partitions 'array' into 'a' and 'b', where 'a' contains the elements up to and including 'part', and b
 * contains the rest; 'a' and 'b' are not copies, they point to data in 'array'.
 * Returns 0 on success.
 * If 'a' and 'b' are NULL, nothing is done and 0 is returned.
 * If one of 'a' or 'b' is NULL, the one which is non-NULL will be identical to 'array'.
 * If 'part' is greater than or equal 'array.count', nothing is done and -1 is returned, even if 'array' is
 * empty.
 * If 'part' is 0, 'a' will consist of only the first element of 'array', and b will consist of the rest.
 * If an error occurs or 'array' is NULL, 'a' & 'b' are unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_partition(const brrarray_t *const array, brrsz part, brrarray_t *const a, brrarray_t *const b);
/* Inserts 'new' into 'array' before 'start', shifting 'start' and all elements past it forward.
 * Returns 0 on success.
 * If 'new' is an empty array, nothing is done and 0 is returned.
 * If 'start' is greater than 'array.count', 'array' is unaffected and -1 is returned.
 * If an error occurs or 'array' is invalid or 'new' is invalid or 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_insert(brrarray_t *const array, const brrarray_t new, brrsz start);
/* Removes 'count' elements from 'array', starting at 'start'.
 * Returns 0 on success.
 * If 'freer' isn't NULL, it is called on each element before it's removal.
 * If 'count' is 0, nothing is done and 0 is returned.
 * If 'count' is greater than 'array.count - start', nothing is done and -1 is returned.
 * If an error occurs or 'array' is invalid, 'array' is unaffected and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_remove(brrarray_t *const array, brrsz count, brrsz start, brrarray_freer_t freer);
/* Swaps 'count' elements of 'array' at 'region_a' with those at 'region_b'.
 * Returns 0 on success.
 * If 'region_a' equals 'region_b' or 'count' is 0, nothing is done and 0 is returned.
 * If 'region_a' or 'region_b' or 'region_a + count' or 'region_b + count' are greater than 'array.count',
 * 'array' is unaffected and -1 is returned.
 * Should 'region_a' overlap 'region_b', only the areas outside the overlap will be swapped.
 * If 'array' is invalid, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_swap(brrarray_t *const array, brrsz count, brrsz region_a, brrsz region_b);
/* Copies data of 'element' from 'array' into 'dst'.
 * Returns 0 on success.
 * If 'dst' is NULL, nothing is done and 0 is returned.
 * If 'element' is out of bounds, nothing is done and -1 is returned.
 * If 'array' is invalid, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_read(const brrarray_t *const array, brrsz element, void *const dst);
/* Writes data of 'src' to 'element' in 'array'.
 * Returns 0 on success.
 * If 'src' is NULL, nothing is done and 0 is returned.
 * If 'element' is out of bounds, nothing is done and -1 is returned.
 * If 'array' is invalid, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_write(brrarray_t *const array, brrsz element, const void *const src);

typedef int (*brrdata_comparer_t)(const void *, const void *, void*);

/* Sorts elements of 'array', using 'compare' to determine the relative order of two elements.
 * 'compare' is given 'parameter'.
 * Returns 0 on success.
 * If 'array' has less than 2 elements, there is nothing to sort and 0 is returned.
 * If 'array' is invalid or 'compare' is NULL, nothing is done and -1 is returned.
 * */
BRRAPI int BRRCALL
brrarray_sort(brrarray_t *const array, brrdata_comparer_t compare, void *parameter);

/* Reverses 'count' elements of 'array' starting at 'start'.
 * If 'count' extends beyond 'array.count', elements of to the end of 'array' are reversed.
 * If 'start' is out of bounds, 'array' is unaffected.
 * If 'array' is invalid, 'array' is unaffected.
 * */
BRRAPI void BRRCALL
brrarray_reverse(brrarray_t *const array, brrsz start, brrsz count);
/* Reverses 'length' bytes of 'array' starting at byte 'start'.
 * If 'length' extends beyond 'array.data', bytes of to the end of 'array.data' are reversed.
 * If 'start' is out of bounds, 'array' is unaffected.
 * If 'array' is invalid, 'array' is unaffected.
 * */
BRRAPI void BRRCALL
brrarray_reverse_bytes(brrarray_t *const array, brrsz start, brrsz length);

#ifdef __cplusplus
}
#endif

#endif /* brrtools_brrarray */
