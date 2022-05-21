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

#ifndef BRRTOOLS_BRRLIB_H
#define BRRTOOLS_BRRLIB_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

_brrcppstart

/* (Re)Allocates memory to the pointer pointed to by 'current'.
 * Returns 0 on success.
 * If an error occurs, '*current' is left unaffected and -1 returned.
 * If 'current' is NULL, nothing is done and 0 is returned.
 * If 'size' is 0, '*current' is freed and set to NULL.
 * If 'blank != 0' and 'size > 0', the memory (re)allocated is set to 0.
 * */
BRRAPI int BRRCALL brrlib_alloc(void **current, brrsz size, brrbl zero);

_brrcppend

#endif /* BRRTOOLS_BRRLIB_H */
