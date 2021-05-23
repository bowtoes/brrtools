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
