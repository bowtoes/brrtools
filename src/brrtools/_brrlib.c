/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

/*------------ brrapi.h */
#define _brrapi_keep_gens
#include "brrtools/brrapi.h"

#include <errno.h>
#include <string.h>

static int i_brrapi_error = 0;

void BRRCALL
brrapi_sete(int e)
{
	i_brrapi_error = e;
}
int BRRCALL
brrapi_gete(void)
{
	return i_brrapi_error;
}

const char *BRRCALL
brrapi_error_name(void)
{
	switch (i_brrapi_error) {
		#define _brrapi_e_op(_n_, _i_, _D_) case BRRAPI_E_##_n_: return _D_;
		_brrapi_e(_brrapi_e_op)
		#undef _brrapi_e_op
		default: return "Invalid Error";
	}
}
const char *BRRCALL
brrapi_err(void)
{
	switch (i_brrapi_error) {
		case BRRAPI_E_MEMERR:
		case BRRAPI_E_LIBC:
			return strerror(errno);
		case BRRAPI_E_OSERR: /* Only on windows */
			return "OS Error (to replace later)";
		default: return brrapi_error_name();
	}
}
#undef _brrapi_keep_gens

/*------------ brrcon.h */
#include "brrtools/brrcon.h"

#include <stdio.h>
#if brrplat_unix
# include <termios.h>
#elif brrplat_dos
# include <windows.h>
# include <conio.h>
#endif

int BRRCALL
brrcon_pause(void)
{
#if brrplat_dos
	getch();
#elif brrplat_unix
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
	return 1;
#endif
	return 0;
}

int BRRCALL
brrcon_clear(void)
{
#if brrplat_unix
	printf("\x1b[1;1H\x1b[2J");
#elif brrplat_dos
	/* https://cboard.cprogramming.com/cplusplus-programming/93806-conio-h-functions-mingw-post672795.html#post672795 */
	COORD a = {0,0};
	DWORD nwrite;
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07, 2000, a, &nwrite);
#else
	return 1;
#endif
	return 0;
}

/*------------ brrtime.h */
#include "brrtools/brrtime.h"

#if brrplat_unix
# include <sys/time.h>
# if brrplat_c99
#  include <inttypes.h>
# else
#  include <stdlib.h>
# endif
#elif brrplat_dos
# include <windows.h>
#endif

brru8 BRRCALL
brrtime_utime(void)
{
#if brrplat_unix
	/* TODO gettimeofday deprecated. */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#elif brrplat_dos
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
	return 0;
#endif
}

void BRRCALL
brrtime_sleep(brru8 usec)
{
	if (usec) {
#if brrplat_unix
		struct timeval tv = {0};
# if brrplat_c99
		imaxdiv_t dv = imaxdiv(usec, 1000000);
# else
		lldiv_t dv = lldiv(usec, 1000000);
# endif
		tv.tv_sec = dv.quot;
		tv.tv_usec = dv.rem;
		select(0, NULL, NULL, NULL, &tv);
#elif brrplat_dos
		HANDLE timer;
		LARGE_INTEGER ft;
		/* Convert usec precision to windows FILETIME (100ns precision). */
		ft.QuadPart = -(10 * (__int64)usec);
		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
#endif
	}
}

/*------------ brrrand.h */
#include "brrtools/brrrand.h"

#define ROTL(_x,_k) (((_x)<<(_k)) | ((_x)>>(64-(_k))))

/* Sebastiano Vigna (vigna@acm.org), 2015
 * https://xorshift.di.unimi.it/splitmix64.c
 * https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define SPLITMIX64(_seed,_rnd) do {\
	brru8 *_r = _rnd;\
	_r[0] = (_seed);\
	for (int_fast8_t i = 1; i < 4; ++i) {\
		_r[i] = (                _r[i-1] += 0x9e3779b97f4a7c15);\
		_r[i] = ((_r[i] >> 30) ^ _r[i])   * 0xbf58476d1ce4e5b9;\
		_r[i] = ((_r[i] >> 27) ^ _r[i])   * 0x94d049bb133111eb;\
		_r[i] =  (_r[i] >> 31) ^ _r[i];\
	}\
} while (0)

/* David Blackman, Sebastiano Vigna (vigna@acm.org), 2018
 * https://prng.di.unimi.it/xoshiro256plus.c
 * https://web.archive.org/web/20210430193320/https://prng.di.unimi.it/ */
#define XOSHIRO256(_rnd) do {\
	brru8 *_r = _rnd;\
	const brru8 _t = _r[1] << 17;\
	_r[2] ^= _r[0];\
	_r[3] ^= _r[1];\
	_r[1] ^= _r[2];\
	_r[0] ^= _r[3];\
	_r[2] ^= _t;\
	_r[3]  = ROTL(_r[3],45);\
} while (0)

#define GETRESULT(_rnd) ((_rnd)[0] + (_rnd)[3])

	/* Starting value is splitmix64 on 0 */
static brru8 i_rnd[4] = {0x0, 0xe220a8397b1dcdaf, 0xa706dd2f4d197e6f, 0x238275bc38fcbe91};
brru8 BRRCALL
brrrand_rand(void)
{
	brru8 res = GETRESULT(i_rnd);
	XOSHIRO256(i_rnd);
	return res;
}
void BRRCALL
brrrand_seed(brru8 seed)
{
	SPLITMIX64(seed, i_rnd);
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

/*------------ brrnum.h */
#include "brrtools/brrnum.h"

#include <stdarg.h>

int BRRCALL
brrnum_ndigits(brru8 x, int base)
{
	if (base < 2)
		return 0;
	else if (!x)
		return 1;
	brrsz c = 1;
	while (x/=base) c++;
	return c;
}
brrs8 BRRCALL
brrnum_wrap(brrs8 x, brru8 modulus, brrs8 offset)
{
	if (!modulus)
		return 0;
	else if (x >= 0)
		return (brrs8)((brru8)x % modulus);
	else
		return (brrs8)(modulus - 1 - ((brru8)(-(x + 1)) % modulus)) + offset;
}

/* assumes b > a */
static BRR_inline brru8 BRRCALL i_gcf(brru8 a, brru8 b)
{
	while (b != 0) {
		brru8 t = b;
		b = a % b;
		a = t;
	}
	return a;
}
brru8 BRRCALL
brrnum_ugcf(brru8 a, brru8 b)
{
	if (a == b)
		return a;
	else if (!a || !b)
		return 0;
	return a < b ? i_gcf(a, b) : i_gcf(b, a);
}
brrs8 BRRCALL
brrnum_sgcf(brrs8 a, brrs8 b)
{
	if (a == b)
		return a;
	else if (!a || !b)
		return 0;
	if ((a > 0 && b > 0) || (a < 0 && b < 0))
		return a < b ? i_gcf(a, b) : i_gcf(b, a);
	if (a < 0) a = -a;
	else b = -b;
	return -(a < b ? i_gcf(a, b) : i_gcf(b, a));
}

brru8 BRRCALL brrnum_umax(brru8 a, brru8 b) { return a > b ? a : b; }
brru8 BRRCALL brrnum_umin(brru8 a, brru8 b) { return a < b ? a : b; }
brru8 BRRCALL brrnum_uclamp(brru8 x, brru8 min, brru8 max) { return x < min ? min : x > max ? max : x; }
brrs8 BRRCALL brrnum_smax(brrs8 a, brrs8 b) { return a < b ? a : b; }
brrs8 BRRCALL brrnum_smin(brrs8 a, brrs8 b) { return a > b ? a : b; }
brrs8 BRRCALL brrnum_sclamp(brrs8 x, brrs8 min, brrs8 max) { return x < min ? min : x > max ? max : x; }

int BRRCALL brrnum_bxor(int a, int b) { return a != b; }

/*------------ brrhash.h */
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

/*------------ brrtypes.h */
#include "brrtools/brrtypes.h"


