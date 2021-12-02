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

#ifndef BRRRAND_H
#define BRRRAND_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

_brrcppstart

/* Generates the next random unsigned 64-bit integer and returns the result.
 * */
BRRAPI brru8 BRRCALL brrrand_rand(void);
/* Generates 'iter' random numbers seeded with 'seed' and returns the
 * last.
 * */
BRRAPI brru8 BRRCALL brrrand_srand(brru8 seed, brru8 iter);
/* Generates 'iter' random numbers with seeded with the current time in
 * microseconds and returns the last.
 * */
BRRAPI brru8 BRRCALL brrrand_trand(brru8 iter);

_brrcppend

#endif /* BRRRAND_H */
