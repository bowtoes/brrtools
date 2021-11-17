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
/* This might qualify as redistribution/modification of ogg's oggpack source,
 * so I've included their license here */
/*
Copyright (c) 2002, Xiph.org Foundation

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

- Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

- Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

- Neither the name of the Xiph.org Foundation nor the names of its
contributors may be used to endorse or promote products derived from
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "brrtools/brrpack.h"

#include <stdlib.h>
#include <string.h>

#define _pack_exp 8
#define _pack_inc (1ULL << (_pack_exp))

#define _pow2less1(_e_) ((1ULL<<(_e_))-1ULL)
#define _pow2next(_n_,_e_) (((_pow2less1(_e_) + (_n_))>>(_e_))<<(_e_))

static const uint32_t _mask[33] = {
	0x00000000,
	0x00000001, 0x00000003, 0x00000007, 0x0000000f,
	0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff,
	0x000001ff, 0x000003ff, 0x000007ff, 0x00000fff,
	0x00001fff, 0x00003fff, 0x00007fff, 0x0000ffff,
	0x0001ffff, 0x0003ffff, 0x0007ffff, 0x000fffff,
	0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
	0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff,
	0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,
};

static const uint8_t _8mask[8] = {
	0x01, 0x03, 0x07, 0x0f,
	0x1f, 0x3f, 0x7f, 0xff,
};

int BRRCALL
brrpack_init(brrpackT *const packer, unsigned char *const buffer, uint64_t n_bytes)
{
	if (!packer)
		return -1;
	memset(packer, 0, sizeof(*packer));
	if (buffer && n_bytes) {
		packer->buffer = buffer;
		packer->storage = n_bytes;
	} else {
		if (!(packer->buffer = malloc(_pack_inc)))
			return -1;
		packer->storage = _pack_inc;
		packer->buffer[0] = 0;
	}
	return 0;
}
void BRRCALL
brrpack_clear(brrpackT *const packer)
{
	if (packer) {
		if (packer->buffer)
			packer->buffer[0] = 0;
		packer->fill = packer->n_bit = 0;
	}
}
void BRRCALL
brrpack_free(brrpackT *const packer)
{
	if (packer) {
		if (packer->buffer)
			free(packer->buffer);
		memset(packer, 0, sizeof(*packer));
	}
}

int BRRCALL
brrpack_resize(brrpackT *const packer, uint64_t n_bytes)
{
	if (!packer)
		return -1;
	if (n_bytes != packer->storage) {
		uint64_t next_storage = _pow2next(n_bytes, _pack_exp);
		unsigned char *tmp = NULL;
		if (!(tmp = realloc(packer->buffer, next_storage)))
			return -1;
		packer->buffer = tmp;
		packer->storage = next_storage;
		if (packer->fill > packer->storage)
			packer->fill = packer->storage;
	}
	return 0;
}

int BRRCALL
brrpack_pack(brrpackT *const packer, uint32_t value, unsigned int n_bits)
{
	unsigned char *position = NULL;
	if (!packer || n_bits > 32)
		return -1;
	if (packer->fill + 4 >= packer->storage) {
		void *tmp = NULL;
		if (packer->storage > UINT64_MAX - _pack_inc)
			return -1;
		if (!(tmp = realloc(packer->buffer, packer->storage + _pack_inc)))
			return -1;
		packer->buffer = tmp;
		packer->storage += _pack_inc;
	}
	position = packer->buffer + packer->fill;

	value &= _mask[n_bits];
	n_bits += packer->n_bit;
	position[0] |= value << packer->n_bit;
	if (n_bits > 7) {
		position[1] = value >> (8 - packer->n_bit);
		if (n_bits > 15) {
			position[2] = value >> (16 - packer->n_bit);
			if (n_bits > 23) {
				position[3] = value >> (24 - packer->n_bit);
				if (n_bits > 31) {
					if (packer->n_bit)
						position[4] = value >> (32 - packer->n_bit);
					else
						position[4] = 0;
				}
			}
		}
	}
	packer->fill += n_bits >> 3;
	packer->n_bit = n_bits & 7;
	return 0;
}
int BRRCALL
brrpack_pack_buffer(brrpackT *const packer, const unsigned char *const buffer, uint64_t n_bits)
{
	unsigned char *position = NULL;
	uint64_t in_bytes, n_bytes;
	uint8_t mask;
	if (!packer)
		return -1;
	mask = _8mask[n_bits & 7];
	n_bits += packer->n_bit;
	n_bytes = n_bits >> 3;
	if (brrpack_resize(packer, packer->storage + n_bytes))
		return -1;
	position = packer->buffer + packer->fill;
	for (uint64_t i = 0; i < n_bytes; ++i, ++position) {
		position[0] |= (buffer[i] & mask) <<      packer->n_bit;
		position[1]  = (buffer[i] & mask) >> (8 - packer->n_bit);
	}
	if (packer->n_bit)
		position[0] = (buffer[n_bytes] & mask) >> (8 - packer->n_bit);
	else
		position[0] = 0;
	packer->fill += n_bits >> 3;
	packer->n_bit = n_bits & 7;
	return 0;
}
int64_t BRRCALL
brrpack_unpack(brrpackT *const unpacker, unsigned int n_bits)
{
	unsigned char *position = NULL;
	uint32_t r = 0;
	int32_t m;
	if (!unpacker || n_bits > 32)
		return -1;
	else if (!n_bits)
		return 0;

	position = unpacker->buffer + unpacker->fill;
	m = _mask[n_bits];
	n_bits += unpacker->n_bit;
	if (unpacker->fill > unpacker->storage - 4) {
		if (unpacker->fill > unpacker->storage - ((n_bits + 7) >> 3))
			return -1;
	}
	r = position[0] >> unpacker->n_bit;
	if (n_bits > 8) {
		r |= position[1] << (8 - unpacker->n_bit);
		if (n_bits > 16) {
			r |= position[2] << (16 - unpacker->n_bit);
			if (n_bits > 24) {
				r |= position[3] << (24 - unpacker->n_bit);
				if (n_bits > 32 && unpacker->n_bit)
					r |= position[4] << (32 - unpacker->n_bit);
			}
		}
	}

	r &= m;
	unpacker->fill += n_bits >> 3;
	unpacker->n_bit = n_bits & 7;
	return r;
}
int64_t BRRCALL
brrpack_transfer(brrpackT *const unpacker, brrpackT *const packer, unsigned int n_bits)
{
	uint32_t r = 0;
	if (-1 == (r = brrpack_unpack(unpacker, n_bits)))
		return r;
	if (brrpack_pack(packer, r, n_bits))
		return -1;
	return r;
}
