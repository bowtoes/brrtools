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

#ifndef BRRSTR_H
#define BRRSTR_H

#include <brrtools/brrapi.h>
#include <brrtools/brrbuffer.h>

typedef struct brrbuffer brrstrT;

/* The length of a c-string, never including the null-terminator. */
typedef brrsz brrstrlen;

BRRAPI brrstrlen BRRCALL brrstr_cstrlen(const char *const cstr, brrstrlen max_strlen);
BRRAPI brrstrlen BRRCALL brrstr_strlen(const brrstrT *const str);

BRRAPI brrstrT BRRCALL brrstr_new(const char *const cstr, brrstrlen max_strlen);

BRRAPI const char *BRRCALL brrstr_cstr(const brrstrT *const str);

#endif /* BRRSTR_H */
