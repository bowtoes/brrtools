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
brrlib_alloc(void **current, brrsz size, brrbl zero)
{
	if (!current)
		return 0;
	if (size) {
		void *t;
		t = *current?realloc(*current, size):malloc(size);
		if (!t)
			return -1;
		if (zero)
			memset(t, 0, size);
		*current = t;
	} else if (*current) {
		free(*current);
		*current = NULL;
	}
	return 0;
}
