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

#include "brrtools/brrplatform.h"

#include <stdio.h>
#if defined(BRRPLATFORMTYPE_WINDOWS)
# include <windows.h>
# include <malloc.h>
# include <conio.h>
#else
# include <termios.h>
# include <sys/select.h>
# if defined(BRRPLATFORM_AIX)
/* https://cr.yp.to/docs/unixport.html */
#  include <time.h>
# endif /* BRRPLATFORM_AIX */
# include <sys/time.h>
#endif
#include <stdlib.h>

#include "brrtools/brrlib.h"
#include "brrtools/brrmem.h"

int BRRCALL
brrlib_pause(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	getch();
#elif defined(BRRPLATFORMTYPE_UNIX)
	struct termios new, before;
	/* https://stackoverflow.com/a/18806671/13528679 */
	tcgetattr(0, &before);       /* get current terminal attirbutes; 0 is the file descriptor for stdin */
	new = before;
	new.c_lflag &= ~ICANON;      /* disable canonical mode */
	new.c_lflag &= ~ECHO;        /* don't echo */
	new.c_cc[VMIN] = 1;          /* wait until at least one keystroke available */
	new.c_cc[VTIME] = 0;         /* no timeout */
	tcsetattr(0, TCSANOW, &new); /* set immediately */
	getc(stdin);
	tcsetattr(0, TCSANOW, &before);
#else
	return 0;
#endif
	return 1;
}
int BRRCALL
brrlib_clear(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	/* https://cboard.cprogramming.com/cplusplus-programming/93806-conio-h-functions-mingw-post672795.html#post672795 */
	COORD a = {0,0};
	DWORD nwrite;
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07, 2000, a, &nwrite);
#elif defined(BRRPLATFORMTYPE_UNIX)
	printf("\x1b[1;1H\x1b[2J");
#else
	return 0;
#endif
	return 1;
}

brru8 BRRCALL
brrlib_utime(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	FILETIME time;
	ULARGE_INTEGER lrg;
	GetSystemTimeAsFileTime(&time);
	lrg.u.LowPart = time.dwLowDateTime;
	lrg.u.HighPart = time.dwHighDateTime;
	/* Convert windows FILETIME (100ns precision) to usec precision */
	lrg.QuadPart /= 10;
	/* Offset windows epoch to be UNIX epoch (not accurate) */
	return lrg.QuadPart - 11644473838000000;
#else
	/* TODO gettimeofday deprecated. */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#endif /* BRRPLATFORMTYPE_WINDOWS */
}
void BRRCALL
brrlib_usleep(brru8 usec)
{
	if (usec) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		HANDLE timer;
		LARGE_INTEGER ft;
		/* Convert usec precision to windows FILETIME (100ns precision). */
		ft.QuadPart = -(10 * (__int64)usec);
		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
#else
		struct timeval tv = {usec / 1000000, usec % 1000000};
		select(0, NULL, NULL, NULL, &tv);
#endif
	}
}

int BRRCALL
brrlib_alloc(void **current, brrsz size, int zero)
{
	int res = 0;
	if (current) {
		void *cur = *current;
		if (!size) {
			if (cur)
				free(cur);
			cur = NULL;
			res = 1;
		} else if (zero) {
			void *t = calloc(1, size);
			if (t) {
				if (cur)
					free(cur);
				cur = t;
				res = 1;
			}
		} else if (!cur) {
			void *t = malloc(size);
			if (t) {
				cur = t;
				res = 1;
			}
		} else {
			void *t = realloc(cur, size);
			if (t) {
				cur = t;
				res = 1;
			}
		}
		*current = cur;
	}
	return res;
}

brrsz BRRCALL
brrlib_ndigits(brru8 number, int is_signed, brru1 base)
{
	brrsz c = 1;
	if (base < 2)
		return 0;
	else if (!number)
		return 1;
	else if (is_signed && (brrs8)number < 0)
		number = -(brrs8)number;
	while (number/=base)
		c++; /* teehee */
	return c;
}
brrs8 BRRCALL
brrlib_wrap(brrs8 number, brru8 wrap, brrs8 offset)
{
	if (!wrap)
		return 0;
	else if (number >= 0)
		return (brrs8)((brru8)number % wrap);
	else
		return (brrs8)(wrap - 1 - ((brru8)(-(number + 1)) % wrap)) + offset;
}

/* Written in 2015 by Sebastiano Vigna (vigna@acm.org)
 * Source: https://xorshift.di.unimi.it/splitmix64.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define SPLITMIX64(_sd,_res) do {\
	(_res)[0]=(_sd);\
	for (unsigned char i=1;i<4;++i) {\
		(_res)[i]=((_res)[i-1]+=0x9e3779b97f4a7c15);\
		(_res)[i]=((_res)[i]^((_res)[i]>>30))*0xbf58476d1ce4e5b9;\
		(_res)[i]=((_res)[i]^((_res)[i]>>27))*0x94d049bb133111eb;\
		(_res)[i]= (_res)[i]^((_res)[i]>>31);\
	}\
} while (0)
#define ROTL(x,k) (((x)<<(k)) | ((x)>>(64-(k))))
/* Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
 * Source:  https://prng.di.unimi.it/xoshiro256plus.c
 * Wayback: https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define XOSHIRO256(_sd,_res) do {\
	const brru8 _t = (_sd)[1] << 17;\
	_res=(_sd)[0]+(_sd)[3];\
	(_sd)[2]^=(_sd)[0];\
	(_sd)[3]^=(_sd)[1];\
	(_sd)[1]^=(_sd)[2];\
	(_sd)[0]^=(_sd)[3];\
	(_sd)[2]^=_t;\
	(_sd)[3]=ROTL((_sd)[3],45);\
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
	static brru8 cur;
	static brru8 preseed[4];
	static brru8 res;
	cur = brrlib_utime();
	CPYSEED(randseed, preseed);
	res = brrlib_srand(cur);
	CPYSEED(preseed, randseed);
	return res;
}

brru8 BRRCALL
brrlib_umax(brru8 a, brru8 b)
{
	return a>b?a:b;
}
brru8 BRRCALL
brrlib_umin(brru8 a, brru8 b)
{
	return a<b?a:b;
}
brru8 BRRCALL
brrlib_uclamp(brru8 x, brru8 min, brru8 max)
{
	return x<min?min:x>max?max:x;
}
brrs8 BRRCALL
brrlib_smax(brrs8 a, brrs8 b)
{
	return a>b?a:b;
}
brrs8 BRRCALL
brrlib_smin(brrs8 a, brrs8 b)
{
	return a<b?a:b;
}
brrs8 BRRCALL
brrlib_sclamp(brrs8 x, brrs8 min, brrs8 max)
{
	return x<min?min:x>max?max:x;
}

brru8 BRRCALL
brrlib_ugcf(brru8 a, brru8 b)
{
	if (a == b)
		return a;
	else if (!a || !b)
		return 0;
	while (b != 0) {
		brru8 t = b;
		b = a % t;
		a = t;
	}
	return a;
}
brrs8 BRRCALL
brrlib_sgcf(brrs8 a, brrs8 b)
{
	if (a == b)
		return a;
	else if (!a || !b)
		return 0;
	while (b != 0) {
		brrs8 t = b;
		b = a % t;
		a = t;
	}
	return a;
}
