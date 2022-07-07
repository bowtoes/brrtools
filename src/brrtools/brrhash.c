/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrhash.h"

brru8 BRRCALL
brrhash_fnv1a(const void *const data, brrsz data_size)
{
	/* These values taken from the wikipedia page on FNV hash */
	#define offset 0xcbf29ce484222325ULL
	#define prime 0x100000001b3ULL
	union { brru8 v; brru1 a[8]; } hash = {.v = offset};
	if (data) {
		for (char *i = (char *)data; i < (char*)data+data_size; ++i) {
			hash.a[4] ^= *i;
			hash.v *= prime;
		}
	}
	return hash.v;
}
