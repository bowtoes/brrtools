/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef _brrtools_util_h
#define _brrtools_util_h

#include "brrapi.h"
#include "brrtypes.h"

#include <string.h>

/* Returns nonzero when one of the buffers has run out of space to write/data to read */
static BRR_inline int u_cpy_limit(
	      void *restrict const dst, brrsz dstlen, brrsz *restrict const dstofs,
	const void *restrict const src, brrsz srclen, brrsz *restrict const srcofs,
	brrsz n)
{
	/* nd = 'n bytes to write to dst', ns = 'n bytes to read from source' */\

	brrsz o = dstofs ? *dstofs : 0, i = srcofs ? *srcofs : 0;
	if (o >= dstlen || i >= srclen)
		return 1;

	/* dw = 'dst available to write', sr = 'source available to read' */\
	brrsz dw = dstlen - o, sr = srclen - i;
	if (!dw) {
		if (!sr) {
			return n > 0;
		} else {
			brrsz ns = n > sr ? sr : n;
			if (srcofs)
				*srcofs += ns;
			return ns != n;
		}
	} else if (!sr) {
		return n > 0;
	} else {
		brrsz nd = n > sr ? sr : n;
		const brrsz ns = n;
		if (nd > dw)
			nd = dw;
		memcpy((char*)dst + o, (char*)src + i, nd);
		if (dstofs)
			*dstofs += nd;
		if (srcofs)
			*srcofs += ns;
		return nd != n;
	}
}


#endif /* _brrtools_util_h */
