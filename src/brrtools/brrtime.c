/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include "brrtools/brrtime.h"

#if defined(_WIN32)
# include <windows.h>
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
# include <sys/time.h>
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#  include <inttypes.h>
# endif
#endif

brru8 BRRCALL
brrtime_utime(void)
{
#if defined(_WIN32)
	FILETIME time;
	ULARGE_INTEGER lrg;
	GetSystemTimeAsFileTime(&time);
	lrg.u.LowPart = time.dwLowDateTime;
	lrg.u.HighPart = time.dwHighDateTime;
	/* Convert windows FILETIME (100ns precision) to usec precision */
	lrg.QuadPart /= 10;
	/* Offset windows epoch to be UNIX epoch (not accurate) */
	return lrg.QuadPart - 11644473838000000;
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
	/* TODO gettimeofday deprecated. */
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000000 + tv.tv_usec;
#else
	return 0;
#endif
}

void BRRCALL
brrtime_sleep(brru8 usec)
{
	if (usec) {
#if defined(_WIN32)
		HANDLE timer;
		LARGE_INTEGER ft;
		/* Convert usec precision to windows FILETIME (100ns precision). */
		ft.QuadPart = -(10 * (__int64)usec);
		timer = CreateWaitableTimer(NULL, TRUE, NULL);
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
		WaitForSingleObject(timer, INFINITE);
		CloseHandle(timer);
#elif defined(unix) || defined(__unix) || defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
		struct timeval tv = {0};
# if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
		imaxdiv_t dv = imaxdiv(usec, 1000000);
# else
		ldiv_t dv = ldiv(usec, 1000000);
# endif
		tv.tv_sec = dv.quot;
		tv.tv_usec = dv.rem;
		select(0, NULL, NULL, NULL, &tv);
#endif
	}
}
