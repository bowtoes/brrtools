/*
Copyright 2021 BowToes (bow.toes@mailfence.com)

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

#include "brrtools/noinstall/statics.h"

#include <stdlib.h>

#include "brrtools/brrstg.h"

int BRRCALL
brrsizestr(struct brrstg *const str, brrsz ns)
{
	if (ns != str->length) {
		void *t = realloc(str->opaque, ns+1);
		if (!t) {
			free(str->opaque);
			str->opaque = NULL;
			return 0;
		}
		str->opaque = t;
		str->length = ns;
		((char *)str->opaque)[str->length] = 0;
	}
	return 1;
}
