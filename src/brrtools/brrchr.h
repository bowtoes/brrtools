/* Copyright (c), BowToes (bow.toes@mailfence.com)
Apache 2.0 license, http://www.apache.org/licenses/LICENSE-2.0
Full license can be found in 'license' file */

#ifndef brrchr_h
#define brrchr_h

#include <stdint.h>

#include <brrtools/brrplat.h>

typedef int (* brrchr_cmp_t)(const char *const, const char *const);
typedef int (* brrchr_ncmp_t)(const char *const, const char *const, size_t);
extern const brrchr_cmp_t brrchr_cmp;
extern const brrchr_cmp_t brrchr_case_cmp;
extern const brrchr_ncmp_t brrchr_ncmp;
extern const brrchr_ncmp_t brrchr_case_ncmp;

#endif /* brrchr_h */
