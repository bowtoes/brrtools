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

#ifndef BRRNUM_H
#define BRRNUM_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

/* Counts the number of digits of base 'base' in 'number'.
 * If 'is_signed' is true, 'number' is cast as a signed integer, then the digits
 * are counted.
 * The sign is not counted in resulting digit count.
 * */
BRRAPI brrsz BRRCALL brrnum_ndigits(brru8 number, brru1 base, brrbl is_signed);
BRRAPI brrs8 BRRCALL brrnum_wrap(brrs8 number, brru8 wrap, brrs8 offset);

/* Returns the unsigned greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned.
 * */
BRRAPI brru8 BRRCALL brrnum_ugcf(brru8 a, brru8 b);
/* Returns the signed greatest common factor of 'a' and 'b'.
 * If 'a' equals 'b', then 'a' is returned.
 * If 'a' or 'b' are 0, then 0 is returned.
 * */
BRRAPI brrs8 BRRCALL brrnum_sgcf(brrs8 a, brrs8 b);

/* Returns the maximum of the unsigned integers 'a' and 'b'. */
static inline brru8 BRRCALL brrnum_umax(brru8 a, brru8 b)
{ return a>b?a:b; }
/* Returns the minimum of the unsigned integers 'a' and 'b'. */
static inline brru8 BRRCALL brrnum_umin(brru8 a, brru8 b)
{ return a<b?a:b; }
/* Clamps the unsigned input value 'x' within the range [min, max]. */
static inline brru8 BRRCALL brrnum_uclamp(brru8 x, brru8 min, brru8 max)
{ return x<min?min:x>max?max:x; }
/* Returns the maximum of the signed integers 'a' and 'b'. */
static inline brrs8 BRRCALL brrnum_smax(brrs8 a, brrs8 b)
{ return a>b?a:b; }
/* Returns the minimum of the signed integers 'a' and 'b'. */
static inline brrs8 BRRCALL brrnum_smin(brrs8 a, brrs8 b)
{ return a<b?a:b; }
/* Clamps the signed input value 'x' within the range [min, max]. */
static inline brrs8 BRRCALL brrnum_sclamp(brrs8 x, brrs8 min, brrs8 max)
{ return x<min?min:x>max?max:x; }

static inline brrbl BRRCALL brrnum_bxor(brrbl a, brrbl b)
{ return a != b; }

#endif /* BRRNUM_H */
