#ifndef _brrtools_brrlog_test_h
#define _brrtools_brrlog_test_h

#include <brrtools/brrapi.h>
#include <brrtools/brrtypes.h>

BRRCPPSTART

BRRAPI void BRRCALL brrtools_brrlog_test(brrsz *total, brrsz *ran, brrsz *skipped, brrsz *succeeded, brrsz *failed);

BRRCPPEND

#endif /* _brrtools_brrlog_test_h */
