#include "brrtools/brrcon.h"
#include "brrtools/brrtest.h"

BRRTEST_NEW_TEST(CON_PAUSE) {
	fprintf(stderr, "Pausing...\n");
	brrcon_pause();
	return "";
}
BRRTEST_NEW_TEST(CON_CLEAR) {
	brrcon_clear();
	fprintf(stderr, "Cleared\n");
	return "";
}
BRRTEST_NEW_SUITE(con, brrfalse
	,CON_PAUSE
	,CON_CLEAR
)
