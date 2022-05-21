/*
Copyright 2021-2022 BowToes (bow.toes@mailfence.com)

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brrtools/brrlib.h"

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
