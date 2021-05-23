#include "brrtools/brrlib.h"

#include "brrtools/brrapi.h"
#include "brrtools/brrplatform.h"
#include "brrtools/brrmem.h"

#include <stdlib.h>
#if defined(BRRPLATFORMTYPE_WINDOWS)
# include <windows.h>
# include <malloc.h>
# include <conio.h>
#else
# include <stdio.h>
# include <termios.h>
# include <sys/select.h>
# if defined(BRRPLATFORM_AIX)
#  // https://cr.yp.to/docs/unixport.html
#  include <time.h>
# endif // BRRPLATFORM_AIX
# include <sys/time.h>
#endif

#include "brrtools/brrtypes.h"

#define BRRBASES \
    "0123456789"\
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"\

#define BRREXTBASES \
    "abcdefghijklmnopqrstuvwxyz"\
    "([{</\"'?!@#$%^&*|,.`~=+-_;:\\>}])"\

brrb1 brrlib_use_extended_bases = false;
const char brrlib_bases[] = BRRBASES BRREXTBASES;

static const brrsz maxnormalbase = sizeof(BRRBASES)-1;
static const brrsz maxextbase = (sizeof(BRREXTBASES)-1)/2;
static const brrsz maxbase = maxnormalbase + maxextbase;
brrsz BRRCALL
brrlib_max_base(void)
{
	return brrlib_use_extended_bases?maxbase:maxnormalbase;
}

brrsz BRRCALL
brrlib_print_base(char *const dst, brru8 max_length,
	    brru8 number, brrb1 is_signed, brru1 base)
{
	brrsz ns = 0;
	brrb1 isneg = false;
	if (!max_length || base < 2 || base > (brrlib_use_extended_bases?maxbase:maxnormalbase)) {
		return 0;
	} else if (!number) {
		dst[0]='0';
		dst[1]=0;
		return 1;
	} else if (is_signed && (brrs8)number < 0) {
		isneg = true;
		number = -(brrs8)number;
		dst[ns] = '-';
		ns++;
	}
	for (;ns < max_length && number; number /= base) {
		brrsz idx = number % base;
		if (idx > maxnormalbase) {
			idx = (idx - maxnormalbase) * 2 + maxnormalbase;
			dst[ns++] = brrlib_bases[idx];
			dst[ns++] = brrlib_bases[idx+1];
		} else {
			dst[ns++] = brrlib_bases[idx];
		}
	}
	brrmem_static_reverse(dst + isneg, ns - isneg);
	dst[ns] = 0;

	return ns;
}

brrb1 BRRCALL
brrlib_pause(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	getch();
	return true;
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
	return true;
#else
	return false;
#endif
}

brrb1 BRRCALL
brrlib_clear(void)
{
#if defined(BRRPLATFORMTYPE_WINDOWS)
	// https://cboard.cprogramming.com/cplusplus-programming/93806-conio-h-functions-mingw-post672795.html#post672795
	COORD a = {0,0};
	DWORD nwrite;
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07, 2000, a, &nwrite);
	return true;
#elif defined(BRRPLATFORMTYPE_UNIX)
	printf("\x1b[1;1H\x1b[2J");
	return true;
#else
	return false;
#endif
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
	// Convert windows FILETIME (100ns precision) to usec precision
	lrg.QuadPart /= 10;
	// Offset windows epoch to be UNIX epoch
	return lrg.QuadPart - 11644473838000000;
#else
	// TODO gettimeofday deprecated.
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#endif // BRRPLATFORMTYPE_WINDOWS
}

void BRRCALL
brrlib_usleep(brru8 usec)
{
	if (usec) {
#if defined(BRRPLATFORMTYPE_WINDOWS)
		HANDLE timer;
		LARGE_INTEGER ft;
		// Convert usec precision to windows FILETIME (100ns precision).
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
brrlib_ndigits(brrb1 is_signed, brru8 number, brru1 base)
{
	brrsz c = 1;
	if (is_signed && (brrs8)number < 0)
		number = (brru8)(-(brrs8)number);
	if (number == 0)
		return c;
	while (number/=base) c++; /* teehee */
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

brru8 BRRCALL brrlib_umax(brru8 a, brru8 b)
{return a>b?a:b;}
brru8 BRRCALL brrlib_umin(brru8 a, brru8 b)
{return a<b?a:b;}
brru8 BRRCALL brrlib_uclamp(brru8 x, brru8 min, brru8 max)
{return x<min?min:x>max?max:x;}
brrs8 BRRCALL brrlib_smax(brrs8 a, brrs8 b)
{return a>b?a:b;}
brrs8 BRRCALL brrlib_smin(brrs8 a, brrs8 b)
{return a<b?a:b;}
brrs8 BRRCALL brrlib_sclamp(brrs8 x, brrs8 min, brrs8 max)
{return x<min?min:x>max?max:x;}
