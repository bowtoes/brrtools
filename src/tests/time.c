#include <inttypes.h>

#include "brrtools/brrtest.h"
#include "brrtools/brrtime.h"

BRRTEST_NEW_TEST(TIME_UTIME) {
	brru8 u = brrtime_utime();
	brru8 s = u / 1000000, U = u % 1000000;
	brru8 m = s / 60, S = s % 60;
	brru8 h = m / 60, M = m % 60;
	brru8 d = h / 24, H = h % 24;
	fprintf(stderr, "Time: us=%"PRIu64" %"PRIu64" days, %"PRIu64":%"PRIu64":%"PRIu64".%"PRIu64"\n", u, d, H, M, S, U);
	return "";
}
BRRTEST_NEW_TEST(TIME_SLEEP) {
	fprintf(stderr, "Sleeping for 333333 usecs (1/3 sec)...\n");
	brrtime_sleep(333333);
	return "";
}

BRRTEST_NEW_SUITE(time, 0
	,TIME_UTIME
	,TIME_SLEEP
)
