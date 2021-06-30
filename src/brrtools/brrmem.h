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

#ifndef BRRMEM_H
#define BRRMEM_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

/* Swaps the memory at locations 'data0' and 'data1'.
 * If 'data0' or 'data1' is NULL or 'data_size' == 0, nothing is done.
 * */
BRRAPI void BRRCALL brrmem_swap(void *const data0, void *const data1, brrsz data_size);

/* Searches 'data' for the first occurrence of 'key' at or after 'offset' and
 * returns the found offset, up-to-and-including 'data_size-1'.
 * If 'data' is NULL or size == 0, 'offset' is returned.
 * If 'key' is not found, 'data_size' is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_next(const void *const data, brrsz data_size,
    brrby key, brrsz offset);
/* Searches 'data' for the first occurrence of 'key' at or before 'offset' and
 * returns the found offset, UTAI 'data_size - 1'.
 * If 'data' is NULL or size == 0, 'offset' is returned.
 * If 'key' is not found, 'data_size' is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_previous(const void *const data, brrsz data_size,
    brrby key, brrsz offset);

/* Allocates and creates a copy of 'data' and returns a pointer to the new memory.
 * If 'data' is NULL or size == 0, nothing is done and NULL is returned.
 * If an error occurs, NULL is returned.
 * */
BRRAPI void *BRRCALL brrmem_duplicate(const void *const data, brrsz data_size);
/* Copies at most 'destination_length' bytes from 'source', of 'source_length' bytes,
 * into 'destination' and returns the number of bytes copied.
 * If either 'source' or 'destination' is NULL or size == 0,
 * nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_copy(const void *const source, brrsz source_length,
    void *const destination, brrsz destination_length);

/* Searches 'data' for the first occurrence of 'key' at or after 'offset' and
 * returns the found offset, up-to-and-including 'data_size - 1'.
 * If 'data' or 'key' is NULL or size == 0, 'offset' is returned.
 * If 'key' is not found in 'data', 'data_size' is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_search(const void *const data, brrsz data_size,
    const void *const key, brrsz key_size, brrsz offset);
/* Searches 'data' for the first occurrence of 'key' at or before 'offset' and
 * returns the found offset, UTAI 'data_size - 1'.
 * If 'data' or 'key' is NULL or size == 0, 'offset' is returned.
 * If 'key' is not found in 'data', 'data_size' is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_search_reverse(const void *const data, brrsz data_size,
    const void *const key, brrsz key_size, brrsz offset);

/* Reverses 'data_size' bytes of 'data' in-place and returns 'data'.
 * If 'data' is NULL or size == 0, nothing is done and NULL is returned.
 * */
BRRAPI void *BRRCALL brrmem_static_reverse(void *const data, brrsz data_size);
/* Creates a reversed copy of 'data' and returns the copy.
 * If 'data' is NULL or size == 0, nothing is done and NULL is returned.
 * If an error occurs, nothing is done and NULL is returned.
 * */
BRRAPI void *BRRCALL brrmem_reverse(const void *const data, brrsz data_size);
/* Reverses 'block_count' blocks of 'block_size' size bytes in data in-place and returns data.
 * If 'data' is NULL, nothing is done and NULL is returned.
 * If 'block_size' or 'block_count' is 0, nothing is done and 'data' is returned.
 * */
BRRAPI void *BRRCALL brrmem_static_reverse_blocks(void *const data, brrsz block_size, brrsz block_count);
/* Returns a copy of 'data', where blocks of 'block_size' are in reverse order.
 * If 'data' is NULL or size == 0 or block_size == 0, nothing is done and NULL is returned.
 * If an error occurs, nothing is done and NULL is returned.
 * */
BRRAPI void *BRRCALL brrmem_reverse_blocks(const void *const data, brrsz block_size, brrsz block_count);

/* Concatenates the two memory regions 'data_a' and 'data_b' together into new
 * memory and returns a pointer to the new memory.
 * If 'data_a' or 'data_b' are invalid (NULL or size == 0), nothing is done and
 * NULL is returned.
 * If an error occurs, nothing is done and NULL is returned.
 * If an error doesn't occur and 'new_size' is non-NULL, the size of the new
 * memory region will be stored in 'new_size'.
 * */
BRRAPI void *BRRCALL brrmem_join(const void *const data_a, brrsz data_a_size,
    const void *const data_b, brrsz data_b_size, brrsz *const new_size);
/* Appends 'suffix' onto the end of 'data' and returns a pointer to 'data'.
 * 'data' is realloced to fit 'suffix'.
 * If 'data' is NULL or size == 0, nothing is done and NULL is returned.
 * If 'suffix' is NULL or size == 0, nothing is done and 'data' is returned.
 * If an error occurs, nothing is done and 'data' is returned.
 * If an error doesn't occur and 'new_size' is non-NULL, the total size of 'data'
 * after reallocation will be stored in 'new_size'.
 * */
BRRAPI void *BRRCALL brrmem_append(void *data, brrsz data_size,
    const void *const suffix, brrsz suffix_size, brrsz *const new_size);
/* Prepends 'prefix' to the start of 'data' and returns a pointer to 'data'.
 * 'data' is realloced to fit 'prefix'.
 * If 'data' is NULL or size == 0, nothing is done and NULL is returned.
 * If 'prefix' is NULL or size == 0, nothing is done and 'data' is returned.
 * If an error occurs, nothing is done and 'data' is returned.
 * If an error doesn't occur and 'new_size' is non-NULL, the total size of 'data'
 * after reallocation will be stored in 'new_size'.
 * */
BRRAPI void *BRRCALL brrmem_prepend(void *data, brrsz data_size,
    const void *const prefix, brrsz prefix_size, brrsz *const new_size);

/* Copies a subsection of 'data' from 'start' up 'end_inclusive'
 * into 'destination' and returns the length of the copied subsection.
 * If 'data' or 'destination' is NULL or size == 0, nothing is done and 0 is returned.
 * If 'start > end', the subsection is copied in reverse.
 * If 'start == end', nothing is done and 0 is returned.
 * */
BRRAPI brrsz BRRCALL brrmem_slice(const void *const data, brrsz data_size,
    void *const destination, brrsz start, brrsz end_inclusive);

#define _brrmem_swap(_d0_, _d1_, _ds_) do { \
    brrby *_b0_ = (brrby *)(_d0_); \
    brrby *_b1_ = (brrby *)(_d1_); \
    brrsz _s_ = (_ds_); \
    for (brrsz _i_=0;_i_<_s_;++_i_) { \
        brrby _ = _b0_[_i_]; \
        _b0_[_i_]=_b1_[_i_]; \
        _b1_[_i_]=_; \
    } \
} while (0)

#define BRRMEM_BLOCK(_d_, _bs_, _bi_) (((brrby *)(_d_))+(_bs_)*(_bi_))

BRRCPPEND

#endif /* BRRMEM_H */
