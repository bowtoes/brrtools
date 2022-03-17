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

#ifndef BRRCON_H
#define BRRCON_H

#include <brrtools/brrapi.h>

_brrcppstart

/* Pauses console until a character is pushed to stdin.
 * Returns 1.
 * */
BRRAPI int BRRCALL brrcon_pause(void);
/* Clears console and puts cursor at start position.
 * Returns 1.
 * */
BRRAPI int BRRCALL brrcon_clear(void);

_brrcppend

#endif /* BRRCON_H */
