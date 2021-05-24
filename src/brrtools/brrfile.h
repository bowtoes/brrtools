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

#ifndef BRRTOOLS_BRRFILE_H
#define BRRTOOLS_BRRFILE_H

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>
#include <brrtools/brrpath.h>

BRRCPPSTART

BRRAPI brru8 BRRCALL brrfile_size(const char *const file);
BRRAPI brru8 BRRCALL brrfile_fsize(int fd);

BRRCPPEND

#endif /* BRRTOOLS_BRRFILE_H */
