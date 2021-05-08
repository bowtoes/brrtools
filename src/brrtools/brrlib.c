#include "brrtools/brrlib.h"

#include "brrtools/brrapi.h"
#include "brrtools/brrplatform.h"

#include <stdlib.h>
#if defined(BRRPLATFORMTYPE_WINDOWS)
# include <windows.h>
# include <malloc.h>
#else
# include <sys/time.h>
#endif

#include "brrtools/brrtypes.h"

brrb1 BRRCALL
brrlib_alloc(void **current, brrsz size, brrb1 zero)
{
	brrb1 res = false;
	if (current) {
		void *cur = *current;
		if (!size) {
			if (cur)
				free(cur);
			cur = NULL;
			res = true;
		} else if (zero) {
			void *t = calloc(1, size);
			if (t) {
				if (cur)
					free(cur);
				cur = t;
				res = true;
			}
		} else if (!cur) {
			void *t = malloc(size);
			if (t) {
				cur = t;
				res = true;
			}
		} else {
			void *t = realloc(cur, size);
			if (t) {
				cur = t;
				res = true;
			}
		}
		*current = cur;
	}
	return res;
}

brrsz BRRCALL
brrlib_ndigits(brrb1 is_signed, brru8 number)
{
	brrsz c = 1;
	if (is_signed && (brrs8)number < 0)
		number = (brru8)(-(brrs8)number);
	if (number == 0)
		return c;
	while (number/=10) c++; /* teehee */
	return c;
}

brrs8 BRRCALL
brrlib_wrap(brrs8 number, brru8 wrap, brrs8 offset)
{
	if (!wrap)
		return 0;
	if (number >= 0) {
		return (brrs8)((brru8)number % wrap);
	} else {
		return (brrs8)(wrap - 1 - ((brru8)(-(number + 1)) % wrap)) + offset;
	}
}

brru8 BRRCALL
brrlib_mutime(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	FILETIME time;
	ULARGE_INTEGER lrg;
	GetSystemTimeAsFileTime(&time);
	lrg.u.LowPart = time.dwLowDateTime;
	lrg.u.HighPart = time.dwHighDateTime;
	return lrg.QuadPart;
#else
	// TODO gettimeofday deprecated.
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#endif
}

#define ROTL(x,k) (((x)<<(k)) | ((x)>>(64-(k))))
/* Written in 2015 by Sebastiano Vigna (vigna@acm.org)
 * Source: https://xorshift.di.unimi.it/splitmix64.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define SPLITMIX64(_sd,_res) do {\
	_res[0]=_sd;\
	for (unsigned char i=1;i<4;++i) {\
		_res[i]=(_res[i-1]+0x9e3779b97f4a7c15);\
		_res[i]=(_res[i]^(_res[i]>>30))*0xbf58476d1ce4e5b9;\
		_res[i]=(_res[i]^(_res[i]>>27))*0x94d049bb133111eb;\
		_res[i]= _res[i]^(_res[i]>>31);\
	}\
} while (0)
/* Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 * Source:  https://prng.di.unimi.it/xoshiro256plus.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define XOSHIRO256(_sd,_res) do {\
	const brru8 t = _sd[1] << 17;\
	_res=_sd[0]+_sd[3];\
	_sd[2]^=_sd[0];\
	_sd[3]^=_sd[1];\
	_sd[1]^=_sd[2];\
	_sd[0]^=_sd[3];\
	_sd[2]^=t;\
	_sd[3]=ROTL(_sd[3],45);\
} while (0)
/* Starting value determined from executing splitmix64 on '0' */
static brru8 randseed[4] = {0x0, 0xe220a8397b1dcdaf, 0xa706dd2f4d197e6f, 0x238275bc38fcbe91};
brru8 BRRCALL
brrlib_rand(void)
{
	static brru8 res;
	XOSHIRO256(randseed, res);
	return res;
}

brru8 BRRCALL
brrlib_srand(brru8 seed)
{
	SPLITMIX64(seed,randseed);
	return randseed[0] + randseed[3];
}

#define CPYSEED(_from, _to) (_to[0]=_from[0],_to[1]=_from[1],_to[2]=_from[2],_to[3]=_from[3])
brru8 BRRCALL
brrlib_trand(void)
{
	static brru8 preseed[4];
	static brru8 res;
	CPYSEED(randseed, preseed);
	res = brrlib_srand(brrlib_mutime());
	CPYSEED(preseed, randseed);
	return res;
}
