/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

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

brru8 BRRCALL
brrrand_rand(void)
{
	/* Starting value is splitmix64 on 0 */
	static brru8 rnd[4] = {0x0, 0xe220a8397b1dcdaf, 0xa706dd2f4d197e6f, 0x238275bc38fcbe91};
	brru8 res = GETRESULT(rnd);
	XOSHIRO256(rnd);
	return res;
}

brru8 BRRCALL
brrrand_srand(brru8 seed, brru8 iter)
{
	static brru8 srnd[4] = {0};
	SPLITMIX64(seed, srnd);
	for (brru8 i = 0; i < iter; ++i)
		XOSHIRO256(srnd);
	return GETRESULT(srnd);
}

brru8 BRRCALL
brrrand_trand(brru8 iter)
{
	return brrrand_srand(brrtime_utime(), iter);
}
