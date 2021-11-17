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

#ifndef BRRPACK_H
#define BRRPACK_H

/* Totally not a copy of oggpack, not at all nooooooo sirree. */
#include <brrtools/brrapi.h>

#include <stdint.h>

typedef struct brrpack {
	uint64_t storage;
	uint64_t fill;

	unsigned char *buffer;
	int8_t n_bit;
} brrpackT;

BRRAPI int BRRCALL brrpack_init(brrpackT *const packer, unsigned char *const buffer, uint64_t n_bytes);
BRRAPI void BRRCALL brrpack_clear(brrpackT *const packer);
BRRAPI void BRRCALL brrpack_free(brrpackT *const packer);

BRRAPI int BRRCALL brrpack_resize(brrpackT *const packer, uint64_t n_bytes);

BRRAPI int BRRCALL brrpack_pack(brrpackT *const packer, uint32_t value, unsigned int n_bits);
BRRAPI int BRRCALL brrpack_pack_buffer(brrpackT *const packer, const unsigned char *const buffer, uint64_t n_bits);
BRRAPI int64_t BRRCALL brrpack_unpack(brrpackT *const unpacker, unsigned int n_bits);
BRRAPI int64_t BRRCALL brrpack_transfer(brrpackT *const unpacker, brrpackT *const packer, unsigned int n_bits);

#endif /* BRRPACK_H */
