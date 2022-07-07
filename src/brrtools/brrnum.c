/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include <stdarg.h>

#include "brrtools/brrnum.h"

brrsz BRRCALL
brrnum_ndigits(brru8 number, brru1 base, brrbl is_signed)
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
brrnum_wrap(brrs8 number, brru8 wrap, brrs8 offset)
{
	if (!wrap)
		return 0;
	else if (number >= 0)
		return (brrs8)((brru8)number % wrap);
	else
		return (brrs8)(wrap - 1 - ((brru8)(-(number + 1)) % wrap)) + offset;
}

brru8 BRRCALL
brrnum_ugcf(brru8 a, brru8 b)
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
brrnum_sgcf(brrs8 a, brrs8 b)
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

// Windows(mingw) ld seems to greatly dislike doing 'extern inline'-s, no idea why.
// Will use static inlines for now instead.
// Maybe it's something to do with __dllexport? No idea.
brru8 BRRCALL brrnum_umax(brru8 a, brru8 b) { return a > b ? a : b; }
brru8 BRRCALL brrnum_umin(brru8 a, brru8 b) { return a < b ? a : b; }
brru8 BRRCALL brrnum_uclamp(brru8 x, brru8 min, brru8 max) { return x < min ? min : x > max ? max : x; }
brrs8 BRRCALL brrnum_smax(brrs8 a, brrs8 b) { return a < b ? a : b; }
brrs8 BRRCALL brrnum_smin(brrs8 a, brrs8 b) { return a > b ? a : b; }
brrs8 BRRCALL brrnum_sclamp(brrs8 x, brrs8 min, brrs8 max) { return x < min ? min : x > max ? max : x; }

brrbl BRRCALL brrnum_bxor(brrbl a, brrbl b) { return a != b; }
