#include "tests/pack.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "brrtools/brrpack.h"
#include "brrtools/brrtest.h"

static char test_msg[2048] = {0};
#define MSG(...) (snprintf(test_msg, 2048, __VA_ARGS__), test_msg)

BRRTEST_TEST(PACK_PACK) {
	brrpackT packer = {0};
#define MESSAGE "FUCKING GOD"
	const char message[] = MESSAGE;
	fprintf(stderr, "Going to write '%s'\n", message);
	BRRTEST_ASSERT(0 == brrpack_init(&packer, NULL, 0), MSG("Could not initialize packer : %s", strerror(errno)));
	brrpack_pack_buffer(&packer, (unsigned char *)message, 8 * sizeof(message));
	fprintf(stderr, "Wrote '%s' storage=%zu fill=%zu\n", packer.buffer, packer.storage, packer.fill);
	brrpack_free(&packer);
	return "";
}

#define SUITE_NAME pack
BRRTEST_SUITE(SUITE_NAME, 0
	,PACK_PACK
)

void BRRCALL
brrtools_brrpack_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed)
{
	BRRTEST_RUN_SUITE(SUITE_NAME);
	if (total)
		*total += BRRTEST_SUITE_TOTAL(SUITE_NAME);
	if (ran)
		*ran += BRRTEST_SUITE_RUN(SUITE_NAME);
	if (skipped)
		*skipped += BRRTEST_SUITE_SKIP(SUITE_NAME);
	if (succeeded)
		*succeeded += BRRTEST_SUITE_SUCCEED(SUITE_NAME);
	if (failed)
		*failed += BRRTEST_SUITE_FAIL(SUITE_NAME);
}
