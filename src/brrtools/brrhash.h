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

#ifndef BRRHASH_H
#define BRRHASH_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Computes a FNV-1a hash on 'data' of length 'data_size'.
 * If 'data' is NULL or 'data_size' is 0, nothing is done and a default value
 * is returned.
 * */
BRRAPI brru8 BRRCALL brrhash_fnv1a(const void *const data, brrsz data_size);

#endif /* BRRHASH_H */
