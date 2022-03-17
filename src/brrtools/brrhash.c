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

#include "brrtools/brrhash.h"

brru8 BRRCALL
brrhash_fnv1a(const void *const data, brrsz data_size)
{
	/* These values taken from the wikipedia page on FNV hash */
	static const brru8 offset = 0xcbf29ce484222325;
	static const brru8 prime = 0x100000001b3;
	const char *const max = (char *)data + data_size;
	union { brru8 v; brru1 a[8]; } hash = {.v = offset};
	if (data && data_size) {
		for (char *i = (char *)data; i < max; ++i) {
			hash.a[4] ^= *i;
			hash.v *= prime;
		}
	}
	return hash.v;
}
