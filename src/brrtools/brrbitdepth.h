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

#ifndef BRRBITDEPTH_H
#define BRRBITDEPTH_H

#define BRRBITDEPTH_64BIT   64
#define BRRBITDEPTH_32BIT   32
#define BRRBITDEPTH_UNKNOWN 0

#if !defined(BRRBITDEPTH_SYSTEM) || \
	!(BRRBITDEPTH_SYSTEM == BRRBITDEPTH_32BIT || BRRBITDEPTH_SYSTEM == BRRBITDEPTH_64BIT)
# if defined(BRRBITDEPTH_SYSTEM)
#  undef BRRBITDEPTH_SYSTEM
# endif
# if defined(__LP64) || defined(__LP64__) || defined(_WIN64) || \
    defined(_M_X64) || defined(_M_IA64) || defined(_M_ARM64) || defined(_M_AMD64) || \
    defined(__ia64) || defined(__aarch64__) || \
    defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64) || \
    (defined(__x86_64__) && !defined(__ILP32__))
#  define BRRBITDEPTH_SYSTEM BRRBITDEPTH_64BIT
# else
#  define BRRBITDEPTH_SYSTEM BRRBITDEPTH_32BIT
# endif
#endif // !BRRBITDEPTH_SYSTEM

/* Enum representing different system bitdepths. */
typedef enum brrbitdepth {
	/* Value for a 64-bit system. */
	brrbitdepth_64bit = BRRBITDEPTH_64BIT,
	/* Value for a 32-bit system. */
	brrbitdepth_32bit = BRRBITDEPTH_32BIT,
	/* Value representing current system bitdepth.
	 * May be 'BRRBITDEPTH_64BIT', 'BRRBITDEPTH_32BIT', or 'BRRBITDEPTH_UNKNOWN'. */
	brrbitdepth_system = BRRBITDEPTH_SYSTEM,
} brrbitdepthT;

#endif /* BRRBITDEPTH_H */
