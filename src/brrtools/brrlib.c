/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlib.h"

/* this is totally pointless */
int BRRCALL
brrlib_alloc(void **current, brrsz size, brrbl blank)
{
	if (!current)
		return 0;

	void *p = *current;
	if (size) {
		void *t = p ? realloc(p, size) : blank ? calloc(1,size) : malloc(size);
		if (!t)
			return -1;
		p = t;
		*current = p;
	} else if (p) {
		free(p);
		p = NULL;
		*current = p;
	}
	return 0;
}
