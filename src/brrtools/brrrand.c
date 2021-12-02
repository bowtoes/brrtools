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

#include "brrtools/brrrand.h"
#include "brrtools/brrtime.h"

#define ROTL(_x,_k) (((_x)<<(_k)) | ((_x)>>(64-(_k))))

/* Written in 2015 by Sebastiano Vigna (vigna@acm.org)
 * Source: https://xorshift.di.unimi.it/splitmix64.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define SPLITMIX64(_seed,_rnd)\
do {\
	(_rnd)[0]=(_seed);\
	for (unsigned char i=1;i<4;++i) {\
		(_rnd)[i]=((_rnd)[i-1]+=0x9e3779b97f4a7c15);\
		(_rnd)[i]=((_rnd)[i]^((_rnd)[i]>>30))*0xbf58476d1ce4e5b9;\
		(_rnd)[i]=((_rnd)[i]^((_rnd)[i]>>27))*0x94d049bb133111eb;\
		(_rnd)[i]= (_rnd)[i]^((_rnd)[i]>>31);\
	}\
} while (0)
/* Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 * Source:  https://prng.di.unimi.it/xoshiro256plus.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define XOSHIRO256(_rnd)\
do {\
	const brru8 _t = (_rnd)[1] << 17;\
	(_rnd)[2]^=(_rnd)[0];\
	(_rnd)[3]^=(_rnd)[1];\
	(_rnd)[1]^=(_rnd)[2];\
	(_rnd)[0]^=(_rnd)[3];\
	(_rnd)[2]^=_t;\
	(_rnd)[3]=ROTL((_rnd)[3],45);\
} while (0)
#define GETRESULT(_rnd) ((_rnd)[0] + (_rnd)[3])

/* Starting value determined from executing splitmix64 on '0' */
static brru8 rnd[4] = {0x0, 0xe220a8397b1dcdaf, 0xa706dd2f4d197e6f, 0x238275bc38fcbe91};
brru8 BRRCALL
brrrand_rand(void)
{
	brru8 res = GETRESULT(rnd);
	XOSHIRO256(rnd);
	return res;
}

static brru8 srnd[4] = {0};
brru8 BRRCALL
brrrand_srand(brru8 seed, brru8 iter)
{
	SPLITMIX64(seed,srnd);
	for (brru8 i = 0; i < iter; ++i)
		XOSHIRO256(srnd);
	return GETRESULT(srnd);
}

#define CPYSEED(_from, _to) (_to[0]=_from[0],_to[1]=_from[1],_to[2]=_from[2],_to[3]=_from[3])
static brru8 trnd[4] = {0};
brru8 BRRCALL
brrrand_trand(brru8 iter)
{
	SPLITMIX64(brrtime_utime(), trnd);
	for (brru8 i = 0; i < iter; ++i)
		XOSHIRO256(srnd);
	return GETRESULT(trnd);
}
