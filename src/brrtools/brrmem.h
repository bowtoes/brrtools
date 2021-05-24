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

/* Searches 'data' for the first occurrence of 'key' at or after 'offset' and
 * returns the found offset, up to and including 'data_size-1'.
 * If 'key' is not found in 'data', 'data_size' is returned.
 * If 'data' is invalid (NULL or size == 0), 'offset' is returned. */
BRRAPI brrsz BRRCALL brrmem_next(const void *const data, brrsz data_size, brrby key, brrsz offset);
/* Searches 'data' for the first occurrence of 'key' at or before 'offset' and
 * returns the found offset.
 * If 'key' is not found in 'data', 'data_size' is returned.
 * If 'data' is invalid (NULL or size == 0), 'offset' is returned. */
BRRAPI brrsz BRRCALL brrmem_prev(const void *const data, brrsz data_size, brrby key, brrsz offset);
//BRRAPI void *BRRCALL brrmem_after(const void *const data, brrsz data_size, brrsz start, brrsz *new_size);
/* Allocates and creates a copy of 'data' and returns a pointer to the new memory.
 * If 'data' is invalid (NULL or size == 0), nothing is done and NULL is returned.
 * If an error occurs, 'NULL' is returned. */
BRRAPI void *BRRCALL brrmem_dup(const void *const data, brrsz data_size);
/* Concatenates the two memory regions 'data_a' and 'data_b' together into new
 * memory and returns a pointer to the newly allocated region.
 * The size of the new region is optionally returned in 'new_length'.
 * If 'data_a' and 'data_b' are invalid (NULL or size == 0), nothing is done and
 * NULL is returned. */
BRRAPI void *BRRCALL brrmem_join(const void *const data_a, brrsz data_a_size,
    const void *const data_b, brrsz data_b_size, brrsz *const new_length);
/* Appends 'suffix' onto the end of 'data' and returns a pointer to 'data'.
 * 'data' is realloced to fit 'suffix'.
 * Length of realloced 'data' is optionally returned in 'new_length'.
 * If 'data' is invalid (NULL or size == 0), nothing is done and NULL is returned.
 * If 'suffix' is invalid, nothing is done and 'data' is returned.
 * If an error occurs, nothing is done and 'data' is returned. */
BRRAPI void *BRRCALL brrmem_append(void *data, brrsz data_size,
    const void *const suffix, brrsz suffix_size, brrsz *const new_length);
/* Prepends 'prefix' to the start of 'data' and returns a pointer to 'data'.
 * 'data' is realloced to fit 'prefix'.
 * Length of realloced 'data' is optionally returned in 'new_length'.
 * If 'data' is invalid (NULL or size == 0), nothing is done and NULL is returned.
 * If 'prefix' is invalid, nothing is done and 'data' is returned.
 * If an error occurs, nothing is done and 'data' is returned. */
BRRAPI void *BRRCALL brrmem_prepend(void *data, brrsz data_size,
    const void *const prefix, brrsz prefix_size, brrsz *const new_length);

/* Reverse 'data' in-place and returns 'data'.
 * If 'data' is invalid (NULL or size == 0), nothing is done and NULL is returned. */
BRRAPI void *BRRCALL brrmem_static_reverse(void *const data, brrsz data_size);
/* Creates a reversed copy of 'data' and returns a pointer to the copy.
 * If 'data' is invalid (NULL or size == 0), nothing is done and NULL is returned.
 * If an error occurs, nothing is done and NULL is returned. */
BRRAPI void *BRRCALL brrmem_reverse(const void *const data, brrsz data_size);

// Splits 'data' on each occurrence of 'separator', starting at 'offset', into a
//  list of heap-allocated blocks of memory, returning a pointer to the first
//  element of the list.
// Each split is at most 'max_split + 1' bytes in length, unless 'max_split' is
//  0, in which case there is no maximum.
// If 'ignore_consecutive' is true, then consecutive occurrences of 'separator'
//  are not included in the output.
//  occurrences of 'separator' are included in output.
// The number of stgT's in the returned list is stored in 'count'.
//BRRAPI void **BRRCALL brrmem_split(const void *const data,

BRRCPPEND

#endif /* BRRMEM_H */
