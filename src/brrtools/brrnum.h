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

#ifndef BRRNUM_H
#define BRRNUM_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

_brrcppstart

/* Counts the number of digits of 'number' when written in base 'base'.
 * If 'is_signed' is true, 'number' is cast as a signed integer before the digits are counted.
 * The sign is not counted in resulting digit count.
 * 'base' must be greater than 2, but has no enforced upper bound.
 * */
BRRAPI brrsz BRRCALL brrnum_ndigits(brru8 number, brru1 base, brrbl is_signed);
/* Performs 'correct' (citation needed) signed % unsigned modulo operation, with an optional 'offset' that does... something.
 * */
BRRAPI brrs8 BRRCALL brrnum_wrap(brrs8 number, brru8 modulus, brrs8 offset);

/* Returns the unsigned greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' is 0, then 0 is returned.
 * */
BRRAPI brru8 BRRCALL brrnum_ugcf(brru8 a, brru8 b);
/* Returns the signed greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' is 0, then 0 is returned.
 * */
BRRAPI brrs8 BRRCALL brrnum_sgcf(brrs8 a, brrs8 b);

/* Returns the maximum of the unsigned integers 'a' and 'b'. */
BRRAPI brru8 BRRCALL brrnum_umax(brru8 a, brru8 b);
/* Returns the minimum of the unsigned integers 'a' and 'b'. */
BRRAPI brru8 BRRCALL brrnum_umin(brru8 a, brru8 b);
/* Clamps the unsigned input value 'x' within the range [min, max]. */
BRRAPI brru8 BRRCALL brrnum_uclamp(brru8 x, brru8 min, brru8 max);
/* Returns the maximum of the signed integers 'a' and 'b'. */
BRRAPI brrs8 BRRCALL brrnum_smax(brrs8 a, brrs8 b);
/* Returns the minimum of the signed integers 'a' and 'b'. */
BRRAPI brrs8 BRRCALL brrnum_smin(brrs8 a, brrs8 b);
/* Clamps the signed input value 'x' within the range [min, max]. */
BRRAPI brrs8 BRRCALL brrnum_sclamp(brrs8 x, brrs8 min, brrs8 max);

/* Logical XOR of a and b */
BRRAPI brrbl BRRCALL brrnum_bxor(brrbl a, brrbl b);

_brrcppend

#endif /* BRRNUM_H */
