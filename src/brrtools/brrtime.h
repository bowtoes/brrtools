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

#ifndef BRRTIME_H
#define BRRTIME_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

_brrcppstart

/* Returns the current time-of-day in microseconds.
 * */
BRRAPI brru8 BRRCALL brrtime_utime(void);
/* Pauses program execution for 'usec' microseconds. */
BRRAPI void BRRCALL brrtime_sleep(brru8 usec);

_brrcppend

#endif /* BRRTIME_H */
