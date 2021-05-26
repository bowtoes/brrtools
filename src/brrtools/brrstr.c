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

#include "brrtools/brrstr.h"

brrstrlen BRRCALL
brrstr_cstrlen(const char *const cstr, brrstrlen max_strlen)
{
	brrsz s = 0;
	if (cstr && cstr[0]) {
		for (;s < max_strlen && cstr[s]; ++s);
	}
	return s;
}

brrstrlen BRRCALL
brrstr_strlen(const brrstrT *const str)
{
	brrsz l = brrbuffer_size(str);
	if (l) l--;
	return l;
}

brrstrT BRRCALL
brrstr_new(const char *const cstr, brrstrlen max_strlen)
{
	brrsz l = brrstr_cstrlen(cstr, max_strlen);
	brrstrT s = {0};
	s = brrbuffer_new(l + 1);
	if (l)
		brrbuffer_write(&s, cstr, l);
	return s;
}

const char *BRRCALL
brrstr_cstr(const brrstrT *const str)
{
	return (const char *)brrbuffer_data(str);
}
