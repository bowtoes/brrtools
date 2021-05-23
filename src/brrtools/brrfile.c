#include "brrtools/brrfile.h"

#include "brrtools/noinstall/utils.h"
#include "brrtools/brrplatform.h"
#include "brrtools/brrpath.h"

brru8 BRRCALL
brrfile_size(const char *const file)
{
	brrpath_statT st = brrpath_stat(file, true);
	return st.size;
}

brru8 BRRCALL
brrfile_fsize(int fd)
{
	brrpath_statT st = brrpath_fstat(fd);
	return st.size;
}
